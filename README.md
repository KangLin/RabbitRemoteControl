# <img src="App/Resource/Image/App.png" width="48" height="48" />[Rabbit Remote Control](https://github.com/KangLin/RabbitRemoteControl)

Author: Kang Lin (kl222@126.com)

-----------------------------------------------------------------------
- Project position: https://github.com/KangLin/RabbitRemoteControl
- [Project home](https://kanglin.github.io/RabbitRemoteControl/)

- Language
  - [:cn: Chinese](README_zh_CN.md)
- Build status
  - [![Build status](https://github.com/KangLin/RabbitRemoteControl/workflows/CMake/badge.svg)](https://github.com/KangLin/RabbitRemoteControl/workflows/CMake/badge.svg)
  - [![Build status](https://ci.appveyor.com/api/projects/status/jai7jf3xr2vb44q8?svg=true)](https://ci.appveyor.com/project/KangLin/rabbitremotecontrol)
- [Change log](ChangeLog.md)

-----------------------------------------------------------------------

### Introduction
Rabbit Remote Control is a cross-platform, multi-protocol remote control software.

**Allows you to use any device and system in anywhere and remotely manage any device and system in any way**. 

#### Supported operating system (See: [https://doc.qt.io/qt-5/supported-platforms.html](https://doc.qt.io/qt-5/supported-platforms.html))
- [x] Windows
- [x] Unix/Linux
- [x] Android
- [ ] mac os
- [ ] IPHONE
- [ ] WINCE

I have no devices so I did not compile and test. Please test by yourself if you have devices.
Interested and capable friends are welcome to participate in the development of the project.

If you want to donate the developed devices for me, please contact: (email: kl222@126.com)

#### Supported protocol
- [x] VNC(Virtual network console): [RFB](https://github.com/rfbproto/rfbproto)
- [x] Remote desktop protocol: [RDP](https://github.com/FreeRDP/FreeRDP/wiki/Reference-Documentation)
- [ ] [Spice](https://www.spice-space.org/)
- [x] [SSH]()
- [x] [TELNET]()
- [x] Terminal
- [ ] Custom p2p protocol


|        |Windows           |Unix/linux        |Android           |MAC               |IPHONE            |WINCE             |
|--------|------------------|------------------|------------------|------------------|------------------|------------------|
|vnc     |:heavy_check_mark:|:heavy_check_mark:|                  |                  |                  |                  |
|RDP     |:heavy_check_mark:|:heavy_check_mark:|                  |                  |                  |                  |
|Spice   |                  |                  |                  |                  |                  |                  |
|SSH     |                  |:heavy_check_mark:|                  |                  |                  |                  |
|Terminal|                  |:heavy_check_mark:|                  |                  |                  |                  |
|TELNET  |                  |:heavy_check_mark:|                  |                  |                  |                  |
|P2P     |                  |                  |                  |                  |                  |                  |


### Donation
If this software is useful to you, or you like it, please donate and support the author. Thank you!

[![donation](https://github.com/KangLin/RabbitCommon/raw/master/Src/Resource/image/Contribute.png "donation")](https://github.com/KangLin/RabbitCommon/raw/master/Src/Resource/image/Contribute.png "donation") 

If it cannot be displayed, please open:
https://gitee.com/kl222/RabbitCommon/raw/master/Src/Resource/image/Contribute.png

### Contribution

- [Discussions](https://github.com/KangLin/RabbitRemoteControl/discussions)
- [Issues](https://github.com/KangLin/RabbitRemoteControl/issues)
- [Contributors](https://github.com/KangLin/RabbitRemoteControl/graphs/contributors)

If you're wondering how could you help, fear not, there are plenty of ways:

Some of them are:

* [Donation](#Donation) - Provide funds and devices for this project :smile:
* Spread the good word about [RabbitRemoteControl](https://github.com/KangLin/RabbitRemoteControl) to make it more popular :smile:
* Have an opinion? Make sure to voice it in the [Discussions](https://github.com/KangLin/RabbitRemoteControl/discussions) that need it :wink:
* Fixing easy [Issues](https://github.com/KangLin/RabbitRemoteControl/issues) or [Issues](https://github.com/KangLin/RabbitRemoteControl/issues) that need help. Make sure to read
  [Contributing](#Contribution) first though :wink:
* Testing and [reporting bugs](https://github.com/KangLin/RabbitRemoteControl/issues)
* Translating, it's easy :smile:
* Reviewing and testing pull requests – you don't need to be able to code to
  do that :wink:
* Distribute this program to various operating systems
  - Linux
    + Ubuntu, Debian
    + Redhat
    + openSUSE
    + FreeBSD
    + CentOS
    + Arch Linux
  - Unix
  - Windows 10 mall
  - Android
* Be finished [unsupported protocols](#Supported-protocol)

### Download: [https://github.com/KangLin/RabbitRemoteControl/releases](https://github.com/KangLin/RabbitRemoteControl/releases)

- Windows: Supported WINDOWS 7 and later version
  [RabbitRemoteControl-Setup-v0.0.2.exe](https://github.com/KangLin/RabbitRemoteControl/releases/download/v0.0.2/RabbitRemoteControl-Setup-v0.0.2.exe)
- Ubuntu:
  + Ubuntu 20.04: [rabbitremotecontrol_0.0.2_amd64_Ubuntu-20.04.deb](https://github.com/KangLin/RabbitRemoteControl/releases/download/v0.0.2/rabbitremotecontrol_0.0.2_amd64_Ubuntu-20.04.deb)
  + Ubuntu 18.04: [rabbitremotecontrol_0.0.2_amd64_Ubuntu-18.04.deb](https://github.com/KangLin/RabbitRemoteControl/releases/download/v0.0.2/rabbitremotecontrol_0.0.2_amd64_Ubuntu-18.04.deb)

### Road map
- [x] Support common existing remote control protocols
  - [x] Client
    - [x] Client frame
    - [x] Specific protocol support, see: [Supported protocol](#Supported-protocol)
  - [ ] Server
    - [ ] Server framework
    - [ ] Specific protocol support, see: [Supported protocol](#Supported-protocol)
- [ ] Custom point-to-point remote control protocol, see requirements for details: https://github.com/KangLin/RabbitRemoteControl/issues/7

At present, the development of common remote control protocol client has been completed,
and it has preliminary practical functions.
It is necessary for small partners to try it out, find BUG,
and put forward suggestions that can be more convenient to use in practical applications.

For terminal remote control, because qtermwidget only supports LINUX/UNIX, it needs to support other platforms.

The next level of work is to solve point-to-point remote control,
which is also a feature of this project.
See requirements for details: https://github.com/KangLin/RabbitRemoteControl/issues/7 

### Development
#### Dependent
##### Tools
- [x] [Qt](qt.io)
- [x] c compiler
  + gcc
  + MSVC
- [cmake](https://cmake.org/)

##### Dependent libraries
- [x] [MUST] RabbitCommon：https://github.com/KangLin/RabbitCommon
- [x] VNC
  + [x] [Optional] libvncserver: https://github.com/LibVNC/libvncserver
  + [x] [Optional] TigerVnc: https://github.com/KangLin/tigervnc
- [x] [Optional] FreeRDP: https://github.com/FreeRDP/FreeRDP
- [x] [Optional] [SSH]
  + [x] LIBSSH: https://www.libssh.org
- [x] [Optional] qtermwidget: https://github.com/lxqt/qtermwidget

### [License Agreement](License.md "License.md")

Please abide by this agreement and the license agreement of [dependent library](#Dependent-libraries),
and thank the author of [dependent library](#Dependent-libraries).
