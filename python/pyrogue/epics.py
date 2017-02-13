#!/usr/bin/env python
#-----------------------------------------------------------------------------
# Title      : PyRogue epics support
#-----------------------------------------------------------------------------
# File       : pyrogue/epics.py
# Author     : Ryan Herbst, rherbst@slac.stanford.edu
# Created    : 2016-09-29
# Last update: 2016-09-29
#-----------------------------------------------------------------------------
# Description:
# Module containing epics support classes and routines
#-----------------------------------------------------------------------------
# This file is part of the rogue software platform. It is subject to 
# the license terms in the LICENSE.txt file found in the top-level directory 
# of this distribution and at: 
#    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
# No part of the rogue software platform, including this file, may be 
# copied, modified, propagated, or distributed except according to the terms 
# contained in the LICENSE.txt file.
#-----------------------------------------------------------------------------
import threading
import pyrogue
import time
import pcaspy

try:
   import queue
except ImportError:
   import Queue as queue


class EpicsCaDriver(pcaspy.Driver):
    def __init__(self,queue):
        pcaspy.Driver.__init__(self)
        self._q = queue

    def write(self,reason,value):
        path = reason.replace(':','.')
        entry = {'path':path,'value':value,'epath':reason}
        self._q.put(entry)
        self.setParam(reason,value)


class EpicsCaServer(object):
    """
    Class to contain an epics ca server
    """
    def __init__(self,base,root):
        self._root    = root
        self._base    = base 
        self._runEn   = True
        self._server  = None
        self._driver  = None
        self._queue   = queue.Queue()
        self._wThread = None
        self._eThread = None
        self._pvdb    = {}

        if self._root:
            self._root.addVarListener(self._variableStatus)

    def stop(self):
        self._runEn = False
        self._wThread.join()
        self._eThread.join()
        self._wThread = None
        self._eThread = None

    def start(self):
        self._runEn = True
        self._eThread = threading.Thread(target=self._epicsRun)
        self._wThread = threading.Thread(target=self._workRun)
        self._eThread.start()
        self._wThread.start()

    def _addPv(self,node):
        d = {}
        if node.base == 'enum': 
            d['type'] = 'enum'
            d['enums'] = [val for key,val in node.enum.items()] 
        elif node.base == 'bool':
            d['type'] = 'enum'
            d['enums'] = ['False','True']
        elif node.base == 'float':
            d['type'] = 'float'
        elif node.base == 'uint' or node.base == 'hex' or node.base == 'bin':
            d['type'] = 'int'
        elif node.base == 'range':
            d['type'] = 'int'
            d['lolim'] = node.minimum
            d['hilim'] = node.maximum
        else:
            d['type'] = 'string'

        name = node.path.replace('.',':')
        self._pvdb[name] = d

    def _addDevice(self,node):

        # Get variables 
        for key,value in node.variables.items():
            self._addPv(value)

        # Get commands
        for key,value in node.commands.items():
            self._addPv(value)

        # Get devices
        for key,value in node.devices.items():
            self._addDevice(value)

    def _epicsRun(self):
        self._server = pcaspy.SimpleServer()

        # Create PVs
        self._addDevice(self._root)

        # Add variable for structure
        sname = self._root.name + ':' + 'structure'
        self._pvdb[sname] = {'type':'string'}

        # Create PVs
        self._server.createPV(self._base + ':',self._pvdb)
        self._driver = EpicsCaDriver(self._queue)

        # Load structure string
        s = self._root.getYamlStructure()
        self._driver.setParam(sname,s)

        while(self._runEn):
            self._server.process(0.5)

    def _workRun(self):
        while(self._runEn):
            try:
                e = self._queue.get(True,0.5)
                epath = e['epath']
                path  = e['path']
                value = e['value']

                if self._pvdb[epath]['type'] == 'enum':
                    v = self._pvdb[epath]['enums'][value]
                else:
                    v = e['value']

                self._root.setOrExecPath(path,v)
            except:
                pass

    # Variable field updated on server
    def _variableStatus(self,yml,d):
        if not self._driver: return
        self._walkDict(None,d)
        self._driver.updatePVs()

    def _walkDict(self,currPath,d):
        for key,value in d.items():

            if currPath: locPath = currPath + ':' + key
            else: locPath = key

            if isinstance(value,dict):
                self._walkDict(locPath,value)
            else:
                if self._pvdb[locPath]['type'] == 'enum':
                    v = self._pvdb[locPath]['enums'].index(str(value))
                else:
                    v =value

                self._driver.setParam(locPath,v)

