#!/bin/bash
# Author: Kang Lin <kl222@126.com>

#See: https://blog.csdn.net/alwaysbefine/article/details/114187380
#set -x
set -e
#set -v

APT=
APT_UPDATE=0
BASE_LIBS=0
DEFAULT_LIBS=0
QT=0
FREERDP=0
TIGERVNC=0
PCAPPLUSPLUS=0
RabbitCommon=0
libdatachannel=0
QtService=0

usage_long() {
    echo "$0 [--install=<install directory>] [--source=<source directory>] [--tools=<tools directory>] [--build=<build directory>] [--apt=<'lib1 lib2 ...'>] [--apt_update=[0|1]] [--base[=0|1]] [--default[=0|1]] [--qt[=0|1]] [--rabbitcommon[=0|1]] [--freerdp[=0|1]] [--tigervnc[=0|1]] [--pcapplusplus[=0|1]] [--libdatachannel=[0|1]] [--QtService[0|1]]"
    echo "Directory:"
    echo "  --install: Set install directory"
    echo "  --source: Set source directory"
    echo "  --tools: Set tools directory"
    echo "  --build: set build directory"
    echo "Depend:"
    echo "  --base: Install the base libraries with apt"
    echo "  --default: Install the default dependency libraries that comes with the system"
    echo "  --apt_update: Update system"
    echo "  --apt: Install package with apt"
    echo "  --qt: Install QT"
    echo "  --rabbitcommon: Install RabbitCommon"
    echo "  --freerdp: Install FreeRDP"
    echo "  --tigervnc: Install TigerVNC"
    echo "  --pcapplusplus: Install PcapPlusPlus"
    echo "  --libdatachannel: Install libdatachannel"
    echo "  --QtService: Install QtService"
}

usage() {
    echo "$0 [-i <install directory>] [-s <source directory>] [-t <tools directory>] [-b <build directory>] [-a <'lib1 lib2 ...'>] [apt_update] [base] [default] [qt] [rabbitcommon] [freerdp] [tigervnc] [pcapplusplus] [libdatachannel] [QtService]"
    echo "Directory:"
    echo "  -i: Set install directory"
    echo "  -s: Set source directory"
    echo "  -t: Set tools directory"
    echo "  -b: set build directory"
    echo "Depend:"
    echo "  base: Install then base libraries with apt"
    echo "  default: Install the default dependency libraries that comes with the system"
    echo "  apt_update: Update system"
    echo "  -a: Install package with apt"
    echo "  qt: Install QT"
    echo "  rabbitcommon: Install RabbitCommon"
    echo "  freerdp: Install FreeRDP"
    echo "  tigervnc: Install TigerVNC"
    echo "  pcapplusplus: Install PcapPlusPlus"
    echo "  libdatachannel: Install libdatachannel"
    echo "  QtService: Install QtService"
}

