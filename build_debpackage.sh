#!/bin/bash

if [ -n "$1" -a -z "$QT_ROOT" ]; then
	export QT_ROOT=$1
fi
if [ ! -f /usr/bin/qmake -a -z "$QT_ROOT" ]; then
	echo "$0 QT_ROOT RabbitCommon_DIR"
    exit -1
fi

if [ -n "$2" -a -z "$RabbitCommon_DIR" ]; then
	export RabbitCommon_DIR=$2
fi

if [ -z "$RabbitCommon_DIR" ]; then
	export RabbitCommon_DIR=`pwd`/../RabbitCommon
fi

if [ ! -d "$RabbitCommon_DIR" ]; then
	echo "$0 QT_ROOT RabbitCommon_DIR"
        exit -2
fi

if [ -z "$BUILD_TYPE" ]; then
    export BUILD_TYPE=Release
fi

export PATH=$QT_ROOT/bin:$PATH
export LD_LIBRARY_PATH=$QT_ROOT/lib:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=$QT_ROOT/lib/pkgconfig:$PKG_CONFIG_PATH
#fakeroot debian/rules binary
#dpkg-buildpackage -us -uc -b  #The -us -uc tell it there is no need to GPG sign the package. the -b is build binary
dpkg-buildpackage -us -uc   #The -us -uc tell it there is no need to GPG sign the package
