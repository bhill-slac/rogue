/**
 *-----------------------------------------------------------------------------
 * Title      : Stream interface master
 * ----------------------------------------------------------------------------
 * File       : Master.h
 * Author     : Ryan Herbst, rherbst@slac.stanford.edu
 * Created    : 2016-09-16
 * Last update: 2016-09-16
 * ----------------------------------------------------------------------------
 * Description:
 * Stream interface master
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
#include <unistd.h>
#include <interfaces/stream/Slave.h>
#include <interfaces/stream/Master.h>
#include <interfaces/stream/Frame.h>

#include <boost/python.hpp>
#include <boost/make_shared.hpp>

namespace ris = rogue::interfaces::stream;

//! Class creation
ris::MasterPtr ris::Master::create () {
   ris::MasterPtr msg = boost::make_shared<ris::Master>();
   return(msg);
}

//! Creator
ris::Master::Master() {
   primary_ = ris::Slave::create();
}

//! Destructor
ris::Master::~Master() {
   slaves_.clear();
}

//! Set primary slave, used for buffer request forwarding
void ris::Master::setSlave ( boost::shared_ptr<interfaces::stream::Slave> slave ) {
   slaveMtx_.lock();
   slaves_.push_back(slave);
   primary_ = slave;
   slaveMtx_.unlock();
}

//! Add secondary slave
void ris::Master::addSlave ( ris::SlavePtr slave ) {
   slaveMtx_.lock();
   slaves_.push_back(slave);
   slaveMtx_.unlock();
}

//! Request frame from primary slave
ris::FramePtr ris::Master::reqFrame ( uint32_t size, bool zeroCopyEn, uint32_t timeout) {

   slaveMtx_.lock();
   ris::SlavePtr p = primary_;
   slaveMtx_.unlock();

   return(p->acceptReq(size,zeroCopyEn,timeout));
}

//! Push frame to slaves
bool ris::Master::sendFrame ( FramePtr frame, uint32_t timeout) {
   uint32_t x;
   bool     ret;

   slaveMtx_.lock();
   if ( slaves_.size() == 0 ) return(false);

   ret = true;

   for (x=0; x < slaves_.size(); x++) {
      if ( slaves_[x]->acceptFrame(frame,timeout) == false ) ret = false;
   }
   slaveMtx_.unlock();

   return(ret);
}
