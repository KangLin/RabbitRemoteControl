#!/bin/bash

set -e

#获取运行的程序名
PRONAME=$(readlink -f $0)
#获取文件运行的当前目录
INSTALL_PATH=$(readlink -f $(dirname $(dirname $PRONAME)))

echo "INSTALL_PATH:$INSTALL_PATH"
export LD_LIBRARY_PATH=$INSTALL_PATH/bin:$INSTALL_PATH/lib:$INSTALL_PATH/lib/`uname -m`-linux-gnu:$INSTALL_PATH/plugins/Client:$LD_LIBRARY_PATH

if [[ "$OSTYPE" == "darwin"* ]]; then
  xattr -r -d com.apple.quarantine "$INSTALL_PATH"
  if [[ -f "$INSTALL_PATH/MacOS/RabbitRemoteControlApp" ]]; then
    # Production
    echo "LD_LIBRARY_PATH:$LD_LIBRARY_PATH"
    $INSTALL_PATH/MacOS/RabbitRemoteControlApp $*
  else
    # Development
    eval "$(/opt/homebrew/bin/brew shellenv)"
    LD_LIBRARY_PATH=$HOMEBREW_PREFIX/lib:$LD_LIBRARY_PATH
    DYLD_FRAMEWORK_PATH=$HOMEBREW_PREFIX/lib
    eval "$(/usr/local/bin/brew shellenv)"
    export DYLD_FRAMEWORK_PATH=$DYLD_FRAMEWORK_PATH:$HOMEBREW_PREFIX/lib
    export LD_LIBRARY_PATH=$HOMEBREW_PREFIX/lib:$LD_LIBRARY_PATH
    echo "LD_LIBRARY_PATH:$LD_LIBRARY_PATH"
    echo "DYLD_FRAMEWORK_PATH:$DYLD_FRAMEWORK_PATH"
    $INSTALL_PATH/bin/RabbitRemoteControlApp $*
  fi
else
  echo "LD_LIBRARY_PATH:$LD_LIBRARY_PATH"
  $INSTALL_PATH/bin/RabbitRemoteControlApp $*
fi
