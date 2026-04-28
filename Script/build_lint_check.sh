#!/bin/bash

set -e
#set -x

if [ -z "$BUILD_VERBOSE" ]; then
    BUILD_VERBOSE=OFF
fi

source $(dirname $(readlink -f $0))/common.sh


parse_with_getopt() {
    # [如何使用 getopt 和 getopts 命令解析命令行选项和参数](https://zhuanlan.zhihu.com/p/673908518)
    # [【Linux】Shell 命令 getopts/getopt 用法详解](https://blog.csdn.net/arpospf/article/details/103381621)
    if command -V getopt >/dev/null; then
        echo "getopt is exits"
        #echo "original parameters=[$@]"
        # -o 或 --options 选项后面是可接受的短选项，如 ab:c:: ，表示可接受的短选项为 -a -b -c ，
        # 其中 -a 选项不接参数，-b 选项后必须接参数，-c 选项的参数为可选的
        # 后面没有冒号表示没有参数。后跟有一个冒号表示有参数。跟两个冒号表示有可选参数。
        # -l 或 --long 选项后面是可接受的长选项，用逗号分开，冒号的意义同短选项。
        # -n 选项后接选项解析错误时提示的脚本名字
        OPTS=help,verbose::,install:,source:,tools:,build:,lint::
        ARGS=`getopt -o h,v:: -l $OPTS -n $(basename $0) -- "$@"`
        if [ $? != 0 ]; then
            echo_error "exec getopt fail: $?"
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
                BUILD_DIR="$2"
                shift 2
                ;;
            -v|--verbose)
                case $2 in
                    "")
                        BUILD_VERBOSE=ON;;
                    *)
                        BUILD_VERBOSE=$2;;
                esac
                shift 2
                ;;
            --lint)
                case $2 in
                    "")
                        LINT=1;;
                    *)
                        LINT=$2;;
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
        echo "Build target:"
        echo "  Lint check: $LINT"
        echo "Other Settings:"
        echo "  Verbose Mode: $BUILD_VERBOSE"
        echo "========================="
        echo ""
    fi

    echo "Repo folder: $REPO_ROOT"
    echo "Old folder: $OLD_CWD"
    echo "Current folder: `pwd`"
    echo "SOURCE_DIR: $SOURCE_DIR"
    echo "TOOLS_DIR: $TOOLS_DIR"
    echo "INSTALL_DIR: $INSTALL_DIR"
    echo "BUILD_DIR: $BUILD_DIR"
    echo "BUILD_LINUX_DIR: $BUILD_LINUX_DIR"
}

# Parse command line arguments (will override environment variables)
parse_command_line "$@"

## Directory variables
CURDIR=$(dirname $(safe_readlink $0))
REPO_ROOT=$(safe_readlink $(dirname $(dirname $(safe_readlink $0))))

if [ -z "$BUILD_LINUX_DIR" ]; then
    if [ -z "$BUILD_DIR" ]; then
        BUILD_LINUX_DIR=${REPO_ROOT}/build_linux
    else
        BUILD_LINUX_DIR=${BUILD_DIR}/build_linux
    fi
fi
if [ -z "$SOURCE_DIR" ]; then
    SOURCE_DIR=${BUILD_LINUX_DIR}/source
fi
if [ -z "$TOOLS_DIR" ]; then
    TOOLS_DIR=${BUILD_LINUX_DIR}/tools
fi
if [ -z "$INSTALL_DIR" ]; then
    INSTALL_DIR=${BUILD_LINUX_DIR}/install
fi
#rm -fr $SOURCE_DIR
mkdir -p $SOURCE_DIR
#rm -fr $TOOLS_DIR
mkdir -p $TOOLS_DIR
#rm -fr $INSTALL_DIR
mkdir -p $INSTALL_DIR

# Display configuration
show_configuration

echo_status "Build with lint check ......"
pushd $BUILD_LINUX_DIR

cmake "$REPO_ROOT" \
  -DCMAKE_CXX_COMPILER=clazy -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_INSTALL_PREFIX=/usr \
  -DCMAKE_VERBOSE_MAKEFILE=${BUILD_VERBOSE} \
  -DCMARK_SHARED=OFF \
  -DCMARK_TESTS=OFF \
  -DCMARK_STATIC=ON \
  -DWITH_CMARK=OFF \
  -DWITH_CMARK_GFM=OFF \
  -DENABLE_UPDATE_TRANSLATIONS=OFF \
  -DWITH_WebEngineWidgets=ON \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_QUIWidget=OFF \
  -DBUILD_APP=ON \
  -DBUILD_FREERDP=ON
if [ "$BUILD_VERBOSE"="ON" ]; then
    cmake --build . --config Release --parallel $(nproc) | tee clazy_output.log
else
    cmake --build . --config Release --parallel $(nproc) > clazy_output.log
fi

# 统计错误和警告
errors_found=$(grep -c "error:" clazy_output.log 2>/dev/null || echo "0")
warnings_found=$(grep -c "warning:" clazy_output.log 2>/dev/null || echo "0")

# 处理错误
if [ "$errors_found" -gt 0 ] 2>/dev/null; then
    echo_error "Clazy check found $errors_found error(s):"
    grep "error:" clazy_output.log
    exit 1  # 有错误时退出
fi

# 处理警告
if [ "$warnings_found" -gt 0 ] 2>/dev/null; then
    echo_warn "Clazy check found $warnings_found warning(s):"
    grep "warning:" clazy_output.log
    # 警告不退出，只提示
fi

popd

echo_status "lint check completed"
