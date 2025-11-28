#!/bin/bash

set -e

# 安全的 readlink 函数，兼容各种情况
safe_readlink() {
    local path="$1"
    if [ -L "$path" ]; then
        # 如果是符号链接，使用 readlink
        if command -v readlink >/dev/null 2>&1; then
            if readlink -f "$path" >/dev/null 2>&1; then
                readlink -f "$path"
            else
                readlink "$path"
            fi
        else
            # 如果没有 readlink，使用 ls
            ls -l "$path" | awk '{print $NF}'
        fi
    elif [ -e "$path" ]; then
        # 如果不是符号链接但存在，返回绝对路径
        if command -v realpath >/dev/null 2>&1; then
            realpath "$path"
        else
            echo "$(cd "$(dirname "$path")" && pwd)/$(basename "$path")"
        fi
    else
        # 文件不存在，返回原路径
        echo "$path"
    fi
}

#获取运行的程序名
PRONAME=$(safe_readlink -f $0)
#获取文件运行的当前目录
INSTALL_PATH=$(safe_readlink -f $(dirname $(dirname $PRONAME)))

echo "INSTALL_PATH:$INSTALL_PATH"
export LD_LIBRARY_PATH=$INSTALL_PATH/bin:$INSTALL_PATH/lib:$INSTALL_PATH/lib/`uname -m`-linux-gnu:$INSTALL_PATH/plugins/Client:$LD_LIBRARY_PATH
echo "LD_LIBRARY_PATH:$LD_LIBRARY_PATH"
$INSTALL_PATH/bin/RabbitRemoteControlApp $*
