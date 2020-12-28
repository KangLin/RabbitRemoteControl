# Rabbit Remote Control

Author: Kang Lin (kl222@126.com)

-----------------------------------------------------------------------

- [<img src="Resource/Image/Chinese.png" alt="Chinese" title="Chinese" width="16" height="16"/>Chinese](README_zh_CN.md)
- [![Build Status](https://travis-ci.org/KangLin/RabbitRemoteControl.svg?branch=master)](https://travis-ci.org/KangLin/RabbitRemoteControl)
- [![Build status](https://ci.appveyor.com/api/projects/status/jai7jf3xr2vb44q8?svg=true)](https://ci.appveyor.com/project/KangLin/rabbitremotecontrol)
- [Change log](ChangeLog.md)
-----------------------------------------------------------------------
### Origin of this software
As a software engineer, you often need to use remote control software in your daily work.
Initially, the WINDOWS operating system in the local area network used the remote desktop that WINDOWS comes with.
To develop linxu later, you can only use VNC. There are many open source remote control solutions on LINUX system,
but there are few open source solutions under WINDOWS.
Later, you need to remotely control the host on the internal network through the public network.
All the original plans in the local area network failed. Choose TEAMVIWER.
Recently, TEAMVIWER not only needs to collect private information, but also pay.
Because I did not pay, it restricted my use. There is no alternative.
So I decided to write an open source remote control software myself.
**Able to access any system through any network on any system anywhere**.  
This software has just started to develop. Interested friends are welcome to participate.

### Introduction
Rabbit Remote Control is a cross-platform, multi-protocol remote control software.  
**Able to access any system through any network on any system anywhere**. 

#### Supported operating system (See: [https://doc.qt.io/qt-5/supported-platforms.html](https://doc.qt.io/qt-5/supported-platforms.html))
- [x] Windows
- [x] Unix/Linux
- [x] Android
- [ ] mac os
- [ ] IPHONE
- [ ] WINCE

#### Supported protocol
- [x] VNC(Virtual network console): [RFB](https://github.com/rfbproto/rfbproto)
- [x] Remote desktop protocol: [RDP](https://github.com/FreeRDP/FreeRDP/wiki/Reference-Documentation)
- [ ] [Spice](https://www.spice-space.org/)
- [ ] [SSH]()
- [ ] [TELNET]()

#### Dependent libraries
- [x] VNC
  + [x] [Optional] [libvncserver](https://github.com/LibVNC/libvncserver)
  + [x] [Optional] [TigerVnc](https://github.com/KangLin/tigervnc)
- [x] [Optional] [FreeRDP](https://github.com/FreeRDP/FreeRDP)
- [ ] [SSH]
  + [ ] [LIBSSH](https://www.libssh.org)

##### Reference
- [ ] [qtermwidget](https://github.com/lxqt/qtermwidget)

### Donation

[![donation](https://gitee.com/kl222/RabbitCommon/raw/master/Src/Resource/image/Contribute.png "donation")](https://gitee.com/kl222/RabbitCommon/raw/master/Src/Resource/image/Contribute.png "donation")

### Contribution

- [Discussions](https://github.com/KangLin/RabbitRemoteControl/discussions)
- [Issues](https://github.com/KangLin/RabbitRemoteControl/issues)
- [Contributors](https://github.com/KangLin/RabbitRemoteControl/graphs/contributors)

### [License Agreement](License.md "License.md")

Please abide by this agreement and the license agreement of [dependent library](#Dependent-libraries), and thank the author of [dependent library](#Dependent-libraries).
