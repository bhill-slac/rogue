/**
 *-----------------------------------------------------------------------------
 * Title      : Python Package
 * ----------------------------------------------------------------------------
 * File       : package.cpp
 * Author     : Ryan Herbst, rherbst@slac.stanford.edu
 * Created    : 2016-08-08
 * Last update: 2016-08-08
 * ----------------------------------------------------------------------------
 * Description:
 * Python package setup
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

#include <boost/python.hpp>
#include <rogue/exceptions/module.h>
#include <rogue/interfaces/module.h>
#include <rogue/hardware/module.h>
#include <rogue/utilities/module.h>
#include <rogue/protocols/module.h>

BOOST_PYTHON_MODULE(rogue) {

   PyEval_InitThreads();

   rogue::exceptions::setup_module();
   rogue::interfaces::setup_module();
   rogue::protocols::setup_module();
   rogue::hardware::setup_module();
   rogue::utilities::setup_module();

};
