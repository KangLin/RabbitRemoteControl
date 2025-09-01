#!/bin/bash
# Author: Kang Lin <kl222@126.com>


#See: https://blog.csdn.net/alwaysbefine/article/details/114187380
#set -x
set -e
#set -v

if [ -z "$BUILD_VERBOSE" ]; then
    BUILD_VERBOSE=OFF
fi


usage_long() {
    echo "$0 [--install=<install directory>] [ [-h|--help] [-v|--verbose[=0|1]] --source=<source directory>] [--tools=<tools directory>] [--build=<build directory>]"
    echo "  --help|-h: Show help"
    echo "  -v|--verbose: Show build verbose"
    echo "Directory:"
    echo "  --install: Set install directory"
    echo "  --source: Set source directory"
    echo "  --tools: Set tools directory"
    echo "  --build: set build directory"
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
    OPTS=help,verbose::,install:,source:,tools:,build:
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
        -v |--verbose)
            case $2 in
                "")
                    BUILD_VERBOSE=ON;;
                *)
                    BUILD_VERBOSE=$2;;
            esac
            shift 2
            ;;
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
            BUILD_APPIMAGE_DIR=$2
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

if [ -z "$BUILD_MACOS_DIR" ]; then
    BUILD_MACOS_DIR=${REPO_ROOT}/build_macos
fi
if [ -z "$SOURCE_DIR" ]; then
    SOURCE_DIR=${BUILD_MACOS_DIR}/source
fi
if [ -z "$TOOLS_DIR" ]; then
    TOOLS_DIR=${BUILD_MACOS_DIR}/tools
fi
if [ -z "$INSTALL_DIR" ]; then
    INSTALL_DIR=${BUILD_MACOS_DIR}/install_lib
fi
#rm -fr $SOURCE_DIR
mkdir -p $SOURCE_DIR
#rm -fr $TOOLS_DIR
mkdir -p $TOOLS_DIR
#rm -fr $INSTALL_DIR
mkdir -p $INSTALL_DIR

echo "Repo folder: $REPO_ROOT"
echo "Old folder: $OLD_CWD"
echo "Current folder: `pwd`"
echo "BUILD_MACOS_DIR: $BUILD_MACOS_DIR"
echo "TOOLS_DIR: $TOOLS_DIR"
echo "SOURCE_DIR: $SOURCE_DIR"
echo "INSTALL_DIR: $INSTALL_DIR"

pushd "$BUILD_MACOS_DIR"

cmake "$REPO_ROOT" \
  -DCMAKE_INSTALL_PREFIX=install \
  -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE} \
  -DCMARK_SHARED=OFF \
  -DCMARK_TESTS=OFF \
  -DCMARK_STATIC=ON \
  -DWITH_CMARK=OFF \
  -DWITH_CMARK_GFM=ON \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH=${INSTALL_DIR}/lib/cmake \
  -DCMAKE_TOOLCHAIN_FILE="$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" \
  -DX_VCPKG_APPLOCAL_DEPS_INSTALL=ON \
  -DVCPKG_VERBOSE=ON \
  -DVCPKG_TARGET_TRIPLET=$env{VCPKG_TARGET_TRIPLET} \
  -DRABBIT_WITH_MACDEPLOY=OFF \
  -DBUILD_FREERDP=ON
cmake --build . --config Release --parallel $(nproc)
cmake --install . --config Release --strip --component DependLibraries
cmake --install . --config Release --strip --component Runtime
cmake --install . --config Release --strip --component Plugin
cmake --install . --config Release --strip --component Application
if [ -d "${INSTALL_DIR}/share/qtermwidget6" ]; then
    cp -r ${INSTALL_DIR}/share/qtermwidget6 ${INSTALL_APP_DIR}/share/
else
    echo "${INSTALL_DIR}/share/qtermwidget6 is not exist"
fi

rsync -av --exclude="RabbitRemoteControlApp.app" install install/RabbitRemoteControlApp.app/Contents

if [ -d "${QT_ROOT}/bin/macdeployqt" ]; then
    ${QT_ROOT}/bin/macdeployqt `pwd`/install/RabbitRemoteControlApp.app -dmg -verbose=3
else
    echo "Don't found macdeployqt, please set QT_ROOT"
fi

popd
