.. _interfaces_stream_connecting:

==================
Connecting Streams
==================

The stream interface is made up of a master which is the source of frame data
and a slave which is a receiver of frame data. Each master can be connected to 
one or more slaves, with one slave connected as the primary slave.  The concept of a
primary slave is important because in most cases it is the slave from which the master
will request new frames. The primary slave is also the last slave to receive data
from the master. This is an important distinction when interfacing to a slave which will
empty the frame when processing the data, such as a DMA driver in zero copy mode.

A stream master and slave are connected using the following command in python:

.. code-block:: python

   import pyrogue

   pyrogue.streamConnect(myMaster, mySlave)

A similiar line of code is used to connect a master and slave in c++:

.. code-block:: c

   #include <rogue/Helpers.h>

   streamConnect(myMaster, mySlave)

The above command examples attaches the primary slave to the master.

Additional receivers can be added to a master using a streamTap function. This allows more than
one Slave to receive data from a Master. Each tapped slave will be passed the frame in turn before
it is passed to the primary slave:

.. code-block:: python

   import pyrogue

   pyrogue.streamTap(myMaster, mySlave)

And in C++:

.. code-block:: c

   #include <rogue/Helpers.h>

   streamTap(myMaster, mySlave)

In some cases rogue entties can serve as both a stream Master and Slave. This is often the case when
using a network protocol such as UDP or TCP. Two dual purpose enpoints can be connected together
to create a bi-directional data stream using the following command in python:

.. code-block:: python

   import pyrogue

   pyrogue.streamConnectBiDir(enPointA, endPointB)

This command sets endPointB as the primary Slave for endPointA at the same time setting endPointA as the
primary slave for endPointB. A similiar command is available in C++:

.. code-block:: c

   #include <rogue/Helpers.h>

   streamConnectBiDir(endPointA, endPointB)

