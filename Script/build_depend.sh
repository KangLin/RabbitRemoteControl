#!/bin/bash
# Author: Kang Lin <kl222@126.com>

#See: https://blog.csdn.net/alwaysbefine/article/details/114187380
#set -x
set -e
#set -v

if [ -z "$BUILD_VERBOSE" ]; then
    BUILD_VERBOSE=OFF
fi
PACKAGE_TOOL=apt
PACKAGE=
SYSTEM_UPDATE=0
BASE_LIBS=0
DEFAULT_LIBS=0
QT=0
if [ -z "$QT_VERSION" ]; then
    QT_VERSION=6.9.2
fi
FREERDP=0
TIGERVNC=0
PCAPPLUSPLUS=0
RabbitCommon=0
libdatachannel=0
QtService=0
QTERMWIDGET=0
LIBSSH=0
MACOS=0
QTKEYCHAIN=0

usage_long() {
    echo "$0 [-h|--help] [--install=<install directory>] [--source=<source directory>] [--tools=<tools directory>] [--build=<build directory>] [-v|--verbose[=0|1]] [--package=<'package1 package2 ...'>] [--package-tool=<apt|dnf>] [--system_update=[0|1]] [--base[=[0|1]]] [--default[=[0|1]]] [--qt[=[0|1|version]]] [--macos=[0|1]]] [--rabbitcommon[=[0|1]]] [--freerdp[=[0|1]]] [--tigervnc[=[0|1]]] [--libssh[=[0|1]]] [--pcapplusplus[=[0|1]]] [--libdatachannel=[0|1]] [--QtService[=[0|1]]] [--qtermwidget[=[0|1]]] [--qtkeychain[=[0|1]]"
    echo "  -h|--help: show help"
    echo "  -v|--verbose: Show verbose"
    echo "Directory:"
    echo "  --install: Set install directory"
    echo "  --source: Set source directory"
    echo "  --tools: Set tools directory"
    echo "  --build: set build directory"
    echo "Depend:"
    echo "  --base: Install the base libraries"
    echo "  --default: Install the default dependency libraries that comes with the system"
    echo "  --system_update: Update system"
    echo "  --package-tool: Package install tool, apk or dnf"
    echo "  --package: Install package"
    echo "  --qt: Install QT"
    echo "  --macos: Install macos tools and dependency libraries"
    echo "  --rabbitcommon: Install RabbitCommon"
    echo "  --freerdp: Install FreeRDP"
    echo "  --tigervnc: Install TigerVNC"
    echo "  --libssh: Install libssh"
    echo "  --pcapplusplus: Install PcapPlusPlus"
    echo "  --libdatachannel: Install libdatachannel"
    echo "  --QtService: Install QtService"
    echo "  --qtermwidget: Install qtermwidget"
    echo "  --qtkeychain: Install qtkeychain"
    exit
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
    OPTS=help,install:,source:,tools:,build:,verbose::,package:,package-tool:,system_update::,base::,default::,macos::,qt::,rabbitcommon::,freerdp::,tigervnc::,libssh::,pcapplusplus::,libdatachannel::,QtService::,qtermwidget::,qtkeychain::
    ARGS=`getopt -o h,v:: -l $OPTS -n $(basename $0) -- "$@"`
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
            BUILD_DEPEND_DIR=$2
            shift 2
            ;;
        --package)
            PACKAGE=$2
            shift 2
            ;;
        --package-tool)
            PACKAGE_TOOL=$2
            shift 2
            ;;
        -v | --verbose)
            case $2 in
                "")
                    BUILD_VERBOSE=ON;;
                *)
                    BUILD_VERBOSE=$2;;
            esac
            shift 2
            ;;
        --system_update)
            case $2 in
                "")
                    SYSTEM_UPDATE=1;;
                *)
                    SYSTEM_UPDATE=$2;;
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
                1 | 0)
                    QT=$2;;
                *)
                    QT_VERSION=$2
                    QT=1
                    ;;
            esac
            shift 2
            ;;
        --macos)
            case $2 in
                "")
                    MACOS=1;;
                *)
                    MACOS=$2;;
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
        --libssh)
            case $2 in
                "")
                    LIBSSH=1;;
                *)
                    LIBSSH=$2;;
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
        --qtermwidget)
            case $2 in
                "")
                    QTERMWIDGET=1;;
                *)
                    QTERMWIDGET=$2;;
            esac
            shift 2
            ;;
        --qtkeychain)
            case $2 in
                "")
                    QTKEYCHAIN=1;;
                *)
                    QTKEYCHAIN=$2;;
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
fi

