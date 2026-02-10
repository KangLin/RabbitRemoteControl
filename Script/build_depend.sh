#!/bin/bash
# Author: Kang Lin <kl222@126.com>

#See: https://blog.csdn.net/alwaysbefine/article/details/114187380

#set -x
set -e
#set -v

if [ -z "$BUILD_VERBOSE" ]; then
    BUILD_VERBOSE=OFF
fi

source $(dirname $(readlink -f $0))/common.sh

detect_os_info

install_gnu_getopt
if [ "$OS" = "macOS" ]; then
    MACOS=1
    setup_macos
else
    MACOS=0
fi

if [ -z "$PACKAGE_TOOL" ]; then
    PACKAGE_TOOL=apt
fi

PACKAGE=
SYSTEM_UPDATE=0
BASE_LIBS=0
DEFAULT_LIBS=0
QT=0
if [ -z "$QT_VERSION" ]; then
    QT_VERSION=6.10.2
fi
FREERDP=0
TIGERVNC=0
PCAPPLUSPLUS=0
RabbitCommon=0
libdatachannel=0
QtService=0
QTERMWIDGET=0
LIBSSH=0
QTKEYCHAIN=0
QFtpServer=0

# Display detailed usage information
usage_long() {
    cat << EOF
$(basename $0) - Dependency build script

Usage: $0 [OPTION]...

Options:
  -h, --help                        Show this help message
  -v, --verbose[=LEVEL]             Set verbose mode (LEVEL: ON, OFF, default: $BUILD_VERBOSE)

Directory options:
  --install=DIR                     Set installation directory
  --source=DIR                      Set source code directory  
  --tools=DIR                       Set tools directory
  --build=DIR                       Set build directory

Package management options:
  --package="PKG1 PKG2 ..."         Install specified system packages
  --package-tool=TOOL               Set package manager tool (apt, dnf, brew, pacman, zypper, apk)
  --system_update[=1|0]             Update system package manager
  --system-update[=1|0]             Update system package manager

Dependency options:
  --base[=1|0]                      Install basic development libraries
  --default[=1|0]                   Install system default dependency libraries
  --qt[=1|0|VERSION]                Install Qt (can specify version)
  --macos[=1|0]                     Install macOS specific tools and dependencies

Component options:
  --rabbitcommon[=1|0]              Install RabbitCommon
  --freerdp[=1|0]                   Install FreeRDP
  --tigervnc[=1|0]                  Install TigerVNC
  --libssh[=1|0]                    Install libssh
  --pcapplusplus[=1|0]              Install PcapPlusPlus
  --libdatachannel[=1|0]            Install libdatachannel
  --QtService[=1|0]                 Install QtService
  --qtermwidget[=1|0]               Install qtermwidget
  --qtkeychain[=1|0]                Install qtkeychain
  --qftpserver[=1|0]                Install QFtpServer

Examples:
  $0 --base=1 --qt=1 --install=/opt/local
  $0 --verbose --package="git cmake" --freerdp=1 --tigervnc=1
  $0 --system_update --base --default --qt=6.5.0

Environment variables:
  BUILD_VERBOSE     Set verbose mode (ON/OFF, default: $BUILD_VERBOSE)
  QT_VERSION        Set Qt version (default: $QT_VERSION)
EOF
    exit 0
}

