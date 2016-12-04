#!/usr/bin/env python
#-----------------------------------------------------------------------------
# Title      : Command windows for GUI
#-----------------------------------------------------------------------------
# File       : pyrogue/gui/commands.py
# Author     : Ryan Herbst, rherbst@slac.stanford.edu
# Created    : 2016-10-03
# Last update: 2016-10-03
#-----------------------------------------------------------------------------
# Description:
# Module for functions and classes related to command display in the rogue GUI
#-----------------------------------------------------------------------------
# This file is part of the rogue software platform. It is subject to 
# the license terms in the LICENSE.txt file found in the top-level directory 
# of this distribution and at: 
#    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
# No part of the rogue software platform, including this file, may be 
# copied, modified, propagated, or distributed except according to the terms 
# contained in the LICENSE.txt file.
#-----------------------------------------------------------------------------
from PyQt4.QtCore   import *
from PyQt4.QtGui    import *
from PyQt4.QtWebKit import *

import pyrogue


class CommandLink(QObject):
    """Bridge between the pyrogue tree and the display element"""

    def __init__(self,parent,command):
        QObject.__init__(self)
        self.command = command

        item = QTreeWidgetItem(parent)
        parent.addChild(item)
        item.setText(0,command.name)
        item.setText(1,command.base)

        pb = QPushButton('Execute')
        item.treeWidget().setItemWidget(item,2,pb)
        pb.clicked.connect(self.execPressed)

        if command.base != 'None':
            self.widget = QLineEdit()
            item.treeWidget().setItemWidget(item,3,self.widget)
        else:
            self.widget = None

    def execPressed(self):
        if self.widget != None:
            value = str(self.widget.text())
        else:
            value=None

        if self.command.base == 'hex':
            try:
                self.command(int(value,16))
            except Exception:
                pass

        elif self.command.base == 'bin':
            try:
                self.command(int(value,2))
            except Exception:
                pass
            
        elif self.command.base == 'uint':
            try:
                self.command(int(value))
            except Exception:
                pass

        elif self.command.base == 'float':
            try:
                self.command(float(value))
            except Exception:
                pass

        elif self.command.base == 'string':
            self.command(value)

        else:
            self.command()


class CommandWidget(QWidget):
    def __init__(self, group, parent=None):
        super(CommandWidget, self).__init__(parent)

        self.roots = []
        self.commands = []

        vb = QVBoxLayout()
        self.setLayout(vb)
        self.tree = QTreeWidget()
        vb.addWidget(self.tree)

        self.tree.setColumnCount(2)
        self.tree.setHeaderLabels(['Command','Base','Execute','Arg'])

        self.top = QTreeWidgetItem(self.tree)
        self.top.setText(0,group)
        self.tree.addTopLevelItem(self.top)
        self.top.setExpanded(True)

        hb = QHBoxLayout()
        vb.addLayout(hb)

    def addTree(self,root):
        self.roots.append(root)

        r = QTreeWidgetItem(self.top)
        r.setText(0,root.name)
        r.setExpanded(True)
        self.addTreeItems(r,root)

        for i in range(0,3):
            self.tree.resizeColumnToContents(i)

    def addTreeItems(self,tree,d):

        # First create commands
        for key,val in d.getNodes(pyrogue.Command).iteritems():
            if not val.hidden:
                self.commands.append(CommandLink(tree,val))

        # Then create devices
        for key,val in d.getNodes(pyrogue.Device).iteritems():
            if not val.hidden:
                w = QTreeWidgetItem(tree)
                w.setText(0,val.name)
                w.setExpanded(True)
                self.addTreeItems(w,val)