# store repo root as variable
REPO_ROOT=$(readlink -f $(dirname $(dirname $(readlink -f $0))))
OLD_CWD=$(readlink -f .)

pushd "$REPO_ROOT"

if [ -z "$BUILD_DEPEND_DIR" ]; then
    BUILD_DEPEND_DIR=build_depend
fi
BUILD_DEPEND_DIR=$(readlink -f ${BUILD_DEPEND_DIR})
mkdir -p $BUILD_DEPEND_DIR
pushd "$BUILD_DEPEND_DIR"

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
echo "BUILD_DEPEND_DIR: $BUILD_DEPEND_DIR"
echo "TOOLS_DIR: $TOOLS_DIR"
echo "SOURCE_DIR: $SOURCE_DIR"
echo "INSTALL_DIR: $INSTALL_DIR"

if [ $SYSTEM_UPDATE -eq 1 ]; then
    echo "System update ......"
    $PACKAGE_TOOL update -y
fi

if [ -n "$PACKAGE" ]; then
    ${PACKAGE_TOOL} install -y -q $PACKAGE
fi

if [ $BASE_LIBS -eq 1 ]; then
    echo "Install base libraries ......"
    if [ "$PACKAGE_TOOL" = "apt" ]; then
        apt install -y -q build-essential packaging-dev equivs debhelper \
            fakeroot graphviz gettext wget curl
        # OpenGL
        apt install -y -q libgl1-mesa-dev libglx-dev libglu1-mesa-dev libvulkan-dev mesa-common-dev
        # Virtual desktop (virtual framebuffer X server for X Version 11). Needed by CI
        if [ -z "$RabbitRemoteControl_VERSION" ]; then
            apt install -y -q xvfb xpra
        fi
        # X11 and xcb
        apt install -y -q xorg-dev x11-xkb-utils libxkbcommon-dev libxkbcommon-x11-dev libx11-xcb-dev \
            libx11-dev libxfixes-dev libxcb-randr0-dev libxcb-shm0-dev \
            libxcb-xinerama0-dev libxcb-composite0-dev libxcomposite-dev \
            libxinerama-dev libxcb1-dev libx11-xcb-dev libxcb-xfixes0-dev \
            libxcb-cursor-dev libxcb-xkb-dev libxcb-keysyms1-dev \
            libxcb-* libxcb-cursor0 xserver-xorg-input-mouse xserver-xorg-input-kbd \
            libxkbcommon-dev
        # Base dependency
        apt install -y -q liblzo2-dev libssl-dev libcrypt-dev libicu-dev zlib1g-dev libtelnet-dev
        # RabbitCommon dependency
        apt install -y -q libcmark-dev cmark
        # VNC dependency
        apt install -y -q libpixman-1-dev libjpeg-dev
        # FreeRDP dependency
        apt install -y -q libcjson-dev libusb-1.0-0-dev \
            libkrb5-dev libpulse-dev libcups2-dev libpam0g-dev libutf8proc-dev
        # PcapPlusPlus dependency
        apt install -y -q libpcap-dev
        # FFmpeg needed by QtMultimedia and freerdp
        apt install -y -q libavcodec-dev libavformat-dev libresample1-dev libswscale-dev
        apt install -y -q libx264-dev libx265-dev
        # Needed by QtMultimedia
        apt install -y -q pipewire
        # Needed by QtMultimedia
        apt install -y -q libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer*-dev
        # Needed by AppImage and FreeRDP
        apt install -y -q libfuse-dev libfuse3-dev
        # Other
        apt install -y -q libvncserver-dev
        # File transfer
        apt install -y -q libcurl4-openssl-dev
        # Needed by qtkeychain
        apt install -y -q libsecret-1-dev
    fi

    if [ "$PACKAGE_TOOL" = "dnf" ]; then
        dnf install -y make git rpm-build rpmdevtools gcc-c++ util-linux \
           automake autoconf libtool gettext gettext-autopoint \
           cmake desktop-file-utils appstream appstream-util curl wget
        # X11 and xcb
        dnf install -y xorg-x11-server-source \
            libXext-devel libX11-devel libXi-devel libXfixes-devel \
            libXdamage-devel libXrandr-devel libXt-devel libXdmcp-devel \
            libXinerama-devel mesa-libGL-devel libxshmfence-devel \
            libdrm-devel mesa-libgbm-devel \
            xorg-x11-util-macros xorg-x11-xtrans-devel libXtst-devel \
            xorg-x11-font-utils \
            libxkbfile-devel libXfont2-devel xcb-util-keysyms-devel

        # TigerVNC
        dnf install -y zlib-devel openssl-devel libpng-devel \
            libjpeg-turbo-devel ffmpeg-free-devel \
            pixman-devel gnutls-devel nettle-devel gmp-devel \
            libpciaccess-devel freetype-devel pam-devel
    fi