# Parse arguments using getopt (more powerful)
# [如何使用getopt和getopts命令解析命令行选项和参数](https://zhuanlan.zhihu.com/p/673908518)
# [【Linux】Shell命令 getopts/getopt用法详解](https://blog.csdn.net/arpospf/article/details/103381621)
# 
# 注意：在 macOS 上，本地 getopt 不支持长格式参数，所以需要先在系统上安装 GNU getopt，并设置环境变量 PATH
#  brew install gnu-getopt
#  export PATH="/usr/local/opt/gnu-getopt/bin:$PATH"
parse_with_getopt() {
    local OPTS ARGS
    
    echo "Using getopt to parse command line arguments..."
    
    # Define supported options
    # Format: long_option_name: (colon indicates required argument)
    # :: means optional argument, no colon means no argument
    # -o 或 --options 选项后面是可接受的短选项，如 ab:c:: ，表示可接受的短选项为 -a -b -c ，
    # 其中 -a 选项不接参数，-b 选项后必须接参数，-c 选项的参数为可选的
    # 后面没有冒号表示没有参数。后跟有一个冒号表示有参数。跟两个冒号表示有可选参数。
    # -l 或 --long 选项后面是可接受的长选项，用逗号分开，冒号的意义同短选项。
    # -n 选项后接选项解析错误时提示的脚本名字
    OPTS=help,install:,source:,tools:,build:,verbose::,package:,package-tool:,system_update::,system-update::,base::,default::,macos::,qt::,rabbitcommon::,freerdp::,tigervnc::,libssh::,pcapplusplus::,libdatachannel::,QtService::,qtermwidget::,qtkeychain::,qftpserver::
    
    # Parse arguments using getopt
    # -o: short options
    # -l: long options  
    # -n: script name for error messages
    ARGS=$(getopt -o h,v:: -l "$OPTS" -n "$(basename "$0")" -- "$@")
    if [ $? != 0 ]; then
        echo "Error: Command line argument parsing failed" >&2
        usage_long
    fi
    
    # Set positional parameters to parsed arguments
    #将规范化后的命令行参数分配至位置参数（$1,$2,......)
    eval set -- "$ARGS"
    #echo "formatted parameters=[$@]"

    # Process options
    while true; do
        case "$1" in
        --install)
            validate_directory "$2" "Installation"
            INSTALL_DIR="$2"
            shift 2
            ;;
        --source)
            validate_directory "$2" "Source code"
            SOURCE_DIR="$2"
            shift 2
            ;;
        --tools)
            validate_directory "$2" "Tools"
            TOOLS_DIR="$2"
            shift 2
            ;;
        --build)
            validate_directory "$2" "Build"
            BUILD_DIR="$2"
            shift 2
            ;;
        --package)
            PACKAGE="$2"
            shift 2
            ;;
        --package-tool)
            PACKAGE_TOOL="$2"
            shift 2
            ;;
        -v|--verbose)
            case "$2" in
                "")
                    BUILD_VERBOSE="ON"
                    ;;
                *)
                    BUILD_VERBOSE="$2"
                    ;;
            esac
            shift 2
            ;;
        --system_update|--system-update)
            case "$2" in
                "")
                    SYSTEM_UPDATE=1
                    ;;
                *)
                    SYSTEM_UPDATE="$2"
                    ;;
            esac
            shift 2
            ;;
        --base)
            case "$2" in
                "")
                    BASE_LIBS=1
                    ;;
                *)
                    BASE_LIBS="$2"
                    ;;
            esac
            shift 2
            ;;
        --default)
            case "$2" in
                "")
                    DEFAULT_LIBS=1
                    ;;
                *)
                    DEFAULT_LIBS="$2"
                    ;;
            esac
            shift 2
            ;;
        --qt)
            case "$2" in
                "")
                    QT=1
                    ;;
                1|0)
                    QT="$2"
                    ;;
                *)
                    QT_VERSION="$2"
                    QT=1
                    ;;
            esac
            shift 2
            ;;
        --macos)
            case "$2" in
                "")
                    MACOS=1
                    ;;
                *)
                    MACOS="$2"
                    ;;
            esac
            shift 2
            ;;
        --freerdp)
            case "$2" in
                "")
                    FREERDP=1
                    ;;
                *)
                    FREERDP="$2"
                    ;;
            esac
            shift 2
            ;;
        --tigervnc)
            case "$2" in
                "")
                    TIGERVNC=1
                    ;;
                *)
                    TIGERVNC="$2"
                    ;;
            esac
            shift 2
            ;;
        --libssh)
            case "$2" in
                "")
                    LIBSSH=1
                    ;;
                *)
                    LIBSSH="$2"
                    ;;
            esac
            shift 2
            ;;
        --pcapplusplus)
            case "$2" in
                "")
                    PCAPPLUSPLUS=1
                    ;;
                *)
                    PCAPPLUSPLUS="$2"
                    ;;
            esac
            shift 2
            ;;
        --rabbitcommon)
            case "$2" in
                "")
                    RabbitCommon=1
                    ;;
                *)
                    RabbitCommon="$2"
                    ;;
            esac
            shift 2
            ;;
        --libdatachannel)
            case "$2" in
                "")
                    libdatachannel=1
                    ;;
                *)
                    libdatachannel="$2"
                    ;;
            esac
            shift 2
            ;;
        --QtService)
            case "$2" in
                "")
                    QtService=1
                    ;;
                *)
                    QtService="$2"
                    ;;
            esac
            shift 2
            ;;
        --qtermwidget)
            case "$2" in
                "")
                    QTERMWIDGET=1
                    ;;
                *)
                    QTERMWIDGET="$2"
                    ;;
            esac
            shift 2
            ;;
        --qtkeychain)
            case "$2" in
                "")
                    QTKEYCHAIN=1
                    ;;
                *)
                    QTKEYCHAIN="$2"
                    ;;
            esac
            shift 2
            ;;
        --qftpserver)
            case "$2" in
                "")
                    QFtpServer=1
                    ;;
                *)
                    QFtpServer="$2"
                    ;;
            esac
            shift 2
            ;;
        --) # End of options
            shift
            break
            ;;
        -h|--help)
            usage_long
            ;;
        *)
            echo "Error: Unknown option '$1'" >&2
            usage_long
            ;;
        esac
    done
    
    # Handle remaining non-option arguments (if any)
    if [ $# -gt 0 ]; then
        echo "Warning: Ignoring unknown arguments: $*" >&2
    fi
}

