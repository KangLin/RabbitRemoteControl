# Rabbit Remote Control

Author: Kang Lin (kl222@126.com)

-----------------------------------------------------------------------

- Language
  - [<img src="Resource/Image/Chinese.png" alt="Chinese" title="Chinese" width="16" height="16"/>Chinese](README_zh_CN.md)
- Build status
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

### Help us

If you're wondering how could you help, fear not, there are plenty of ways:

Some of them are:

* Spread the good word about RabbitRemoteControl to make it more popular :smile:
* Have an opinion? Make sure to voice it in the [Discussions](https://github.com/KangLin/RabbitRemoteControl/discussions) that need it :wink:
* Fixing easy [Issues](https://github.com/KangLin/RabbitRemoteControl/issues) or [Issues](https://github.com/KangLin/RabbitRemoteControl/issues) that need help. Make sure to read
  [Contributing] first though :wink:
* [Testing] and [reporting bugs]
* [Translating, it's easy]
* [Reviewing and testing pull requests] – you don't need to be able to code to
  do that :wink:
* Distribute this program to various operating systems
  - linux
    + Ubuntu, Debian
    + Redhat
    + openSUSE
    + FreeBSD
    + CentOS
    + Arch Linux
  - Unix
  - Windows 10 mall
  - Android
* Take a task from our Roadmap below


### Roadmap

At the current development stage of this project, there are many functions that have not been completed:

The next steps are:

- [Unsupport protocol](#Supported-protocol)
- FIX bug: [Issues](https://github.com/KangLin/RabbitRemoteControl/issues)

### [License Agreement](License.md "License.md")

Please abide by this agreement and the license agreement of [dependent library](#Dependent-libraries), and thank the author of [dependent library](#Dependent-libraries).