# [如何使用getopt和getopts命令解析命令行选项和参数](https://zhuanlan.zhihu.com/p/673908518)
# [【Linux】Shell命令 getopts/getopt用法详解](https://blog.csdn.net/arpospf/article/details/103381621)
if command -V getopt >/dev/null; then
    echo "getopt is exits"
    #echo "original parameters=[$@]"
    # -o 或 --options 选项后面是可接受的短选项，如 ab:c:: ，表示可接受的短选项为 -a -b -c ，
    # 其中 -a 选项不接参数，-b 选项后必须接参数，-c 选项的参数为可选的
    # 后面没有冒号表示没有参数。后跟有一个冒号表示有参数。跟两个冒号表示有可选参数。
    # -l 或 --long 选项后面是可接受的长选项，用逗号分开，冒号的意义同短选项。
    # -n 选项后接选项解析错误时提示的脚本名字
    OPTS=help,install:,source:,tools:,build:,apt:,apt_update::,base::,default::,qt::,rabbitcommon::,freerdp::,tigervnc::,pcapplusplus::,libdatachannel::,QtService::
    ARGS=`getopt -o h -l $OPTS -n $(basename $0) -- "$@"`
    if [ $? != 0 ]; then
        echo "exec getopt fail: $?"
        exit 1
    fi
    #echo "ARGS=[$ARGS]"
    #将规范化后的命令行参数分配至位置参数（$1,$2,......)
    eval set -- "${ARGS}"
    #echo "formatted parameters=[$@]"

    while [ $1 ]
    do
        #echo "\$1: $1"
        #echo "\$2: $2"
        case $1 in
        --install)
            INSTALL_DIR=$2
            shift 2
            ;;
        --source)
            SOURCE_DIR=$2
            shift 2
            ;;
        --tools)
            TOOLS_DIR=$2
            shift 2
            ;;
        --build)
            BUILD_DIR=$2
            shift 2
            ;;
        --apt)
            APT=$2
            shift 2
            ;;
        --apt_update)
            case $2 in
                "")
                    APT_UPDATE=1;;
                *)
                    APT_UPDATE=$2;;
            esac
            shift 2
            ;;
        --base)
            case $2 in
                "")
                    BASE_LIBS=1;;
                *)
                    BASE_LIBS=$2;;
            esac
            shift 2
            ;;
        --default)
            case $2 in
                "")
                    DEFAULT_LIBS=1;;
                *)
                    DEFAULT_LIBS=$2;;
            esac
            shift 2
            ;;
        --qt)
            case $2 in
                "")
                    QT=1;;
                *)
                    QT=$2;;
            esac
            shift 2
            ;;
        --freerdp)
            case $2 in
                "")
                    FREERDP=1;;
                *)
                    FREERDP=$2;;
            esac
            shift 2
            ;;
        --tigervnc)
            case $2 in
                "")
                    TIGERVNC=1;;
                *)
                    TIGERVNC=$2;;
            esac
            shift 2
            ;;
        --pcapplusplus)
            case $2 in
                "")
                    PCAPPLUSPLUS=1;;
                *)
                    PCAPPLUSPLUS=$2;;
            esac
            shift 2
            ;;
        --rabbitcommon)
            case $2 in
                "")
                    RabbitCommon=1;;
                *)
                    RabbitCommon=$2;;
            esac
            shift 2
            ;;
        --libdatachannel)
            case $2 in
                "")
                    libdatachannel=1;;
                *)
                    libdatachannel=$2;;
            esac
            shift 2
            ;;
        --QtService)
            case $2 in
                "")
                    QtService=1;;
                *)
                    QtService=$2;;
            esac
            shift 2
            ;;
        --) # 当解析到“选项和参数“与“non-option parameters“的分隔符时终止
            shift
            break
            ;;
        -h | -help)
            usage_long
            shift
            ;;
        *)
            usage_long
            break
            ;;
        esac
    done
else
    echo "getopt is not exits"
    if [ $# -eq 0 ]; then
        usage
    else
        for i in "$@"; do
            case "$i" in
                "apt_update") APT_UPDATE=1;;
                "base") BASE_LIBS=1;;
                "default") DEFAULT_LIBS=1;;
                "qt") QT=1;;
                "freerdp") FREERDP=1;;
                "tigervnc") TIGERVNC=1;;
                "pcapplusplus") PCAPPLUSPLUS=1;;
                "rabbitcommon") RabbitCommon=1;;
                "libdatachannel") libdatachannel=1;;
                "QtService") QtService==1;;
            esac
        done
        while getopts :i:s:t: opt; do
            case $opt in
                i) INSTALL_DIR=$OPTARG ;;
                s) SOURCE_DIR=$OPTARG ;;
                t) TOOLS_DIR=$OPTARG ;;
                b) BUILD_DIR=$OPTARG ;;
                a) APT=$OPTARG ;;
            esac
        done
    fi
fi

# store repo root as variable
REPO_ROOT=$(readlink -f $(dirname $(dirname $(readlink -f $0))))
OLD_CWD=$(readlink -f .)

pushd "$REPO_ROOT"

if [ -z "$BUILD_DIR" ]; then
    BUILD_DIR=build_depend
fi
BUILD_DIR=$(readlink -f ${BUILD_DIR})
mkdir -p $BUILD_DIR
pushd "$BUILD_DIR"

if [ -z "$TOOLS_DIR" ]; then
    TOOLS_DIR=Tools