# Parse command line arguments
parse_command_line() {
    # Use getopt if available, otherwise fall back to getopts
    if command -v getopt >/dev/null 2>&1; then
        parse_with_getopt "$@"
    else
        echo "Install GNU getopt"
    fi
}

# Display current configuration
show_configuration() {
    if [ "$BUILD_VERBOSE" = "ON" ]; then
        echo "=== Current Configuration ==="
        echo "Directory Configuration:"
        echo "  Install Directory: ${INSTALL_DIR:-Not set (using default)}"
        echo "  Source Directory: ${SOURCE_DIR:-Not set (using default)}"
        echo "  Tools Directory: ${TOOLS_DIR:-Not set (using default)}"
        echo "  Build Directory: ${BUILD_DIR:-Not set (using default)}"
        echo "  Build Depend Directory: ${BUILD_DEPEND_DIR:-Not set (using default)}"
        echo ""
        echo "Package Management:"
        echo "  System Update: $SYSTEM_UPDATE"
        echo "  Package Installation: ${PACKAGE:-None}"
        echo "  Package Tool: ${PACKAGE_TOOL:-Auto-detected}"
        echo ""
        echo "Dependency Installation:"
        echo "  Base Libraries: $BASE_LIBS"
        echo "  Default Libraries: $DEFAULT_LIBS"
        echo "  Qt: $QT (Version: $QT_VERSION)"
        echo "  macOS Tools: $MACOS"
        echo ""
        echo "Component Installation:"
        echo "  RabbitCommon: $RabbitCommon"
        echo "  FreeRDP: $FREERDP"
        echo "  TigerVNC: $TIGERVNC"
        echo "  libssh: $LIBSSH"
        echo "  PcapPlusPlus: $PCAPPLUSPLUS"
        echo "  libdatachannel: $libdatachannel"
        echo "  QtService: $QtService"
        echo "  qtermwidget: $QTERMWIDGET"
        echo "  qtkeychain: $QTKEYCHAIN"
        echo ""
        echo "Other Settings:"
        echo "  Verbose Mode: $BUILD_VERBOSE"
        echo "========================="
        echo ""
    fi

    echo "Repo folder: $REPO_ROOT"
    echo "Old folder: $OLD_CWD"
    echo "Current folder: `pwd`"
}

