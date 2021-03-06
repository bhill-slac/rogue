.. _interfaces_stream_using_filter:

==============
Using A Filter
==============

A :ref:`interfaces_stream_filter` object provides a mechanism for selection a particular channel from 
a stream Master which generates channelized Frames. This a rate situation which only occurs when
receiving a stream from a rogue.utilities.StreamReader or from the output of the 
rogue.protocols.batcher.SplitterV1 objects. The Filter object only passes through frames which
have a configured channel id. The Frame filter can also be configured to drop frames which
have a non zero error field. This may be a usefull utlility for non-channelized data as well.

Filter Example
==============

The following python example shows how to read channel 1 data from a data file.

.. code-block:: python

   import rogue.interfaces.stream
   import rogue.utilities.fileio
   import pyrogue
   import pyrogue.utilities.fileio

   # Data file reader, using pyrogue wrapper
   src = pyrogue.utilities.fileio.StreamReader()

   # Filter, channel=1, drop errors
   filt = rogue.interfaces.stream.Filter(1,True)

   # Data destination
   dst = MyCustomSlave()

   # Connect the source to the Filter
   pyrogue.streamConnect(src, filt)
   
   # Connect the filter to the destination
   pyrogue.streamConnect(filt, dst)

   src.open("MyDataFile.bin")

Below is the equivalent code in C++

.. code-block:: c

   #include <rogue/interfaces/stream/Filter.h>
   #include <rogue/utilities/fileio/StreamReader.h>
   #include <rogue/Helpers.h>
   #include <MyCustomMaster.h>
   #include <MyCustomSlave.h>

   # File Reader
   rogue::utilities::fileio::StreamReaderPtr src = rogue::utilities::fileio::StreamReader::create();

   # Filter
   rogue::interfaces::stream::FilterPtr filt = rogue::interfaces::stream::Filter::create(1,true);

   # Data destination
   MyCustomSlavePtr dst = MyCustomSlave::create();

   // Connect the source to the filter
   streamConnect(src, filt);

   // Connect the filter to the destination
   streamConnect(filt, dst);

   src->open("MyDataFile.bin");

