#!/usr/bin/env python

import rogue.hardware.pgp
import rogue.utilities 
import rogue.interfaces.stream
import pyrogue
import time


fwr = rogue.utilities.fileio.StreamWriter()
fwr.setBufferSize(100004)
fwr.setMaxSize(1000003)

prbsA = rogue.utilities.Prbs()
pyrogue.streamConnect(prbsA,fwr.getPort(0x5,0x8))

fwr.open("test.dat")

prbsA.enable(1000)
time.sleep(5)
prbsA.disable()
time.sleep(1)
fwr.close()

print("Generated: Count %i, Bytes %i" % (prbsA.getTxCount(),prbsA.getTxBytes()))
print("     File: Count %i, Bytes %i" % (fwr.getBankCount(),fwr.getSize()))

frd = rogue.utilities.fileio.StreamReader()

prbsB = rogue.utilities.Prbs()
pyrogue.streamConnect(frd,prbsB)

frd.open("test.dat.1")
#frd.open("test.dat")

while (True):

   print("")
   print(" Dest: Count %i, Bytes %i, Alloc %i, Errors %i" % (prbsB.getRxCount(),prbsB.getRxBytes(),prbsB.getAllocBytes(),prbsB.getRxErrors()))
   time.sleep(1)

