#!/bin/bash
# Author: Kang Lin <kl222@126.com>

# See: - https://docs.appimage.org/packaging-guide/from-source/native-binaries.html#examples
#      - https://luyuhuang.tech/2024/04/19/appimage.html

#See: https://blog.csdn.net/alwaysbefine/article/details/114187380
#set -x
set -e
#set -v

usage_long() {
    echo "$0 [--install=<install directory>] [--source=<source directory>] [--tools=<tools directory>] [--build=<build directory>] [-h|--help]"
    echo "  --help|-h: Show help"
    echo "Directory:"
    echo "  --install: Set install directory"
    echo "  --source: Set source directory"
    echo "  --tools: Set tools directory"
    echo "  --build: set build directory"
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
    OPTS=help,install:,source:,tools:,build:
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

## building in temporary directory to keep system clean
## use RAM disk if possible (as in: not building on CI system like Travis, and RAM disk is available)
#if [ "$CI" == "" ] && [ -d /dev/shm ]; then
#    TEMP_BASE=/dev/shm
#else
#    TEMP_BASE=/tmp
#fi
#BUILD_DIR=$(mktemp -d -p "$TEMP_BASE" rabbitremotecontrol-appimage-build-XXXXXX)
## make sure to clean up build dir, even if errors occur

# store repo root as variable
REPO_ROOT=$(readlink -f $(dirname $(dirname $(readlink -f $0))))
OLD_CWD=$(readlink -f .)

pushd "$REPO_ROOT"

if [ -z "$BUILD_DIR" ]; then
    BUILD_DIR=build_appimage
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

cleanup () {
    if [ -d "${BUILD_DIR}" ]; then
        rm -rf "${BUILD_DIR}"
    fi
}
if [ "$CI" != "" ]; then
    trap cleanup EXIT
fi

pushd "${TOOLS_DIR}"
if [ ! -f linuxdeploy-`uname -m`.AppImage ]; then
    wget https://github.com/linuxdeploy/linuxdeploy/releases/download/1-alpha-20250213-2/linuxdeploy-`uname -m`.AppImage
    chmod u+x linuxdeploy-`uname -m`.AppImage
fi
if [ ! -f linuxdeploy-plugin-qt-`uname -m`.AppImage ]; then
    wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/1-alpha-20250213-1/linuxdeploy-plugin-qt-`uname -m`.AppImage
    chmod u+x linuxdeploy-plugin-qt-`uname -m`.AppImage
fi
popd

echo "Compile RabbitRemoteControl ......"
INSTALL_APP_DIR=AppDir/usr
cmake "$REPO_ROOT" \
  -DCMAKE_INSTALL_PREFIX=/usr \
  -DCMARK_SHARED=OFF \
  -DCMARK_TESTS=OFF \
  -DCMARK_STATIC=ON \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_QUIWidget=OFF \
  -DBUILD_APP=ON \
  -DBUILD_FREERDP=ON
cmake --build . --config Release --verbose --parallel $(nproc)
cmake --install . --config Release --component DependLibraries --prefix ${INSTALL_APP_DIR}
cmake --install . --config Release --component Runtime --prefix ${INSTALL_APP_DIR}
cmake --install . --config Release --component Application --prefix ${INSTALL_APP_DIR}
cmake --install . --config Release --component Plugin --prefix ${INSTALL_APP_DIR}

echo "Build AppImage ......"
# See: https://github.com/linuxdeploy/linuxdeploy-plugin-qt
#export QMAKE=$QT_ROOT/bin/qmake6
#export PATH=$QT_ROOT/libexec:$PATH
export EXTRA_PLATFORM_PLUGINS="libqxcb.so"
# Icons from theme are not displayed in QtWidgets Application: https://github.com/linuxdeploy/linuxdeploy-plugin-qt/issues/17
export EXTRA_QT_MODULES="svg"
echo "QT_ROOT: $QT_ROOT"
echo "Qt6_DIR: $Qt6_DIR"
echo "QMAKE: $QMAKE"
echo "EXTRA_PLATFORM_PLUGINS: $EXTRA_PLATFORM_PLUGINS"
echo "EXTRA_QT_MODULES: $EXTRA_QT_MODULES"
echo "LD_LIBRARY_PATH: $LD_LIBRARY_PATH"
echo "QT_PLUGIN_PATH: $QT_PLUGIN_PATH"
echo "PATH: $PATH"
$QMAKE --version
${TOOLS_DIR}/linuxdeploy-`uname -m`.AppImage --appdir=AppDir \
    --plugin qt \
    --output appimage \
    --deploy-deps-only=${INSTALL_APP_DIR}/plugins/Client

chmod a+x Rabbit_Remote_Control-`uname -m`.AppImage

if [ -z "$RabbitRemoteControl_VERSION" ]; then
    cp Rabbit_Remote_Control-`uname -m`.AppImage $REPO_ROOT
else
    mv Rabbit_Remote_Control-`uname -m`.AppImage ../RabbitRemoteControl_${RabbitRemoteControl_VERSION}_Linux_`uname -m`.AppImage
fi

popd
popd
