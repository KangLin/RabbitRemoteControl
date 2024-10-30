#!/bin/bash

set -e

#获取运行的程序名
PRONAME=$(readlink -f `basename $0`)
#获取文件运行的当前目录
current_dir=$(readlink -f $(dirname $(dirname $0)))

echo "Current dir: $current_dir"
cd "$current_dir"
INSTALL_PATH=`pwd`
echo "INSTALL_PATH:$INSTALL_PATH"
export LD_LIBRARY_PATH=$INSTALL_PATH/bin:$INSTALL_PATH/lib:$INSTALL_PATH/lib/`uname -m`-linux-gnu:$INSTALL_PATH/plugins/Client:$LD_LIBRARY_PATH
echo "LD_LIBRARY_PATH:$LD_LIBRARY_PATH"
$INSTALL_PATH/bin/RabbitRemoteControlApp $*
