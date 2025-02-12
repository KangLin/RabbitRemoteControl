#!/bin/bash
# Author: Kang Lin <kl222@126.com>

#See: https://blog.csdn.net/alwaysbefine/article/details/114187380
set -x
set -e
#set -v

if [ $# -lt 3 ]; then
    echo "Input parameters[$#] is fail. "
    echo "Please using:"
    echo "$0 old_color new_color path"
    exit
fi

sed -i "s/fill=\"$1\"/fill=\"$2\"/g" `grep "fill=\"$1\"" -rl $3`
