# <img src="App/Client/Resource/Image/app.png" width="48" height="48" />[Rabbit Remote Control](https://github.com/KangLin/RabbitRemoteControl)

Author: Kang Lin <kl222@126.com>

-----------------------------------------------------------------------

[:cn: 中文](README_zh_CN.md)

-----------------------------------------------------------------------

- [Introduction](#Introduction)
  + [Screenshot](#Screenshot)
    - [Multiple style](#Multiple-style)
  + [Project repository position](#Project-repository-position)
- [Functions](#Functions)
  + [Supported operating system](#Supported-operating-system)
  + [Supported multiple languages](#Supported-multiple-languages)
  + [Supported protocols](#Supported-protocols)
    - [Remote control protocols](#Remote-control-protocols)
    - [Network protocols](#Network-protocols)
  + [Function list](#Function-list)
  + [Plugins](#Plugins)
  + [Full Documentation](#Full-Documentation)
- [Status](#Status)
- [Sponsorship and Donations](#Sponsorship-and-Donations)
- [Contribution](#Contribution)
- [Road map](#Road-map)
- [Download](#Download)
- [Change log](ChangeLog.md)
- [Development](#Development)
  + [Compilation](#Compilation)
    - [Compiling for Linux](docs/Compile/Linux.md)
    - [Compiling for Windows](docs/Compile/Windows.md)
- [License Agreement](#License-Agreement)

-----------------------------------------------------------------------

## Introduction

**Rabbit Remote Control** is a open-source, cross-platform, multi-protocol remote control software.

**Allows you to use any device and system in anywhere and remotely manage any device and system in any way**. 

It include remote desktop, remote terminal, player etc remote control functions.

### Screenshot

|Main window|Recent list menu|Connect|Work|
|--|--|--|--|
|![Main window](docs/Image/Screenshot/MainWindows_en.png)|![Recent list menu](docs/Image/Screenshot/RecentMenu_en.png)|![Connect](docs/Image/Screenshot/Connect_en.png)|![Work](docs/Image/Screenshot/Work_en.png)|

#### Multiple style

|Default style|Black-Green style|Black style|White style|
|:-------:|:------:|:-------:|:------:|
|![Default style](docs/Image/Screenshot/DefaultStyle.png)|![Black-Green style](docs/Image/Screenshot/Green.png)|![Black](docs/Image/Screenshot/black.png)|![White](docs/Image/Screenshot/white.png)|

### Video

https://github.com/user-attachments/assets/6924ec88-5f8c-43e8-8c62-0ed82f73ee83

### Project repository position

- Main repository: https://github.com/KangLin/RabbitRemoteControl
- Mirror repository:
  - sourceforge: https://sourceforge.net/projects/rabbitremotecontrol/
  - gitlab: https://gitlab.com/kl222/RabbitRemoteControl
  - launchpad: https://launchpad.net/rabbitremotecontrol
  - gitee: https://gitee.com/kl222/RabbitRemoteControl
  - bitbucket: https://bitbucket.org/kl222/rabbitremotecontrol

## Functions

### Supported operating system

- [x] Windows 7 SP1 and later
- [x] Unix/Linux
- [x] [Android](https://doc.qt.io/qt-6/android.html)
- [x] mac os

See: [Qt5](https://doc.qt.io/qt-5/supported-platforms.html),
[Qt6](https://doc.qt.io/qt-6/supported-platforms.html)

I have no the mac devices so I did not compile and test. Please test by yourself if you have devices.
Interested and capable friends are welcome to participate in the development of the project.

If you want to donate the developed devices for me, please contact: (email: kl222@126.com)

- Remote desktop

|        |Windows|Unix/linux|Android|MAC|
|--------|:-----:|:--------:|:-----:|:-:|
|vnc     |✓      |✓         |✓      |✓  |
|RDP     |✓      |✓         |……     |✓  |
|Spice   |       |          |       |   |
|Scrcpy  |       |          |       |   |

- Terminal

|        |Windows|Unix/linux|Android|MAC|
|--------|-------|----------|-------|---|
|Terminal|       |✓         |       |   |
|SSH     |       |✓         |       |   |
|TELNET  |       |✓         |       |   |

- File Transfer

|        |Windows|Unix/linux|Android|MAC|
|--------|-------|----------|-------|---|
|FTP     |       |          |       |   |
|SFTP    |       |          |       |   |

- Tool

|           |Windows|Unix/linux|Android|MAC|
|-----------|-------|----------|-------|---|
|Player     |✓      |✓         |✓      |✓  |
|Wake on lan|✓      |✓         |✓      |✓  |
|Screenshot |✓      |✓         |❌      |✓  |

Legend:  
 ✓ : completed; … : working; ⭕ : not completed; Empty: not started; ❌: not supported

### Supported multiple languages

- [x] English
- [x] Chinese
  - [x] Simplified Chinese
  - [x] Traditional Chinese

### Supported protocols

#### Remote control protocols

- [x] RFB(Remote frame buffer) is also called VNC(Virtual Network Computing): [https://github.com/rfbproto/rfbproto](https://github.com/rfbproto/rfbproto)
- [x] RDP(Remote desktop protocol): [https://github.com/FreeRDP/FreeRDP/wiki/Reference-Documentation](https://github.com/FreeRDP/FreeRDP/wiki/Reference-Documentation)
- [ ] Spice: [https://www.spice-space.org/](https://www.spice-space.org/)
- [x] SSH: [https://www.rfc-editor.org/info/rfc4251](https://www.rfc-editor.org/info/rfc4251)
  - [x] [libssh](https://api.libssh.org/stable/index.html)
- [x] TELNET: [https://www.rfc-editor.org/info/rfc318](https://www.rfc-editor.org/info/rfc318)
- [x] Terminal
- [ ] scrcpy: [https://github.com/Genymobile/scrcpy](https://github.com/Genymobile/scrcpy)
- [ ] FTP(File Transfer Protocol): [https://www.rfc-editor.org/info/rfc959](https://www.rfc-editor.org/info/rfc959)

#### Network protocols

- [x] Peer-to-Peer (P2P) protocol
  + ICE (Interactive Connectivity Establishment): [RFC 8445](https://datatracker.ietf.org/doc/rfc8445/) 
  + STUN: [RFC 5389](https://datatracker.ietf.org/doc/rfc5389/)
  + TURN(Traversal Using Relays around NAT): [RFC 5766](https://datatracker.ietf.org/doc/rfc5766/)
- [x] Proxy
  + [x] socks
  + [ ] http
  + [x] Peer-to-Peer (P2P) proxy protocol: [https://github.com/KangLin/RabbitRemoteControl/issues/7](https://github.com/KangLin/RabbitRemoteControl/issues/7)

#### Function list

|        |Server|Client|Desktop  |Clipboard|File|Audio|Redirection|P2P|Proxy|Terminal|
|:-----:|:---:|:---:|:----:|:--:|:-----:|:------:|:-----:|:-:|:--:|:-:|
|  RDP  |  ✓  |  ✓  |  ✓   |✓   | ……    |  ✓     | ……    |   |    | ❌ |
|  RFB  | ……  |  ✓  |  ✓   |✓   |       | ❌     | ❌    |✓  |  ✓ |   |
|Spice  |     |     |      |    |       |        | ❌    |   |    |   |
|Scrcpy |     |     |      |    |       |        |  ❌   |   |    |   |
|Terminal|     | ⭕  | ❌  |✓   | ❌    | ❌    | ❌    |    |   |    |
|TELNET |     | ⭕  | ❌  |✓   | ❌    | ❌    |  ❌    |   |   |    |
|  SSH  |     | ⭕  | ❌  |✓   |       | ❌     | ❌    |   |    |   |
|  FTP  |     |     | ❌  |    |       | ❌     |  ❌   |   |   |    |
| SFTP  |     |     | ❌  |    |       | ❌     |  ❌   |   |   |    |

Legend:  
 ✓ : completed; … : working; ⭕ : not completed; Empty: not started; ❌: not supported

|             | Proxy | P2P  |
|-------------|-------|------|
|RabbitVNC    |✓      | ✓    |
|TigerVNC     |✓      | ✓    |
|LibVNC       |       |      |
|FreeRDP      |       |      |

Legend:  
 ✓ : completed; … : working; ⭕ : not completed; Empty: not started; ❌: not supported

#### Plugins

- [FreeRDP](Plugins/FreeRDP/README.md)
- VNC
  - [RabbitVNC](Plugins/RabbitVNC/README.md)
  - [TigerVnc](Plugins/TigerVnc/README.md)
  - [LibVNCServer](Plugins/LibVNCServer/README.md)
- [Wake on lan](Plugins/WakeOnLan/README.md)
- [Player](Plugins/Player/README.md)

### Full Documentation

- Development documentation online
  - [sourceforge](https://rabbitremotecontrol.sourceforge.io/v0.0.32/English/html/index.html)
  - [github.io](https://kanglin.github.io/RabbitRemoteControl/English/html/index.html)
- [Compilation documentation](#Compilation)

## Status

[![Documentation](https://img.shields.io/badge/documentation-online-blue)](https://kanglin.github.io/RabbitRemoteControl/English/html/index.html)

[![GitHub stars](https://img.shields.io/github/stars/KangLin/RabbitRemoteControl?label=Github%20stars)](https://star-history.com/#KangLin/RabbitRemoteControl&Date)
[![Gitee stars](https://gitee.com/kl222/RabbitRemoteControl/badge/star.svg?theme=dark)](https://gitee.com/kl222/RabbitRemoteControl/stargazers)

[![Latest Release](https://img.shields.io/github/release/KangLin/RabbitRemoteControl.svg?label=Github%20latest%20Release)](https://github.com/KangLin/RabbitRemoteControl/releases/latest)
[![Release](https://img.shields.io/github/release-pre/KangLin/RabbitRemoteControl?label=Github%20Release)](https://github.com/KangLin/RabbitRemoteControl/releases)
[![rabbitremotecontrol](https://snapcraft.io/rabbitremotecontrol/badge.svg)](https://snapcraft.io/rabbitremotecontrol)
[![rabbitremotecontrol](https://snapcraft.io/rabbitremotecontrol/trending.svg?name=0)](https://snapcraft.io/rabbitremotecontrol)

[![Downloads from github](https://img.shields.io/github/downloads/KangLin/RabbitRemoteControl/total?label=Github%20downloads)](http://gra.caldis.me/?user=KangLin&repo=RabbitRemoteControl)
[![Downloads from sourceforge](https://img.shields.io/sourceforge/dt/rabbitremotecontrol.svg?label=Sourceforge%20downloads)](https://sourceforge.net/projects/rabbitremotecontrol/files/latest/download)

[![Gitter](https://badges.gitter.im/RabbitRemoteControl/community.svg)](https://gitter.im/RabbitRemoteControl/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)
[![GitHub issues](https://img.shields.io/github/issues/KangLin/RabbitRemoteControl)](https://github.com/KangLin/RabbitRemoteControl/issues)
[![GitHub forks](https://img.shields.io/github/forks/KangLin/RabbitRemoteControl)](https://github.com/KangLin/RabbitRemoteControl/)
[![Commit Activity](https://img.shields.io/github/commit-activity/m/KangLin/RabbitRemoteControl)](https://github.com/KangLin/RabbitRemoteControl/pulse)
[![Last Commit](https://img.shields.io/github/last-commit/KangLin/RabbitRemoteControl)](https://github.com/KangLin/RabbitRemoteControl/pulse)
[![Contributors](https://img.shields.io/github/contributors/KangLin/RabbitRemoteControl.svg?label=Contributors)](https://github.com/KangLin/RabbitRemoteControl/graphs/contributors)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-yellow.svg)](https://github.com/KangLin/RabbitRemoteControl/pulls)
[![Open PRs](https://img.shields.io/github/issues-pr/KangLin/RabbitRemoteControl)](https://github.com/KangLin/RabbitRemoteControl/pulls)
[![Closed PRs](https://img.shields.io/github/issues-pr-closed/KangLin/RabbitRemoteControl)](https://github.com/KangLin/RabbitRemoteControl/pulls?q=is%3Apr+is%3Aclosed)

[![C++](https://img.shields.io/badge/language-c++-red.svg)](https://en.cppreference.com/)
![Language](https://img.shields.io/github/languages/top/kanglin/RabbitRemoteControl)
[![Qt5](https://img.shields.io/badge/Qt-5-brightgreen)](https://doc.qt.io/qt-5/)
[![Qt6](https://img.shields.io/badge/Qt-6-brightgreen)](https://doc.qt.io/qt-6/)
[![platform](https://img.shields.io/badge/platform-windows%20|%20linux%20|%20android%20|%20macos-blue.svg)](https://github.com/KangLin/RabbitRemoteControl)

### Build status

| master|  develop |
|:-----:|:--------:|
[![build](https://github.com/KangLin/RabbitRemoteControl/actions/workflows/build.yml/badge.svg)](https://github.com/KangLin/RabbitRemoteControl/actions/workflows/build.yml)|[![build](https://github.com/KangLin/RabbitRemoteControl/actions/workflows/build.yml/badge.svg?branch=develop)](https://github.com/KangLin/RabbitRemoteControl/actions/workflows/build.yml)|

## Sponsorship and Donations

If this software is useful to you, or you like it, please donate and support the author. Thank you!

[![donation](https://github.com/KangLin/RabbitCommon/raw/master/Src/Resource/image/Contribute_en.png "donation")](https://github.com/KangLin/RabbitCommon/raw/master/Src/Resource/image/Contribute_en.png "donation") 

If it cannot be displayed, please open:
- https://gitlab.com/kl222/RabbitCommon/-/raw/master/Src/Resource/image/Contribute_en.png
- https://sourceforge.net/p/rabbitcommon/code/ci/master/tree/Src/Resource/image/Contribute_en.png

Contact me:
- <kl222@126.com>
- [jami](https://jami.net): eddacfd6c7b70b258e511e812ffab7d3490396fa

## Contribution

- [Mailing list](https://sourceforge.net/p/rabbitremotecontrol/mailman/)
  - Developer <rabbitremotecontrol-developer@lists.sourceforge.net>
  - Discuss <rabbitremotecontrol-discuss@lists.sourceforge.net>
- [Issues](https://github.com/KangLin/RabbitRemoteControl/issues)
- [![Join the chat at https://gitter.im/RabbitRemoteControl/community](https://badges.gitter.im/RabbitRemoteControl/community.svg)](https://gitter.im/RabbitRemoteControl/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge) 
- [Discussions](https://github.com/KangLin/RabbitRemoteControl/discussions)
- [Contributors](https://github.com/KangLin/RabbitRemoteControl/graphs/contributors):
  Thank [Contributors](https://github.com/KangLin/RabbitRemoteControl/graphs/contributors)

If you're wondering how could you help, fear not, there are plenty of ways:

Some of them are:

* [Donation](#Donation) - Provide funds and devices for this project :smile:
  + The project is need server with public IP
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
* Be finished [unsupported protocols](#Supported-protocols)

## Road map

- [x] Support common existing remote control protocols
  - [x] Client
    - [x] Client frame
      - [x] Remote desktop
      - [x] Terminal（Linux implemented，but it is has a same BUG）
    - [x] Specific protocol support, see: [Supported protocol](#Supported-protocols)
    - [x] Peer-to-Peer (P2P) remote control protocol, see requirements for details: https://github.com/KangLin/RabbitRemoteControl/issues/7
  - [x] Server
    - [x] Server framework
    - [x] Specific protocol support, see: [Supported protocol](#Supported-protocols)
    - [x] Peer-to-Peer (P2P) remote control protocol, see requirements for details: https://github.com/KangLin/RabbitRemoteControl/issues/7
- [x] Peer-to-Peer (P2P)remote control protocol, see requirements for details: https://github.com/KangLin/RabbitRemoteControl/issues/7
- [ ] File Transfer Protocol(FTP)

|            |Server|Client|Desktop|Clipboard|File|Audio|Redirection|P2P|Proxy|Terminal|
|:----------:|:----:|:-----:|:------:|:------:|:--:|:---:|:--------:|:--:|:--:|:--:|
|RabbitVNC   | ✓    |✓      |✓       |    ✓   |    |  ❌ |    ❌    |✓   |  ✓ |    |
|TigerVNC    | ✓    |✓      |✓       |    ✓   |    |  ❌  |   ❌    |✓   |  ✓ |    |
|LibVNCServer| ✓    |✓      |✓       |    ✓   |    |  ❌  |   ❌    |    |    |    |
|RDP         | ✓    |✓      |✓       |    ✓   | …… |  ✓   |  ……     |    |    | ❌ |
|Spice       |      |       |        |        |    |      |    ❌   |    |    |    |
|Scrcpy      |      |       |        |        |    |      |    ❌   |    |    |    |
|Terminal    |      |⭕    |  ❌     |✓       |    |  ❌  |    ❌  |    |    |    |
|SSH         |      |⭕     |  ❌     |✓       |    |  ❌  |  ❌   |    |    |    |
|TELNET      |      |⭕     |  ❌     |✓       |❌   |  ❌|   ❌   |    |    |    |
|FTP         |      |       |  ❌     |        |    |  ❌  |  ❌   |    |    |    |
|SFTP        |      |       |  ❌     |        |    |  ❌  |  ❌   |    |    |    |

Legend:  
 ✓ : completed; … : working; ⭕ : not completed; Empty: not started; ❌: not supported

- The development of common remote control protocol clients 
and some servers has been completed.
It has preliminary practical functions,
and needs to be tried by small partners, find bugs,
and put forward suggestions that can be more convenient to use in practical applications.

- For terminal remote control, because qtermwidget only supports LINUX/UNIX, it needs to support other platforms.

- Peer-to-Peer (P2P) remote control is finished.
which is also a feature of this project.
See requirements for details: https://github.com/KangLin/RabbitRemoteControl/issues/7 

## Download

- [Change log](ChangeLog.md)
- Semantic Versioning: https://semver.org/

+ Latest Release: 
  - [![Latest Release](https://img.shields.io/github/release/KangLin/RabbitRemoteControl.svg?label=Github%20latest%20Release)](https://github.com/KangLin/RabbitRemoteControl/releases/latest)
  - [![Release](https://img.shields.io/github/release-pre/KangLin/RabbitRemoteControl?label=Github%20Release)](https://github.com/KangLin/RabbitRemoteControl/releases)
  - [![Download RabbitRemoteControl](https://a.fsdn.com/con/app/sf-download-button)](https://sourceforge.net/projects/rabbitremotecontrol/files/latest/download)
  - [![rabbitremotecontrol](https://snapcraft.io/rabbitremotecontrol/badge.svg)](https://snapcraft.io/rabbitremotecontrol)
    [![rabbitremotecontrol](https://snapcraft.io/rabbitremotecontrol/trending.svg?name=0)](https://snapcraft.io/rabbitremotecontrol)
  
+ Current version: v0.0.32
  - Windows: Supported WINDOWS 7 and later version
    + Win64
      - Install package: RabbitRemoteControl_v0.0.32_win64_msvc2022_64_qt6.8.1_Setup.exe
        - [Download from Github](https://github.com/KangLin/RabbitRemoteControl/releases/download/v0.0.32/RabbitRemoteControl_v0.0.32_win64_msvc2022_64_qt6.8.1_Setup.exe)
        - [Download from sourceforge](https://sourceforge.net/projects/rabbitremotecontrol/files/v0.0.32/RabbitRemoteControl_v0.0.32_win64_msvc2022_64_qt6.8.1_Setup.exe/download)
      - MD5SUM file: RabbitRemoteControl_v0.0.32_win64_msvc2022_64_qt6.8.1_Setup.exe.md5sum
        - [Download from Github](https://github.com/KangLin/RabbitRemoteControl/releases/download/v0.0.32/RabbitRemoteControl_v0.0.32_win64_msvc2022_64_qt6.8.1_Setup.exe.md5sum)
        - [Download from sourceforge](https://sourceforge.net/projects/rabbitremotecontrol/files/v0.0.32/RabbitRemoteControl_v0.0.32_win64_msvc2022_64_qt6.8.1_Setup.exe.md5sum/download)
    + Win32
      - Install package: RabbitRemoteControl_v0.0.32_win32_msvc2017_qt5.12.12_Setup.exe
        - [Download from Github](https://github.com/KangLin/RabbitRemoteControl/releases/download/v0.0.32/RabbitRemoteControl_v0.0.32_win32_msvc2017_qt5.12.12_Setup.exe)
        - [Download from sourceforge](https://sourceforge.net/projects/rabbitremotecontrol/files/v0.0.32/RabbitRemoteControl_v0.0.32_win32_msvc2017_qt5.12.12_Setup.exe/download)
      - MD5SUM file: RabbitRemoteControl_v0.0.32_win32_msvc2017_qt5.12.12_Setup.exe.md5sum
        - [Download from Github](https://github.com/KangLin/RabbitRemoteControl/releases/download/v0.0.32/RabbitRemoteControl_v0.0.32_win32_msvc2017_qt5.12.12_Setup.exe.md5sum)
        - [Download from sourceforge](https://sourceforge.net/projects/rabbitremotecontrol/files/v0.0.32/RabbitRemoteControl_v0.0.32_win32_msvc2017_qt5.12.12_Setup.exe.md5sum/download)
  - Linux
    - Ubuntu, Debian:
      + rabbitremotecontrol_0.0.32_amd64.deb
      
            sudo apt install ./rabbitremotecontrol_0.0.32_amd64.deb
          
            # If you're on an older ubuntu distribution, you will need to run this instead:
            # sudo dpkg -i rabbitremotecontrol_0.0.32_amd64.deb
            # sudo apt-get install -f # Install dependencies

        - [Download from github](https://github.com/KangLin/RabbitRemoteControl/releases/download/v0.0.32/rabbitremotecontrol_0.0.32_amd64.deb)
        - [Download from sourceforge](https://sourceforge.net/projects/rabbitremotecontrol/files/v0.0.32/rabbitremotecontrol_0.0.32_amd64.deb/download)
      + MD5SUM file: rabbitremotecontrol_0.0.32_amd64.deb.md5sum
        - [Download from github](https://github.com/KangLin/RabbitRemoteControl/releases/download/v0.0.32/rabbitremotecontrol_0.0.32_amd64.deb.md5sum)
        - [Download from sourceforge](https://sourceforge.net/projects/rabbitremotecontrol/files/v0.0.32/rabbitremotecontrol_0.0.32_amd64.deb.md5sum/download)
    - Snap:
      [![rabbitremotecontrol](https://snapcraft.io/rabbitremotecontrol/badge.svg)](https://snapcraft.io/rabbitremotecontrol)
      [![rabbitremotecontrol](https://snapcraft.io/rabbitremotecontrol/trending.svg?name=0)](https://snapcraft.io/rabbitremotecontrol)
    - AppImage:
      - RabbitRemoteControl_v0.0.32_x86_64.AppImage
        - [Download from github](https://github.com/KangLin/RabbitRemoteControl/releases/download/v0.0.32/RabbitRemoteControl_v0.0.32_x86_64.AppImage)
        - [Download from sourceforge](https://sourceforge.net/projects/rabbitremotecontrol/files/v0.0.32/RabbitRemoteControl_v0.0.32_x86_64.AppImage/download)

              chmod u+x RabbitRemoteControl_v0.0.32_x86_64.AppImage
              ./RabbitRemoteControl_v0.0.32_x86_64.AppImage

      - md5sum file: RabbitRemoteControl_v0.0.32_x86_64.AppImage.md5sum
        - [Download from github](https://github.com/KangLin/RabbitRemoteControl/releases/download/v0.0.32/RabbitRemoteControl_v0.0.32_x86_64.AppImage.md5sum)
        - [Download from sourceforge](https://sourceforge.net/projects/rabbitremotecontrol/files/v0.0.32/RabbitRemoteControl_v0.0.32_x86_64.AppImage.md5sum/download)
  - Other:
    - [Download from github](https://github.com/KangLin/RabbitRemoteControl/releases/tag/v0.0.32)
    - [Download from sourceforge](https://sourceforge.net/projects/rabbitremotecontrol/files/v0.0.32/)

## [Development](docs/Doxygen/Develop.md)

### Development documentation online
- [sourceforge](https://rabbitremotecontrol.sourceforge.io/v0.0.32/English/html/index.html)
- [github.io](https://kanglin.github.io/RabbitRemoteControl/English/html/index.html)

### Compilation
- [Compiling for Linux](docs/Compile/Linux.md)
- [Compiling for Windows](docs/Compile/Windows.md)

## [License Agreement](License.md "License.md")

Please abide by the [License Agreement](License.md "License.md") of this project
and the license agreement of the [dependent library](#Dependent-libraries),
and thank the authors of [dependent library](#Dependent-libraries) and [tools](#Tools).
