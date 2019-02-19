/**
 *-----------------------------------------------------------------------------
 * Title      : Stream Network Core
 * ----------------------------------------------------------------------------
 * File       : TcpCore.h
 * Created    : 2019-01-30
 * ----------------------------------------------------------------------------
 * Description:
 * Stream Network Core
 * ----------------------------------------------------------------------------
 * This file is part of the rogue software platform. It is subject to 
 * the license terms in the LICENSE.txt file found in the top-level directory 
 * of this distribution and at: 
 *    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
 * No part of the rogue software platform, including this file, may be 
 * copied, modified, propagated, or distributed except according to the terms 
 * contained in the LICENSE.txt file.
 * ----------------------------------------------------------------------------
**/
#ifndef __ROGUE_INTERFACES_STREAM_TCP_CORE_H__
#define __ROGUE_INTERFACES_STREAM_TCP_CORE_H__
#include <rogue/interfaces/stream/Master.h>
#include <rogue/interfaces/stream/Slave.h>
#include <rogue/interfaces/stream/Frame.h>
#include <rogue/Logging.h>
#include <boost/thread.hpp>
#include <stdint.h>

namespace rogue {
   namespace interfaces {
      namespace stream {

         //! Stream TCP Bridge Core
         /** This class implements the core functionality of the TcpClient and TcpServer
          * classes which implment a Rogue stream bridge over a TCP network. This core
          * can operation in either client or server mode. The TcpClient and TcpServer
          * classes are thin wrapper which define which mode flag to pass to this base
          * class.
          */
         class TcpCore : public rogue::interfaces::stream::Master, 
                         public rogue::interfaces::stream::Slave {

            protected:

               // Inbound Address
               std::string pullAddr_;

               // Outbound Address
               std::string pushAddr_;

               // Zeromq Context
               void * zmqCtx_;

               // Zeromq inbound port
               void * zmqPull_;

               // Zeromq outbound port
               void * zmqPush_;

               // Thread background
               void runThread();

               // Log
               boost::shared_ptr<rogue::Logging> bridgeLog_;

               // Thread
               boost::thread * thread_;

               // Lock
               boost::mutex bridgeMtx_;

            public:

               //! Create a TcpCore object and return as a TcpCorePtr
               /**The creator takes an address, port and server mode flag. The passed
                * address can either be an IP address or hostname. When running in server
                * mode the address string defines which network interface the socket server
                * will listen on. A string of "*" results in all network interfaces being
                * listened on. The stream bridge requires two TCP ports. The pased port is the 
                * base number of these two ports. A passed value of 8000 will result in both
                * 8000 and 8001 being used by this bridge.
                *
                * Not exposed to Python
                * @param addr Interface address for server, remote server address for client.
                * @param port Base port number of use for connection.
                * @param server Server flag. Set to True to run in server mode.
                * @return TcpCore object as a TcpCorePtr
                */
               static boost::shared_ptr<rogue::interfaces::stream::TcpCore> 
                  create (std::string addr, uint16_t port, bool server);

               //! Setup class for use in python
               /** Not exposed to Python
                */
               static void setup_python();

               //! Create a TcpCore object
               /**The creator takes an address, port and server mode flag. The passed
                * address can either be an IP address or hostname. When running in server
                * mode the address string defines which network interface the socket server
                * will listen on. A string of "*" results in all network interfaces being
                * listened on. The stream bridge requires two TCP ports. The pased port is the 
                * base number of these two ports. A passed value of 8000 will result in both
                * 8000 and 8001 being used by this bridge.
                *
                * Not exposed to Python
                * @param addr Interface address for server, remote server address for client.
                * @param port Base port number of use for connection.
                * @param server Server flag. Set to True to run in server mode.
                */
               TcpCore(std::string addr, uint16_t port, bool server);

               //! Destroy the TcpCore
               ~TcpCore();

               //! Accept a frame from master
               /** This method is called by the Master object to which this Slave is attached when
                * passing a Frame.
                * @param frame Frame pointer (FramePtr)
                */
               void acceptFrame ( boost::shared_ptr<rogue::interfaces::stream::Frame> frame );
         };

         //! Alias for using shared pointer as TcpCorePtr
         typedef boost::shared_ptr<rogue::interfaces::stream::TcpCore> TcpCorePtr;

      }
   }
};

#endif
