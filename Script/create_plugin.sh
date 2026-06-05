#!/bin/bash
# Generate plugin from template
# Author: Kang Lin <kl222@126.com>

set -e
#set -x

if [ -z "$BUILD_VERBOSE" ]; then
    BUILD_VERBOSE=OFF
fi

source $(dirname $(readlink -f $0))/common.sh

install_gnu_getopt
if [ "$OS" = "macOS" ]; then
    MACOS=1
    setup_macos
else
    MACOS=0
fi

# Display detailed usage information
usage_long() {
    cat << EOF
$(basename $0) - Generate plugin from template

Usage: $0 [OPTION] PluginName

Options:
  -h, --help            Show this help message
  -v, --verbose[=LEVEL] Set verbose mode. [LEVEL: ON, OFF]

Directory options:
  --install=DIR         Set installation directory

Other options:
  --name=NAME           Plugin name
  --template=NAME       Template name. [NAME: Base(Default), Desktop, Server]

Examples:
  $0 --name=Server

EOF
    exit 0
}

parse_with_getopt() {
    # [如何使用 getopt 和 getopts 命令解析命令行选项和参数](https://zhuanlan.zhihu.com/p/673908518)
    # [【Linux】Shell 命令 getopts/getopt 用法详解](https://blog.csdn.net/arpospf/article/details/103381621)
    if command -V getopt >/dev/null; then
        #echo "getopt is exits"
        #echo "original parameters=[$@]"
        # -o 或 --options 选项后面是可接受的短选项，如 ab:c:: ，表示可接受的短选项为 -a -b -c ，
        # 其中 -a 选项不接参数，-b 选项后必须接参数，-c 选项的参数为可选的
        # 后面没有冒号表示没有参数。后跟有一个冒号表示有参数。跟两个冒号表示有可选参数。
        # -l 或 --long 选项后面是可接受的长选项，用逗号分开，冒号的意义同短选项。
        # -n 选项后接选项解析错误时提示的脚本名字
        OPTS=help,verbose::,install:,name:,template:
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
            --name|--)  # -- 当解析到“选项和参数“与“non-option parameters“的分隔符时终止
                if [ -n "$2" ]; then
                    PLUGIN_NAME=$2
                    shift 2
                else
                    shift
                fi
                ;;
            --template)
                TEMPLATE_NAME=$2
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
            -h | -help)
                usage_long
                shift
                break
                ;;
            *)
                usage_long
                break
                ;;
            esac
        done
    fi
}

if [ $# -eq 0 ]; then
    usage_long
fi

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
    echo ""
    echo_status "Configuration:"
    echo_status "  Plugins directory: $PLUGIN_DIR"
    echo_status "  Plugin name: ${PLUGIN_NAME:-Not set}"
    echo_status "  Install Directory: ${INSTALL_DIR:-Not set}"
    echo_status "  Template name: ${TEMPLATE_NAME-Not set}"
    echo_status "  Template directory: $TEMPLATE_DIR"
    echo_status "  Verbose Mode: ${BUILD_VERBOSE:-Not set(default: OFF)}"
    echo_status "  SED_CMD: $SED_CMD"
    echo ""
}

init_global() {

    # Check if sed supports -E
    if sed -E 's/a/b/g' 1>/dev/null 2>/dev/null <<< "test"; then
        #echo "Using sed with -E flag"
        SED_CMD="sed -i -E"
    else
        #echo "Falling back to BRE pattern"
        # Convert pattern to BRE
        BRE_PATTERN=$(sed_safe_pattern "$VERSION_PATTERN")
        SED_CMD="sed -i"
        VERSION_PATTERN="$BRE_PATTERN"
    fi

    if [ -z "$TEMPLATE_NAME" ]; then
        TEMPLATE_NAME="Base"
    fi
    case $TEMPLATE_NAME in
        Base|Desktop|Server|QtEvent)
            ;;
        *)
            echo_error "The template name is not know"
            exit -1
            ;;
    esac

    TEMPLATE_DIR="${REPO_ROOT}/Plugins/Template/$TEMPLATE_NAME"
    PLUGIN_DIR="${REPO_ROOT}/Plugins"
    if [ -z "$INSTALL_DIR" ]; then
        INSTALL_DIR="${PLUGIN_DIR}/$PLUGIN_NAME"
    fi
    if [ -z "$PLUGIN_NAME" ]; then
        echo_error "The plugin name is empty"
        exit -1
    fi
}

copy_files() {
    echo_status "Copy files to \"$INSTALL_DIR\" ......"
    pushd $TEMPLATE_DIR
    for file in * ; do
       #echo "$SED_CMD \"s/${TEMPLATE_NAME}/${PLUGIN_NAME}/g\" $file"
       if [ -f $file ]; then
           local dest_file=$INSTALL_DIR/${file//Template${TEMPLATE_NAME}/${PLUGIN_NAME}}
           #echo "file: $file"
           #echo "dest_file:$dest_file"
           cp $file $dest_file
           $SED_CMD "s/Template${TEMPLATE_NAME}/${PLUGIN_NAME}/g" $dest_file
       fi
    done
    popd
}

# Parse command line arguments (will override environment variables)
parse_command_line "$@"

## Directory variables
CURDIR=$(dirname $(safe_readlink $0))
REPO_ROOT=$(safe_readlink $(dirname $(dirname $(safe_readlink $0))))

init_global

if [ "$BUILD_VERBOSE" = "ON" ]; then
    show_configuration
fi

echo_status "Make plugin \"$PLUGIN_NAME\" directory ......"
if [ -d "$INSTALL_DIR" ]; then
    echo_warn "The plugin directory is exits"

    read -t 60 -p "Do you continue? (y/N): " INPUT
    if [ "${INPUT:-N}" != "Y" ] && [ "${INPUT:-N}" != "y" ]; then
        echo_error "User cancelled"
        exit 0
    fi

    echo ""
else
    mkdir -p "$INSTALL_DIR"
fi

copy_files

echo ""
echo_info "Please add the following code in $PLUGIN_DIR/CMakeLists.txt"
echo_color_yellow "option(BUILD_$PLUGIN_NAME \"Build $PLUGIN_NAME\" ON)"
echo_color_yellow "if(BUILD_$PLUGIN_NAME)"
echo_color_yellow "    add_subdirectory($PLUGIN_NAME)"
echo_color_yellow "endif()"
echo_info "Then use the cmake parameter \"-DENABLE_UPDATE_TRANSLATIONS:BOOL=ON\" to build, generating translation resource files"