fi

if [ $DEFAULT_LIBS -eq 1 ]; then
    echo "Install default dependency libraries ......"
    if [ "$PACKAGE_TOOL" = "apt" ]; then
        # Qt6
        apt-get install -y -q qmake6 qt6-tools-dev qt6-tools-dev-tools \
            qt6-base-dev qt6-base-dev-tools qt6-qpa-plugins \
            libqt6svg6-dev qt6-l10n-tools qt6-translations-l10n \
            qt6-scxml-dev qt6-multimedia-dev libqt6serialport6-dev qt6-websockets-dev \
            qt6-webengine-dev qt6-webengine-dev-tools qt6-positioning-dev qt6-webchannel-dev
        apt-get install -y -q freerdp2-dev qtkeychain-qt6-dev
    fi
    
    if [ "$PACKAGE_TOOL" = "dnf" ]; then
        dnf install -y qt6-qttools-devel qt6-qtbase-devel qt6-qtmultimedia-devel \
            qt6-qt5compat-devel qt6-qtmultimedia-devel qt6-qtscxml-devel \
            qt6-qtserialport-devel qt6-qtsvg-devel qt6-qtwebsockets-devel \
            qt6-qtwebengine-devel qt6-qtwebengine-devtools qt6-qtpositioning-devel qt6-qtwebchannel-devel
    fi
fi

if [ $QT -eq 1 ]; then
    echo "Install qt ${QT_VERSION} ......"
    pushd "$TOOLS_DIR"
    if [ ! -d qt_`uname -m` ]; then
        # See: https://ddalcino.github.io/aqt-list-server/
        #      https://www.cnblogs.com/clark1990/p/17942952
        #if [ "$PACKAGE_TOOL" = "apt" ]; then
        #    apt install -y -q python3-pip python3-pip-whl python3-pipdeptree cpio
        #    pip install --upgrade typing-extensions
        #fi

        #pip install -U pip
        pip install --ignore-installed aqtinstall

        echo "PATH: $PATH"
        if [ "`uname -m`" == "x86_64" ]; then
            aqt install-qt linux desktop ${QT_VERSION} linux_gcc_64 -m qtscxml qtmultimedia qtimageformats qtserialport qt5compat qtwebsockets qtpositioning qtwebchannel qtwebengine
            mv ${QT_VERSION}/gcc_64 qt_`uname -m`
         elif [ "`uname -m`" == "aarch64" ]; then
            aqt install-qt linux_arm64 desktop ${QT_VERSION} linux_gcc_arm64 -m qtscxml qtmultimedia qtimageformats qtserialport qt5compat qtwebsockets qtpositioning qtwebchannel qtwebengine
            mv ${QT_VERSION}/gcc_arm64 qt_`uname -m`
        fi
    fi
    popd
fi

if [ $MACOS -eq 1 ]; then
    echo "Install macos tools and dependency libraries ......"
    brew install qt doxygen freerdp libvncserver libssh zstd libpcap pcapplusplus
fi

if [ $RabbitCommon -eq 1 ]; then
    echo "Install RabbitCommon ......"
    pushd "$SOURCE_DIR"
    if [ ! -d RabbitCommon ]; then
        git clone https://github.com/KangLin/RabbitCommon.git
    else
        cd RabbitCommon
        git pull
        cd ..
    fi
    popd