# Validate parsed parameters
validate_parameters() {
    # Check if any action was requested
    local actions=0
    for action in $SYSTEM_UPDATE $BASE_LIBS $DEFAULT_LIBS $QT $FREERDP $TIGERVNC \
                  $LIBSSH $PCAPPLUSPLUS $RabbitCommon $libdatachannel \
                  $QtService $QTERMWIDGET $QTKEYCHAIN; do
        if [ "$action" = "1" ]; then
            actions=1
            break
        fi
    done
    
    # If no actions and no packages specified, show error
    if [ $actions -eq 0 ] && [ -z "$PACKAGE" ]; then
        echo "Warning: No operation specified" >&2
        #echo "Use '$0 --help' to see available options" >&2
    fi
    
    # Validate boolean parameters
    for var in SYSTEM_UPDATE BASE_LIBS DEFAULT_LIBS QT FREERDP TIGERVNC \
               LIBSSH PCAPPLUSPLUS RabbitCommon libdatachannel \
               QtService QTERMWIDGET QTKEYCHAIN; do
        local value="${!var}"
        if [ "$value" != "0" ] && [ "$value" != "1" ]; then
            echo "Error: Parameter $var must be 0 or 1" >&2
            exit 1
        fi
    done
    
    # Validate verbose mode
    if [ "$BUILD_VERBOSE" != "ON" ] && [ "$BUILD_VERBOSE" != "OFF" ]; then
        echo "Error: BUILD_VERBOSE must be ON or OFF" >&2
        exit 1
    fi
}

# Parse command line arguments (will override environment variables)
parse_command_line "$@"

# store repo root as variable
REPO_ROOT=$(safe_readlink $(dirname $(dirname $(safe_readlink $0))))
OLD_CWD=$(safe_readlink .)

pushd "$REPO_ROOT"

if [ -z "$BUILD_DEPEND_DIR" ]; then
    if [ -z "$BUILD_DIR" ]; then
        BUILD_DEPEND_DIR=build_depend
    else
        BUILD_DEPEND_DIR=$BUILD_DIR/build_depend
    fi
fi
BUILD_DEPEND_DIR=$(safe_readlink ${BUILD_DEPEND_DIR})
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

TOOLS_DIR=$(safe_readlink ${TOOLS_DIR})
mkdir -p $TOOLS_DIR
SOURCE_DIR=$(safe_readlink ${SOURCE_DIR})
mkdir -p $SOURCE_DIR
INSTALL_DIR=$(safe_readlink ${INSTALL_DIR})
mkdir -p $INSTALL_DIR

# Validate parameters
validate_parameters

# Display configuration
show_configuration

if [ $SYSTEM_UPDATE -eq 1 ]; then
    echo "System update ......"
    case "$PACKAGE_TOOL" in
        brew)
            brew update -q
            ;;
        apt)
            if [ "$BUILD_VERBOSE" = "ON" ]; then
                apt update -y
            else
                apt update -y -qq
            fi
            ;;
        *)
            "$PACKAGE_TOOL" update -y
            ;;
    esac
fi

if [ -n "$PACKAGE" ]; then
    echo "Install package: $PACKAGE"
    package_install $PACKAGE
fi

