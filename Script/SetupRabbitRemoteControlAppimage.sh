#!/bin/bash

# Setup rabbit remote control shell script
# Author: Kang Lin <kl222@126.com>

#set -v
set -e
#set -x

INSTALL_DIR=~/AppImage/io.github.KangLin.RabbitRemoteControl
usage_long() {
    echo "$0 [-h|--help] [--install=<install directory>]"
    echo "  -h|--help: show help"
    echo "Directory:"
    echo "  --install: Set install directory"
    exit
}
# [如何使用getopt和getopts命令解析命令行选项和参数](https://zhuanlan.zhihu.com/p/673908518)
# [【Linux】Shell命令 getopts/getopt用法详解](https://blog.csdn.net/arpospf/article/details/103381621)
if command -V getopt >/dev/null; then
    #echo "getopt is exits"
    #echo "original parameters=[$@]"
    # -o 或 --options 选项后面是可接受的短选项，如 ab:c:: ，表示可接受的短选项为 -a -b -c ，
    # 其中 -a 选项不接参数，-b 选项后必须接参数，-c 选项的参数为可选的
    # 后面没有冒号表示没有参数。后跟有一个冒号表示有参数。跟两个冒号表示有可选参数。
    # -l 或 --long 选项后面是可接受的长选项，用逗号分开，冒号的意义同短选项。
    # -n 选项后接选项解析错误时提示的脚本名字
    OPTS=help,install:
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

if [ -f ~/.local/share/applications/io.github.KangLin.RabbitRemoteControl.AppImage.desktop ]; then
    OLD_UNINSTALL=$(dirname $(readlink -f ~/.local/share/applications/io.github.KangLin.RabbitRemoteControl.AppImage.desktop))
    if [ -f $OLD_UNINSTALL/uninstall.sh ]; then
        $OLD_UNINSTALL/uninstall.sh
    fi
fi

INSTALL_DIR=$(readlink -f $INSTALL_DIR)
if [ ! -d $INSTALL_DIR ]; then
    mkdir -p $INSTALL_DIR
fi

ROOT_DIR=$(dirname $(readlink -f $0))
if [ ! -d /usr/share/icons/hicolor/scalable/apps ]; then
    mkdir -p /usr/share/icons/hicolor/scalable/apps
fi

pushd $ROOT_DIR > /dev/null 

APPIMAGE_FILE=`ls RabbitRemoteControl_*.AppImage`
if [ $INSTALL_DIR != $ROOT_DIR ]; then
    cp $APPIMAGE_FILE $INSTALL_DIR/$APPIMAGE_FILE
    cp io.github.KangLin.RabbitRemoteControl.svg $INSTALL_DIR/io.github.KangLin.RabbitRemoteControl.svg
    cp io.github.KangLin.RabbitRemoteControl.desktop $INSTALL_DIR/io.github.KangLin.RabbitRemoteControl.desktop
fi

sed -i "s#Exec=.*#Exec=${INSTALL_DIR}/${APPIMAGE_FILE}#g" $INSTALL_DIR/io.github.KangLin.RabbitRemoteControl.desktop
if [ ! -f ~/.local/share/applications/io.github.KangLin.RabbitRemoteControl.AppImage.desktop ]; then
    ln -s $INSTALL_DIR/io.github.KangLin.RabbitRemoteControl.desktop ~/.local/share/applications/io.github.KangLin.RabbitRemoteControl.AppImage.desktop
fi
if [ ! -d ~/.icons/hicolor/scalable/apps ]; then
    mkdir -p ~/.icons/hicolor/scalable/apps
fi
if [ ! -f ~/.icons/hicolor/scalable/apps/io.github.KangLin.RabbitRemoteControl.svg ]; then
    ln -s $INSTALL_DIR/io.github.KangLin.RabbitRemoteControl.svg ~/.icons/hicolor/scalable/apps/io.github.KangLin.RabbitRemoteControl.svg
fi

echo "echo \"Uninstall rabbit remote control in $(dirname $(readlink -f ~/.local/share/applications/io.github.KangLin.RabbitRemoteControl.AppImage.desktop))\"" > $INSTALL_DIR/uninstall.sh
echo "rm ~/.local/share/applications/io.github.KangLin.RabbitRemoteControl.AppImage.desktop" >> $INSTALL_DIR/uninstall.sh
echo "rm ~/.icons/hicolor/scalable/apps/io.github.KangLin.RabbitRemoteControl.svg" >> $INSTALL_DIR/uninstall.sh
echo "rm -fr $INSTALL_DIR" >> $INSTALL_DIR/uninstall.sh
chmod u+x $INSTALL_DIR/uninstall.sh
chmod u+x $INSTALL_DIR/$APPIMAGE_FILE

echo ""
echo "Install rabbit remote control AppImage to \"$INSTALL_DIR\"."
echo "If you want to uninstall it. Please execute:"
echo "    $INSTALL_DIR/unistasll.sh"
echo ""

popd > /dev/null
