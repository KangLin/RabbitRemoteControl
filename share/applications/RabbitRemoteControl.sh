#!/bin/bash

set -e

#获取运行的程序名
PRONAME=$(readlink -f $0)
#获取文件运行的当前目录
INSTALL_PATH=$(readlink -f $(dirname $(dirname $PRONAME)))

echo "INSTALL_PATH:$INSTALL_PATH"
export LD_LIBRARY_PATH=$INSTALL_PATH/bin:$INSTALL_PATH/lib:$INSTALL_PATH/lib/`uname -m`-linux-gnu:$INSTALL_PATH/plugins/Client:$LD_LIBRARY_PATH
echo "LD_LIBRARY_PATH:$LD_LIBRARY_PATH"
$INSTALL_PATH/bin/RabbitRemoteControlApp $*
