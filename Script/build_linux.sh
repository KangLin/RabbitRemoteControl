#!/bin/bash

set -e
#set -x

DOCKER=0
DEB=0
RPM=0
APPIMAGE=0
if [ -z "$DOCKERT_IMAGE" ]; then
    DOCKERT_IMAGE=ubuntu
fi
if [ -z "$BUILD_VERBOSE" ]; then
    BUILD_VERBOSE=OFF
fi

usage_long() {
    echo "$0 [-h|--help] [-v|--verbose[=0|1]] [--docker] [--deb] [--rpm] [--appimage] [--docker-image=<docker image>] --install=<install directory>] [--source=<source directory>] [--tools=<tools directory>]"
    echo "  --help|-h: Show help"
    echo "  -v|--verbose: Show build verbose"
    echo "  --docker: run docket for build"
    echo "  --deb: build deb package"
    echo "  --rpm: build rpm package"
    echo "  --appimage: build AppImage"
    echo "  --docker-image: The name of docker image"
    echo "Directory:"
    echo "  --install: Set install directory"
    echo "  --source: Set source directory"
    echo "  --tools: Set tools directory"
}

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
    OPTS=help,verbose::,docker::,deb::,rpm::,appimage::,docker-image::,install:,source:,tools:
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
        -v |--verbose)
            case $2 in
                "")
                    BUILD_VERBOSE=ON;;
                *)
                    BUILD_VERBOSE=$2;;
            esac
            shift 2
            ;;
        --docker)
            case $2 in
                "")
                    DOCKER=1;;
                *)
                    DOCKER=$2;;
            esac
            shift 2
            ;;
        --deb)
            case $2 in
                "")
                    DEB=1;;
                *)
                    DEB=$2;;
            esac
            shift 2
            ;;
        --rpm)
            case $2 in
                "")
                    RPM=1;;
                *)
                    RPM=$2;;
            esac
            shift 2
            ;;
        --appimage)
            case $2 in
                "")
                    APPIMAGE=1;;
                *)
                    APPIMAGE=$2;;
            esac
            shift 2
            ;;
        --docker-image)
            case $2 in
                "")
                    DOCKERT_IMAGE=1;;
                *)
                    DOCKERT_IMAGE=$2;;
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

## Basic variables

CURDIR=$(dirname $(readlink -f $0))
REPO_ROOT=$(readlink -f $(dirname $(dirname $(readlink -f $0))))

if [ -z "$BUILD_LINUX_DIR" ]; then
    BUILD_LINUX_DIR=${REPO_ROOT}/build_linux
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

if [ $DOCKER -eq 1 ]; then

    ## Copy the source code to build directory
    pushd ${REPO_ROOT}
    # Generated source archive
    git archive  --format=tar.gz -o RabbitRemoteControl.tar.gz --prefix RabbitRemoteControl/ HEAD
    if [ -d ${BUILD_LINUX_DIR} ]; then
        rm -fr ${BUILD_LINUX_DIR}
    fi
    mkdir -p ${BUILD_LINUX_DIR}
    mv RabbitRemoteControl.tar.gz ${BUILD_LINUX_DIR}/
    chmod a+rw ${BUILD_LINUX_DIR}/RabbitRemoteControl.tar.gz
    popd

    if [ $DEB -eq 1 ]; then
       docker run --volume ${BUILD_LINUX_DIR}:/home/build  --privileged --interactive --rm ${DOCKERT_IMAGE} \
           bash -e -x -c "
           tar -C ~ -xf /home/build/RabbitRemoteControl.tar.gz
           ~/RabbitRemoteControl/Script/build_linux.sh --deb --verbose ${BUILD_VERBOSE}
           cp ~/rabbitremotecontrol*.deb /home/build/
           "
    fi
    if [ $APPIMAGE -eq 1 ]; then
        docker run --volume ${BUILD_LINUX_DIR}:/home/build --privileged --interactive --rm ${DOCKERT_IMAGE} \
            bash -e -x -c "
            tar -C ~ -xf /home/build/RabbitRemoteControl.tar.gz
            ~/RabbitRemoteControl/Script/build_linux.sh --appimage --verbose ${BUILD_VERBOSE}
            cp ~/RabbitRemoteControl/RabbitRemoteControl_`uname -m`.AppImage /home/build/
            "
    fi
    if [ $RPM -eq 1 ]; then
        docker run --volume ${BUILD_LINUX_DIR}:/home/build --privileged --interactive --rm ${DOCKERT_IMAGE} \
            bash -e -x -c "
            mkdir -p ~/rpmbuild/SOURCES/
            cp /home/build/RabbitRemoteControl.tar.gz ~/rpmbuild/SOURCES/
            tar -C ~ -xf /home/build/RabbitRemoteControl.tar.gz
            # Install getopt
            dnf install -y util-linux
            ~/RabbitRemoteControl/Script/build_linux.sh --rpm --verbose ${BUILD_VERBOSE}
            cp ~/rpmbuild/RPMS/`uname -m`/rabbitremotecontrol*.rpm /home/build/
            "
    fi
    exit 0