fi
if [ -z "$SOURCE_DIR" ]; then
    SOURCE_DIR=Source
fi
if [ -z "$INSTALL_DIR" ]; then
    INSTALL_DIR=Install
fi

TOOLS_DIR=$(readlink -f ${TOOLS_DIR})
mkdir -p $TOOLS_DIR
SOURCE_DIR=$(readlink -f ${SOURCE_DIR})
mkdir -p $SOURCE_DIR
INSTALL_DIR=$(readlink -f ${INSTALL_DIR})
mkdir -p $INSTALL_DIR

echo "Repo folder: $REPO_ROOT"
echo "Old folder: $OLD_CWD"
echo "Current folder: `pwd`"
echo "BUILD_DIR: $BUILD_DIR"
echo "TOOLS_DIR: $TOOLS_DIR"
echo "SOURCE_DIR: $SOURCE_DIR"
echo "INSTALL_DIR: $INSTALL_DIR"

if [ $APT_UPDATE -eq 1 ]; then
    echo "apt update ......"
    apt-get update -y
    apt-get upgrade -y
fi

if [ -n "$APT" ]; then
    apt install -y -q $APT
fi

if [ $BASE_LIBS -eq 1 ]; then
    echo "Install base libraries ......"
    apt install -y -q build-essential \
        git cmake gcc g++ debhelper fakeroot graphviz gettext
    # Needed by QtMultimedia
    apt install -y -q pipewire
    # OpenGL
    apt install -y -q libgl1-mesa-dev libglx-dev libglu1-mesa-dev libvulkan-dev mesa-common-dev
    # Virtual desktop (virtual framebuffer X server for X Version 11). Needed by CI
    apt install -y -q xvfb xpra
    # X11
    apt install -y -q xorg-dev x11-xkb-utils libxkbcommon-dev libxkbcommon-x11-dev libx11-xcb-dev \
        libx11-dev libxfixes-dev libxcb-randr0-dev libxcb-shm0-dev \
        libxcb-xinerama0-dev libxcb-composite0-dev libxcomposite-dev \
        libxinerama-dev libxcb1-dev libx11-xcb-dev libxcb-xfixes0-dev \
        libxcb-cursor-dev libxcb-xkb-dev libxcb-keysyms1-dev \
        libxcb-* libxcb-cursor0 xserver-xorg-input-mouse xserver-xorg-input-kbd \
        libxkbcommon-dev
    # Base dependency
    apt install -y -q libssl-dev libcrypt-dev libicu-dev zlib1g-dev libssh-dev libtelnet-dev
    # RabbitCommon dependency
    apt install -y -q libcmark-dev cmark
    # VNC dependency
    apt install -y -q libpixman-1-dev libjpeg-dev
    # FreeRDP dependency
    apt install -y -q libcjson-dev libfuse-dev libfuse3-dev libusb-1.0-0-dev \
        libkrb5-dev libpulse-dev libcups2-dev libpam0g-dev libutf8proc-dev
    # PcapPlusPlus dependency
    apt install -y -q libpcap-dev
    # FFmpeg
    apt install -y -q libavcodec-dev libavformat-dev libresample1-dev libswscale-dev
    # AppImage
    apt install -y -q fuse
    # Other
    apt install -y -q libvncserver-dev
fi

if [ $DEFAULT_LIBS -eq 1 ]; then
    echo "Install default dependency libraries ......"
    # Qt6
    apt-get install -y -q qmake6 qt6-tools-dev qt6-tools-dev-tools \
        qt6-base-dev qt6-base-dev-tools qt6-qpa-plugins \
        libqt6svg6-dev qt6-l10n-tools qt6-translations-l10n \
        qt6-scxml-dev qt6-multimedia-dev libqt6serialport6-dev
    apt-get install -y -q freerdp2-dev
fi

