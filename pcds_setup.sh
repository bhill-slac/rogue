# source this file to setup pcds environment for building rogue w/o python
source $PSPKG_ROOT/etc/env_add_pkg.sh cmake/3.15.0
source $PSPKG_ROOT/etc/env_add_pkg.sh zeromq/4.1.5

# rogue is built w/ cmake
# To bootstart from fresh checkout
# mkdir build
# cd build
# cmake .. -DROGUE_INSTALL=local -DNO_EPICS=TRUE -DNO_PYTHON=TRUE -DSTATIC_LIB=TRUE -DSHARED_LIB=TRUE
# make
# make install