if [ $BASE_LIBS -eq 1 ]; then
    echo "Install base libraries ......"
    if [ "$PACKAGE_TOOL" = "apt" ]; then
        # Build tools
        package_install build-essential devscripts equivs debhelper \
            fakeroot graphviz gettext wget curl git cmake ninja-build doxygen \
            dh-make lintian quilt git-buildpackage dctrl-tools
            #pbuilder

        # OpenGL
        #package_install libgl1-mesa-dev libglx-dev libglu1-mesa-dev libvulkan-dev mesa-common-dev
        # Virtual desktop (virtual framebuffer X server for X Version 11). Needed by CI
        if [ -z "$RabbitRemoteControl_VERSION" ]; then
            package_install xvfb #xpra
        fi
        # X11 and xcb
        package_install xorg-dev x11-xkb-utils libxkbcommon-dev libxkbcommon-x11-dev libx11-xcb-dev \
            libx11-dev libxfixes-dev libxcb-randr0-dev libxcb-shm0-dev \
            libxcb-xinerama0-dev libxcb-composite0-dev libxcomposite-dev \
            libxinerama-dev libxcb1-dev libx11-xcb-dev libxcb-xfixes0-dev \
            libxcb-cursor-dev libxcb-xkb-dev libxcb-keysyms1-dev \
            libxcb-* libxcb-cursor0 xserver-xorg-input-mouse xserver-xorg-input-kbd \
            libxkbcommon-dev
        # Base dependency
        package_install liblzo2-dev libssl-dev libcrypt-dev libicu-dev zlib1g-dev libtelnet-dev
        # RabbitCommon dependency
        package_install libcmark-dev cmark
        # VNC dependency
        package_install libpixman-1-dev libjpeg-dev
        # FreeRDP dependency
        package_install libcjson-dev libusb-1.0-0-dev libpcsclite-dev \
            libkrb5-dev libpulse-dev libcups2-dev libpam0g-dev libutf8proc-dev
        # PcapPlusPlus dependency
        package_install libpcap-dev
        # FFmpeg needed by QtMultimedia and freerdp
        package_install libavcodec-dev libavformat-dev libresample1-dev libswscale-dev
        package_install libx264-dev libx265-dev
        # Needed by QtMultimedia
        #package_install pipewire
        # Needed by QtMultimedia
        #package_install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer*-dev
        # Needed by AppImage and FreeRDP
        package_install libfuse-dev libfuse3-dev fuse
        # Other
        # libmysqlclient
        package_install libmariadb-dev libmariadb-dev-compat
        # Needed by qtkeychain
        package_install libsecret-1-dev
    fi

    if [ "$PACKAGE_TOOL" = "dnf" ]; then
        package_install make git rpm-build rpmdevtools gcc-c++ util-linux \
           automake autoconf libtool gettext gettext-autopoint \
           cmake desktop-file-utils curl wget dnf-plugins-core
           #appstream appstream-util
        # X11 and xcb
        package_install xorg-x11-server-source \
            libXext-devel libX11-devel libXi-devel libXfixes-devel \
            libXdamage-devel libXrandr-devel libXt-devel libXdmcp-devel \
            libXinerama-devel mesa-libGL-devel libxshmfence-devel \
            libdrm-devel mesa-libgbm-devel \
            xorg-x11-util-macros xorg-x11-xtrans-devel libXtst-devel \
            xorg-x11-font-utils \
            libxkbfile-devel libXfont2-devel xcb-util-keysyms-devel

        # TigerVNC
        package_install zlib-devel openssl-devel libpng-devel \
            libjpeg-turbo-devel \
            pixman-devel gnutls-devel nettle-devel gmp-devel \
            libpciaccess-devel freetype-devel pam-devel
    fi

    if [ $MACOS -eq 1 ]; then
        package_install nasm autoconf automake libtool pkg-config doxygen zstd curl
    fi
fi

if [ $DEFAULT_LIBS -eq 1 ]; then
    echo "Install default dependency libraries ......"
    if [ "$PACKAGE_TOOL" = "apt" ]; then
        package_install libvncserver-dev
        if [ $FREERDP -ne 1 ]; then
            package_install freerdp2-dev
        fi
        if [ $LIBSSH -ne 1 ]; then
            package_install libssh-dev
        fi
        # File transfer
        package_install libcurl4-openssl-dev
        # Qt6
        if [ $QT -ne 1 ]; then
            package_install qmake6 qt6-tools-dev qt6-tools-dev-tools \
                qt6-base-dev qt6-base-dev-tools qt6-qpa-plugins \
                libqt6svg6-dev qt6-l10n-tools qt6-translations-l10n \
                qt6-scxml-dev qt6-multimedia-dev qt6-serialport-dev qt6-websockets-dev \
                qt6-webengine-dev qt6-webengine-dev-tools qt6-positioning-dev qt6-webchannel-dev
        fi
        if [ $QTKEYCHAIN -ne 1 ]; then
            package_install qtkeychain-qt6-dev
        fi
    fi # apt

    if [ "$PACKAGE_TOOL" = "dnf" ]; then
        if [ $QT -ne 1 ]; then
            package_install qt6-qttools-devel qt6-qtbase-devel qt6-qtmultimedia-devel \
                qt6-qt5compat-devel qt6-qtmultimedia-devel qt6-qtscxml-devel \
                qt6-qtserialport-devel qt6-qtsvg-devel qt6-qtwebsockets-devel \
                qt6-qtwebengine-devel qt6-qtwebengine-devtools qt6-qtpositioning-devel qt6-qtwebchannel-devel
        fi

        dnf builddep -y ${REPO_ROOT}/Package/rpm/rabbitremotecontrol.spec
    fi

    if [ $MACOS -eq 1 ]; then
        package_install qt freerdp libvncserver libssh pcapplusplus qtkeychain #libpcap
    fi
