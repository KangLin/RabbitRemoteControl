# Rabbit Remote Control

Author: Kang Lin (kl222@126.com)

-----------------------------------------------------------------------

- Language
  - [<img src="Resource/Image/Chinese.png" alt="Chinese" title="Chinese" width="16" height="16"/>Chinese](README_zh_CN.md)
- Build status
  - [![Build Status](https://travis-ci.org/KangLin/RabbitRemoteControl.svg?branch=master)](https://travis-ci.org/KangLin/RabbitRemoteControl)
  - [![Build status](https://ci.appveyor.com/api/projects/status/jai7jf3xr2vb44q8?svg=true)](https://ci.appveyor.com/project/KangLin/rabbitremotecontrol)
  - [![Build status](https://github.com/KangLin/RabbitRemoteControl/workflows/CMake/badge.svg)](https://github.com/KangLin/RabbitRemoteControl/workflows/CMake/badge.svg)
- [Change log](ChangeLog.md)

-----------------------------------------------------------------------

### Introduction
Rabbit Remote Control is a cross-platform, multi-protocol remote control software.

**Allows you to use any device and system anywhere and remotely manage any device and system in any way**. 

#### Supported operating system (See: [https://doc.qt.io/qt-5/supported-platforms.html](https://doc.qt.io/qt-5/supported-platforms.html))
- [x] Windows
- [x] Unix/Linux
- [x] Android
- [ ] mac os
- [ ] IPHONE
- [ ] WINCE

I have no device so I did not compile and test. Please test by yourself if you have device.
Interested and capable friends are welcome to participate in the development of the project.

If you want to donate the developed device for me, please contact: (email: kl222@126.com)

#### Supported protocol
- [x] VNC(Virtual network console): [RFB](https://github.com/rfbproto/rfbproto)
- [x] Remote desktop protocol: [RDP](https://github.com/FreeRDP/FreeRDP/wiki/Reference-Documentation)
- [ ] [Spice](https://www.spice-space.org/)
- [ ] [SSH]()
- [ ] [TELNET]()

#### Dependent
##### Tools
- [x] [Qt](qt.io)
- [x] c compiler
  + gcc
  + MSVC
- [cmake](https://cmake.org/)

##### Dependent libraries
- [x] VNC
  + [x] [Optional] [libvncserver](https://github.com/LibVNC/libvncserver)
  + [x] [Optional] [TigerVnc](https://github.com/KangLin/tigervnc)
- [x] [Optional] [FreeRDP](https://github.com/FreeRDP/FreeRDP)
- [ ] [SSH]
  + [ ] [LIBSSH](https://www.libssh.org)

##### Reference
- [ ] [qtermwidget](https://github.com/lxqt/qtermwidget)

### Donation
If this software is useful to you, or you like it, please donate and support the author. Thank you!

[![donation](https://github.com/KangLin/RabbitCommon/raw/master/Src/Resource/image/Contribute.png "donation")](https://github.com/KangLin/RabbitCommon/raw/master/Src/Resource/image/Contribute.png "donation") 

If it cannot be displayed, please open:
https://gitee.com/kl222/RabbitCommon/raw/master/Src/Resource/image/Contribute.png

[![donation](https://gitee.com/kl222/RabbitCommon/raw/master/Src/Resource/image/Contribute.png "donation")](https://gitee.com/kl222/RabbitCommon/raw/master/Src/Resource/image/Contribute.png "donation")

### Contribution

- [Discussions](https://github.com/KangLin/RabbitRemoteControl/discussions)
- [Issues](https://github.com/KangLin/RabbitRemoteControl/issues)
- [Contributors](https://github.com/KangLin/RabbitRemoteControl/graphs/contributors)

### [License Agreement](License.md "License.md")

Please abide by this agreement and the license agreement of [dependent library](#Dependent-libraries), and thank the author of [dependent library](#Dependent-libraries).
