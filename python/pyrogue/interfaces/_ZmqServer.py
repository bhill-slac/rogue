#!/usr/bin/env python
#-----------------------------------------------------------------------------
# Title      : PyRogue ZMQ Server
#-----------------------------------------------------------------------------
# File       : pyrogue/interfaces/_ZmqServer.py
# Created    : 2017-06-07
#-----------------------------------------------------------------------------
# This file is part of the rogue software platform. It is subject to 
# the license terms in the LICENSE.txt file found in the top-level directory 
# of this distribution and at: 
#    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
# No part of the rogue software platform, including this file, may be 
# copied, modified, propagated, or distributed except according to the terms 
# contained in the LICENSE.txt file.
#-----------------------------------------------------------------------------
import rogue.interfaces
import pyrogue
import jsonpickle

class ZmqServer(rogue.interfaces.ZmqServer):

    def __init__(self,*,root,addr,port):
        rogue.interfaces.ZmqServer.__init__(self,addr,port)
        self._root = root

    def encode(self,data,rawStr):

        if rawStr and isinstance(data,str):
            return data
        else:
            return jsonpickle.encode(data)

    def _doRequest(self,data):
        try:
            d = jsonpickle.decode(data)

            path    = d['path']   if 'path'   in d else None
            attr    = d['attr']   if 'attr'   in d else None
            args    = d['args']   if 'args'   in d else ()
            kwargs  = d['kwargs'] if 'kwargs' in d else {}
            rawStr  = d['rawStr'] if 'rawStr' in d else False

            # Special case to get name
            if path == "__rootname__":
                return self.encode(self._root.name,rawStr=rawStr)

            # Special case to get structure
            if path == "__structure__":
                return self._root._structure

            node = self._root.getNode(path)

            if node is None:
                return self.encode(None,rawStr=False)

            nAttr = getattr(node, attr)

            if nAttr is None:
                return self.encode(None,rawStr=False)
            elif callable(nAttr):
                return self.encode(nAttr(*args,**kwargs),rawStr=rawStr)
            else:
                return self.encode(nAttr,rawStr=rawStr)

        except Exception as msg:
            print("ZMQ Got Exception: {}".format(msg))
            return 'null\n'