fi

if [ $QT -eq 1 ]; then
    echo "Install qt ${QT_VERSION} ......"
    pushd "$TOOLS_DIR"
    if [ ! -d qt_`uname -m` ]; then
        # See: https://ddalcino.github.io/aqt-list-server/
        #      https://www.cnblogs.com/clark1990/p/17942952
        #if [ "$PACKAGE_TOOL" = "apt" ]; then
        #    package_install python3-pip python3-pip-whl python3-pipdeptree cpio
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

if [ $RabbitCommon -eq 1 ]; then
    echo "Install RabbitCommon ......"
    pushd "$SOURCE_DIR"
    if [ ! -d RabbitCommon ]; then
        git clone https://github.com/KangLin/RabbitCommon.git
    else
        pushd RabbitCommon
        git pull
        popd
    fi
    popd
fi

if [ $LIBSSH -eq 1 ]; then
    echo "Install libssh ......"
    pushd "$SOURCE_DIR"
    if [ ! -d ${INSTALL_DIR}/lib/cmake/libssh ]; then
        git clone -b libssh-0.11.3 --depth=1 https://git.libssh.org/projects/libssh.git
        cmake -E make_directory $BUILD_DEPEND_DIR/libssh
        pushd $BUILD_DEPEND_DIR/libssh
        cmake -S $SOURCE_DIR/libssh -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE} \
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
            -DWITH_EXAMPLES=OFF
        cmake --build . --config Release --parallel $(nproc)
        cmake --build . --config Release --target install
        popd
    fi
    popd
fi

if [ $FREERDP -eq 1 ]; then
    echo "Install FreeRDP ......"
    pushd "$SOURCE_DIR"
    if [ ! -d ${INSTALL_DIR}/lib/cmake/FreeRDP3 ]; then
        git clone -b 3.22.0 --depth=1 https://github.com/FreeRDP/FreeRDP.git
        cmake -E make_directory $BUILD_DEPEND_DIR/FreeRDP
        pushd $BUILD_DEPEND_DIR/FreeRDP
        cmake -S $SOURCE_DIR/FreeRDP \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE} \
          -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" \
          -DWITH_SAMPLE=OFF \
          -DWITH_SERVER=ON \
          -DWITH_CLIENT=OFF \
          -DWITH_CLIENT_SDL=OFF \
          -DWITH_KRB5=OFF \
          -DWITH_MANPAGES=OFF \
          -DWITH_WAYLAND=OFF
        cmake --build . --config Release --parallel $(nproc)
        cmake --build . --config Release --target install
        popd
    fi
    popd
fi

if [ $TIGERVNC -eq 1 ]; then
    echo "Install tigervnc ......"
    pushd "$SOURCE_DIR"
    if [ ! -d ${INSTALL_DIR}/lib/cmake/tigervnc ]; then
      git clone --depth=1 https://github.com/KangLin/tigervnc.git
      cmake -E make_directory $BUILD_DEPEND_DIR/tigervnc
      pushd $BUILD_DEPEND_DIR/tigervnc
      cmake -S $SOURCE_DIR/tigervnc -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
          -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE} \
          -DBUILD_TESTS=OFF -DBUILD_VIEWER=OFF -DENABLE_NLS=OFF \
          -DCMAKE_POLICY_VERSION_MINIMUM=3.5
      cmake --build . --config Release --parallel $(nproc)
      cmake --build . --config Release --target install
      popd
    fi
    popd
fi

