#!/bin/bash

export LD_LIBRARY_PATH=${SNAP}/qt/lib:${SNAP}/usr/local/lib:${SNAP}/opt/RabbitRemoteControl/lib/:${LD_LIBRARY_PATH}
export PATH=$SNAP/opt/RabbitRemoteControl/bin:$PATH
export QT_PLUGIN_PATH=${SNAP}/qt/plugins:${QT_PLUGIN_PATH}

RabbitRemoteControlApp