fi

pushd $REPO_ROOT/Script

if [ $DEB -eq 1 ]; then
    echo "build deb package ......"
    ./build_depend.sh --apt_update --base --default  \
        --rabbitcommon --tigervnc --pcapplusplus \
        --install ${INSTALL_DIR} \
        --source ${SOURCE_DIR} \
        --tools ${TOOLS_DIR} \
        --verbose ${BUILD_VERBOSE}

    export RabbitCommon_ROOT=${SOURCE_DIR}/RabbitCommon
    export CMAKE_PREFIX_PATH=${INSTALL_DIR}:${CMAKE_PREFIX_PATH}
    export INSTALL_DIR=${INSTALL_DIR}
    ./build_debpackage.sh
fi

if [ $APPIMAGE -eq 1 ]; then
    echo "build AppImage ......"
    ./build_depend.sh --apt_update --base --rabbitcommon\
        --tigervnc --freerdp --pcapplusplus \
        --install ${INSTALL_DIR} \
        --source ${SOURCE_DIR} \
        --tools ${TOOLS_DIR} \
        --verbose ${BUILD_VERBOSE} \
        --qt ${QT_VERSION}
    
    export QT_ROOT=${TOOLS_DIR}/qt_`uname -m`
    export Qt6_DIR=$QT_ROOT
    export QMAKE=$QT_ROOT/bin/qmake
    export PATH=$QT_ROOT/libexec:$PATH
    export PKG_CONFIG_PATH=$QT_ROOT/lib/pkgconfig:${INSTALL_DIR}/lib/pkgconfig:$PKG_CONFIG_PATH
    export LD_LIBRARY_PATH=$QT_ROOT/lib:${INSTALL_DIR}/lib:$LD_LIBRARY_PATH
    export QT_PLUGIN_PATH=$QT_ROOT/plugins
    export CMAKE_PREFIX_PATH=$QT_ROOT:${INSTALL_DIR}:${CMAKE_PREFIX_PATH}
    export RabbitCommon_ROOT=${SOURCE_DIR}/RabbitCommon
    export BUILD_FREERDP=ON
    apt install -y -q fuse3
    ./build_appimage.sh --install ${INSTALL_DIR} --verbose ${BUILD_VERBOSE}
fi

if [ $RPM -eq 1 ]; then
    ./build_depend.sh --base --default --package-tool=dnf \
        --rabbitcommon --tigervnc --pcapplusplus \
        --install ${INSTALL_DIR} \
        --source ${SOURCE_DIR} \
        --tools ${TOOLS_DIR} \
        --verbose ${BUILD_VERBOSE}
    dnf builddep -y ${REPO_ROOT}/Package/rpm/rabbitremotecontrol.spec
    ./build_rpm_package.sh \
        --install ${INSTALL_DIR} \
        --source ${SOURCE_DIR} \
        --tools ${TOOLS_DIR} \
        --verbose ${BUILD_VERBOSE}
fi

popd