fi

if [ $FREERDP -eq 1 ]; then
    echo "Install FreeRDP ......"
    pushd "$SOURCE_DIR"
    if [ ! -d ${INSTALL_DIR}/lib/cmake/FreeRDP3 ]; then
        git clone -b 3.15.0 https://github.com/FreeRDP/FreeRDP.git
        cd FreeRDP
        git submodule update --init --recursive
        cmake -E make_directory build
        cd build
        cmake .. \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE} \
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
          -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE} \
          -DBUILD_TESTS=OFF -DBUILD_VIEWER=OFF -DENABLE_NLS=OFF
      cmake --build . --config Release --parallel $(nproc)
      cmake --build . --config Release --target install
    fi
    popd
fi

if [ $LIBSSH -eq 1 ]; then
    echo "Install libssh ......"
    pushd "$SOURCE_DIR"
    if [ ! -d ${INSTALL_DIR}/lib/cmake/libssh ]; then
        git clone -b libssh-0.11.3 --depth=1 https://git.libssh.org/projects/libssh.git
        cd libssh
        cmake -E make_directory build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE} \
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
            -DWITH_EXAMPLES=OFF
        cmake --build . --config Release --parallel $(nproc)
        cmake --build . --config Release --target install
    fi
    popd
fi

if [ $PCAPPLUSPLUS -eq 1 ]; then
    echo "Install PcapPlusPlus ......"
    pushd "$SOURCE_DIR"
    if [ ! -d ${INSTALL_DIR}/lib/cmake/pcapplusplus ]; then
        git clone -b v25.05 --depth=1 https://github.com/seladb/PcapPlusPlus.git
        cd PcapPlusPlus
        cmake -E make_directory build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE} \
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
          -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE} \
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
          -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
          -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE}
      cmake --build . --config Release --parallel $(nproc)
      cmake --build . --config Release --target install
    fi
    popd
fi

if [ $QTERMWIDGET -eq 1 ]; then
    echo "Install qtermwidget ......"
    pushd "$SOURCE_DIR"
    #CMAKE=`/usr/bin/qtpaths6 --query QT_HOST_BINS`/qt-cmake
    #echo "CMAKE: $CMAKE"
    if [ ! -d ${INSTALL_DIR}/share/cmake/lxqt2-build-tools ]; then
        echo "Install lxqt-build-tools ......"
        if [ ! -d lxqt-build-tools ]; then
            git clone --depth=1 -b 2.2.1 https://github.com/lxqt/lxqt-build-tools.git
        fi
        cd lxqt-build-tools
        sed -i "s/LXQT_MIN_LINGUIST_VERSION \"6.6\"/LXQT_MIN_LINGUIST_VERSION \"6.0\"/g" CMakeLists.txt
        cmake -E make_directory build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
            -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE}
        cmake --build . --config Release --parallel $(nproc)
        cmake --build . --config Release --target install
    fi
    if [ ! -d ${INSTALL_DIR}/lib/cmake/qtermwidget6 ]; then
        if [ ! -d qtermwidget ]; then
            git clone --depth=1 https://github.com/KangLin/qtermwidget.git
        fi
        cd qtermwidget
        cmake -E make_directory build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
            -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE} \
            -Dlxqt2-build-tools_DIR=${INSTALL_DIR}/share/cmake/lxqt2-build-tools
        cmake --build . --config Release --parallel $(nproc)
        cmake --build . --config Release --target install
    fi
    popd
fi

if [ $QTKEYCHAIN -eq 1 ]; then
    echo "Install QtKeyChain ......"
    pushd "$SOURCE_DIR"
    if [ ! -d ${INSTALL_DIR}/lib/cmake/Qt6Keychain ]; then
        git clone -b 0.15.0 https://github.com/frankosterfeld/qtkeychain.git
        cd qtkeychain
        cmake -E make_directory build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
            -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE} \
            -DBUILD_WITH_QT6:BOOL=ON
        cmake --build . --config Release --parallel $(nproc)
        cmake --build . --config Release --target install
    fi
    popd
fi
