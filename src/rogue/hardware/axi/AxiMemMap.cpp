/**
 *-----------------------------------------------------------------------------
 * Title      : AXI Memory Mapped Access
 * ----------------------------------------------------------------------------
 * File       : AxiMemMap.cpp
 * Created    : 2017-03-21
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
#include <rogue/hardware/axi/AxiMemMap.h>
#include <rogue/interfaces/memory/Constants.h>
#include <rogue/GeneralError.h>
#include <boost/make_shared.hpp>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <DataDriver.h>

namespace rha = rogue::hardware::axi;
namespace rim = rogue::interfaces::memory;
namespace bp  = boost::python;

//! Class creation
rha::AxiMemMapPtr rha::AxiMemMap::create (std::string path) {
   rha::AxiMemMapPtr r = boost::make_shared<rha::AxiMemMap>(path);
   return(r);
}

//! Creator
rha::AxiMemMap::AxiMemMap(std::string path) : rim::Slave(4,0xFFFFFFFF) {
   fd_ = ::open(path.c_str(), O_RDWR);
   log_ = new rogue::Logging("axi.AxiMemMap");
   if ( fd_ < 0 ) throw(rogue::GeneralError::open("AxiMemMap::AxiMemMap",path));
}

//! Destructor
rha::AxiMemMap::~AxiMemMap() {
   ::close(fd_);
}

//! Post a transaction
void rha::AxiMemMap::doTransaction(uint32_t id, boost::shared_ptr<rogue::interfaces::memory::Master> master, 
                                uint64_t address, uint32_t size, uint32_t type) {
   uint32_t count;
   uint32_t data;
   int32_t  ret;

   count = 0;
   ret = 0;

   while ( (ret == 0 ) && (count < size) ) {
      if (type == rim::Write || type == rim::Post) {
         master->getTransactionData(id,&data,count,4);
         ret = dmaWriteRegister(fd_,address+count,data);
      }
      else {
         ret = dmaReadRegister(fd_,address+count,&data);
         master->setTransactionData(id,&data,count,4);
      }
      count += 4;
   }

   if ( ret != 0 ) 
      throw rogue::GeneralError::create("AxiMemMap::doTransaction","Error accessing register space. Check driver permissions");

   log_->debug("Transaction id=0x%08x, addr 0x%08x. Size=%i, type=%i, data=0x%08x",id,address,size,type,data);
   master->doneTransaction(id,(ret==0)?0:1);
}

void rha::AxiMemMap::setup_python () {

   bp::class_<rha::AxiMemMap, rha::AxiMemMapPtr, bp::bases<rim::Slave>, boost::noncopyable >("AxiMemMap",bp::init<std::string>())
      .def("create",         &rha::AxiMemMap::create)
      .staticmethod("create")
   ;

   bp::implicitly_convertible<rha::AxiMemMapPtr, rim::SlavePtr>();
}