if [ $PCAPPLUSPLUS -eq 1 ]; then
    echo "Install PcapPlusPlus ......"
    pushd "$SOURCE_DIR"
    if [ ! -d ${INSTALL_DIR}/lib/cmake/pcapplusplus ]; then
        git clone -b v25.05 --depth=1 https://github.com/seladb/PcapPlusPlus.git
        cmake -E make_directory $BUILD_DEPEND_DIR/PcapPlusPlus
        pushd $BUILD_DEPEND_DIR/PcapPlusPlus
        cmake -S $SOURCE_DIR/PcapPlusPlus -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE} \
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
            -DPCAPPP_BUILD_EXAMPLES=OFF \
            -DPCAPPP_BUILD_TESTS=OFF \
            -DPCAPPP_BUILD_TUTORIALS=OFF
        cmake --build . --config Release --parallel $(nproc)
        cmake --build . --config Release --target install
        popd
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
      cmake -E make_directory $BUILD_DEPEND_DIR/libdatachannel
      pushd $BUILD_DEPEND_DIR/libdatachannel
      cmake -S $SOURCE_DIR/libdatachannel -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE} \
          -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
      cmake --build . --config Release --parallel $(nproc)
      cmake --build . --config Release --target install
      popd
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
      cmake -E make_directory $BUILD_DEPEND_DIR/qtservice
      pushd $BUILD_DEPEND_DIR/qtservice
      cmake -S $SOURCE_DIR/qtservice -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
          -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE}
      cmake --build . --config Release --parallel $(nproc)
      cmake --build . --config Release --target install
      popd
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
            git clone --branch 2.3.0 --depth=1 https://github.com/lxqt/lxqt-build-tools.git
        fi
        pushd lxqt-build-tools
        if [ $MACOS -eq 1 ]; then
            sed -i '' "s/LXQT_MIN_LINGUIST_VERSION \"6.6\"/LXQT_MIN_LINGUIST_VERSION \"6.0\"/g" CMakeLists.txt
        else
            sed -i "s/LXQT_MIN_LINGUIST_VERSION \"6.6\"/LXQT_MIN_LINGUIST_VERSION \"6.0\"/g" CMakeLists.txt
        fi
        popd
        cmake -E make_directory $BUILD_DEPEND_DIR/lxqt-build-tools
        pushd $BUILD_DEPEND_DIR/lxqt-build-tools
        cmake -S $SOURCE_DIR/lxqt-build-tools -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
            -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE}
        cmake --build . --config Release --parallel $(nproc)
        cmake --build . --config Release --target install
        popd
    fi
    if [ ! -d ${INSTALL_DIR}/lib/cmake/qtermwidget6 ]; then
        pushd "$SOURCE_DIR"
        if [ ! -d qtermwidget ]; then
            git clone --depth=1 https://github.com/KangLin/qtermwidget.git
        fi
        popd
        cmake -E make_directory $BUILD_DEPEND_DIR/qtermwidget
        pushd $BUILD_DEPEND_DIR/qtermwidget
        cmake -S $SOURCE_DIR/qtermwidget -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
            -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE} \
            -Dlxqt2-build-tools_DIR=${INSTALL_DIR}/share/cmake/lxqt2-build-tools
        cmake --build . --config Release --parallel $(nproc)
        cmake --build . --config Release --target install
        popd
    fi
    popd
fi

if [ $QTKEYCHAIN -eq 1 ]; then
    echo "Install QtKeyChain ......"
    pushd "$SOURCE_DIR"
    if [ ! -d ${INSTALL_DIR}/lib/cmake/Qt6Keychain ]; then
        git clone -b 0.15.0 --depth=1 https://github.com/frankosterfeld/qtkeychain.git
        cmake -E make_directory $BUILD_DEPEND_DIR/qtkeychain
        pushd $BUILD_DEPEND_DIR/qtkeychain
        cmake -S $SOURCE_DIR/qtkeychain -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
            -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE} \
            -DBUILD_WITH_QT6:BOOL=ON
        cmake --build . --config Release --parallel $(nproc)
        cmake --build . --config Release --target install
        popd
    fi
    popd
fi

if [ $QFtpServer -eq 1 ]; then
    echo "Install QFtpServer ......"
    pushd "$SOURCE_DIR"
    if [ ! -d ${INSTALL_DIR}/lib/cmake/QFtpServerLib/QFtpServerLib ]; then
        git clone --depth=1 https://github.com/KangLin/QFtpServer.git
        cmake -E make_directory $BUILD_DEPEND_DIR/QFtpServer
        pushd $BUILD_DEPEND_DIR/QFtpServer
        cmake -S $SOURCE_DIR/QFtpServer -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
            -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE} \
            -DWITH_APP=OFF
        cmake --build . --config Release --parallel $(nproc)
        cmake --build . --config Release --target install
        popd
    fi
    popd
fi

popd
popd