if [ $QT -eq 1 ]; then
    echo "Install qt ......"
    pushd "$TOOLS_DIR"
    # See: https://ddalcino.github.io/aqt-list-server/
    apt install -y -q python3-pip python3-pip-whl python3-pipdeptree cpio
    #pip install -U pip
    pip install aqtinstall
    export QTVERSION=6.8.2
    if [ "`uname -m`" == "x86_64" ]; then
        aqt install-qt linux desktop ${QTVERSION} linux_gcc_64 -m qt5compat qtgraphs qtimageformats qtmultimedia qtscxml qtserialport
        mv ${QTVERSION}/gcc_64 qt
    elif [ "`uname -m`" == "aarch64" ]; then
        aqt install-qt linux_arm64 desktop ${QTVERSION} linux_gcc_arm64 -m qt5compat qtgraphs qtimageformats qtmultimedia qtscxml qtserialport
        mv ${QTVERSION}/gcc_arm64 qt
    fi
    popd
fi

if [ $RabbitCommon -eq 1 ]; then
    pushd "$SOURCE_DIR"
    if [ ! -d RabbitCommon ]; then
        git clone https://github.com/KangLin/RabbitCommon.git
    fi
    popd
fi

if [ $FREERDP -eq 1 ]; then
    echo "Install FreeRDP ......"
    pushd "$SOURCE_DIR"
    if [ ! -d ${INSTALL_DIR}/lib/cmake/FreeRDP3 ]; then
        git clone -b 3.11.1 https://github.com/FreeRDP/FreeRDP.git
        cd FreeRDP
        git submodule update --init --recursive
        cmake -E make_directory build
        cd build
        cmake .. \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" \
          -DWITH_SERVER=ON \
          -DWITH_CLIENT_SDL=OFF \
          -DWITH_KRB5=OFF \
          -DWITH_MANPAGES=OFF
        cmake --build . --config Release --parallel $(nproc)
        cmake --build . --config Release --target install
    fi
    popd
fi

if [ $TIGERVNC -eq 1 ]; then
    echo "Install tigervnc ......"
    pushd "$SOURCE_DIR"
    if [ ! -d ${INSTALL_DIR}/lib/cmake/tigervnc ]; then
      git clone --depth=1 https://github.com/KangLin/tigervnc.git
      cd tigervnc
      cmake -E make_directory build
      cd build
      cmake .. -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
          -DBUILD_TESTS=OFF -DBUILD_VIEWER=OFF -DENABLE_NLS=OFF
      cmake --build . --config Release --parallel $(nproc)
      cmake --build . --config Release --target install
    fi
    popd
fi

if [ $PCAPPLUSPLUS -eq 1 ]; then
    echo "Install PcapPlusPlus ......"
    pushd "$SOURCE_DIR"
    if [ ! -d ${INSTALL_DIR}/lib/cmake/pcapplusplus ]; then
        git clone -b v24.09 --depth=1 https://github.com/seladb/PcapPlusPlus.git
        cd PcapPlusPlus
        cmake -E make_directory build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
            -DPCAPPP_BUILD_EXAMPLES=OFF \
            -DPCAPPP_BUILD_TESTS=OFF \
            -DPCAPPP_BUILD_TUTORIALS=OFF
        cmake --build . --config Release --parallel $(nproc)
        cmake --build . --config Release --target install
    fi
    popd
fi

if [ $libdatachannel -eq 1 ]; then
    echo "Install libdatachannel ......"
    pushd "$SOURCE_DIR"
    if [ ! -d ${INSTALL_DIR}/lib/cmake/LibDataChannel ]; then
      git clone -b v0.17.8 --depth=1 https://github.com/paullouisageneau/libdatachannel.git
      cd libdatachannel
      git submodule update --init --recursive
      cmake -E make_directory build
      cd build
      cmake .. -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
      cmake --build . --config Release --parallel $(nproc)
      cmake --build . --config Release --target install
    fi
    popd
fi

if [ $QtService -eq 1 ]; then
    echo "Install QtService ......"
    pushd "$SOURCE_DIR"
    if [ ! -d ${INSTALL_DIR}/lib/cmake/QtService ]; then
      git clone --depth=1 https://github.com/KangLin/qt-solutions.git
      cd qt-solutions/qtservice
      git submodule update --init --recursive
      cmake -E make_directory build
      cd build
      cmake .. -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
      cmake --build . --config Release --parallel $(nproc)
      cmake --build . --config Release --target install
    fi
    popd
fi
