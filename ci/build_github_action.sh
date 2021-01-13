#!/bin/bash
set -e

echo "$0 [SOURCE_DIR] [TOOLS_DIR] [ThirdLibs_DIR] [ThirdLibs_SOURCE_DIR]"

SOURCE_DIR=`pwd`
if [ -n "$1" ]; then
    SOURCE_DIR=$1
fi
TOOLS_DIR=${SOURCE_DIR}/Tools
if [ -n "$2" ]
    TOOLS_DIR=$2
fi
ThirdLibs_DIR=${TOOLS_DIR}/ThirdLibs
if [ -n "$3" ]
    ThirdLibs_DIR=$3
fi
ThirdLibs_SOURCE_DIR=${TOOLS_DIR}/ThirdLibsSrc
if [ -n "$4" ]
    ThirdLibs_SOURCE_DIR=$4
fi
if [ ! -d "$ThirdLibs_SOURCE_DIR" ]; then
    mkdir -p $ThirdLibs_SOURCE_DIR
fi

cd ${ThirdLibs_SOURCE_DIR}
git clone https://github.com/KangLin/RabbitCommon.git
export RabbitCommon_DIR="${ThirdLibs_SOURCE_DIR}/RabbitCommon"
cd ${ThirdLibs_DIR}
if [ -f ${ThirdLibs_DIR}/change_prefix.sh ]; then
    cd ${ThirdLibs_DIR}
    ./change_prefix.sh
fi

function version_gt() { test "$(echo "$@" | tr " " "\n" | sort -V | head -n 1)" != "$1"; }
function version_le() { test "$(echo "$@" | tr " " "\n" | sort -V | head -n 1)" == "$1"; }
function version_lt() { test "$(echo "$@" | tr " " "\n" | sort -rV | head -n 1)" != "$1"; }
function version_ge() { test "$(echo "$@" | tr " " "\n" | sort -rV | head -n 1)" == "$1"; }

MAKE_JOB_PARA="-j`cat /proc/cpuinfo |grep 'cpu cores' |wc -l`"  #make 同时工作进程参数
if [ "$MAKE_JOB_PARA" = "-j1" ];then
    MAKE_JOB_PARA=""
fi

QT_ROOT=${Qt5_dir}

cd ${SOURCE_DIR}

mkdir -p build
cd build

if [ "${BUILD_TARGERT}" = "android" ]; then
    if [ -n "${ANDROID_ARM_NEON}" ]; then
        CONFIG_PARA="${CONFIG_PARA} -DANDROID_ARM_NEON=${ANDROID_ARM_NEON}"
    fi
    if [ -d "$ThirdLibs_DIR" ]; then
        CONFIG_PARA="${CONFIG_PARA} -DOPENSSL_ROOT_DIR=$ThirdLibs_DIR"
    fi
    cmake -G"${GENERATORS}" ${SOURCE_DIR} ${CONFIG_PARA} \
        -DCMAKE_INSTALL_PREFIX=`pwd`/android-build \
        -DCMAKE_VERBOSE_MAKEFILE=ON \
        -DCMAKE_BUILD_TYPE=Release \
        -DQt5_DIR=${QT_ROOT}/lib/cmake/Qt5 \
        -DQt5Core_DIR=${QT_ROOT}/lib/cmake/Qt5Core \
        -DQt5Gui_DIR=${QT_ROOT}/lib/cmake/Qt5Gui \
        -DQt5Widgets_DIR=${QT_ROOT}/lib/cmake/Qt5Widgets \
        -DQt5Xml_DIR=${QT_ROOT}/lib/cmake/Qt5Xml \
        -DQt5Sql_DIR=${QT_ROOT}/lib/cmake/Qt5Sql \
        -DQt5Network_DIR=${QT_ROOT}/lib/cmake/Qt5Network \
        -DQt5Multimedia_DIR=${QT_ROOT}/lib/cmake/Qt5Multimedia \
        -DQt5LinguistTools_DIR=${QT_ROOT}/lib/cmake/Qt5LinguistTools \
        -DQt5AndroidExtras_DIR=${QT_ROOT}/lib/cmake/Qt5AndroidExtras \
        -DANDROID_PLATFORM=${ANDROID_API} \
        -DANDROID_ABI="${BUILD_ARCH}" \
        -DCMAKE_MAKE_PROGRAM=make \
        -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake 
    
    cmake --build . --config MinSizeRel -- ${MAKE_JOB_PARA}
    cmake --build . --config MinSizeRel --target install-runtime -- ${MAKE_JOB_PARA}
    
else
    echo "cmake -G\"${GENERATORS}\" ${SOURCE_DIR} ${CONFIG_PARA} 
        -DCMAKE_INSTALL_PREFIX=`pwd`/install 
        -DCMAKE_VERBOSE_MAKEFILE=ON 
        -DCMAKE_BUILD_TYPE=Release 
        -DQt5_DIR=${QT_ROOT}/lib/cmake/Qt5"
    cmake -G"${GENERATORS}" ${SOURCE_DIR} ${CONFIG_PARA} \
        -DCMAKE_INSTALL_PREFIX=`pwd`/install \
        -DCMAKE_VERBOSE_MAKEFILE=ON \
        -DCMAKE_BUILD_TYPE=Release \
        -DQt5_DIR=${QT_ROOT}/lib/cmake/Qt5
    
    cmake --build . --config Release -- ${MAKE_JOB_PARA}
    cmake --build . --config Release --target install-runtime
fi
