# <img src="App/Viewer/Resource/Image/App.png" width="48" height="48" />[玉兔远程控制](https://github.com/KangLin/RabbitRemoteControl)

作者: 康林 <kl222@126.com>

-----------------------------------------------------------------------

[![Gitter](https://badges.gitter.im/RabbitRemoteControl/community.svg)](https://gitter.im/RabbitRemoteControl/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)
[![GitHub stars](https://img.shields.io/github/stars/KangLin/RabbitRemoteControl)](https://github.com/KangLin/RabbitRemoteControl/stars)
[![GitHub forks](https://img.shields.io/github/forks/KangLin/RabbitRemoteControl)](https://github.com/KangLin/RabbitRemoteControl/forks)
[![GitHub issues](https://img.shields.io/github/issues/KangLin/RabbitRemoteControl)](https://github.com/KangLin/RabbitRemoteControl/issues)
[![Current release](https://img.shields.io/github/release/KangLin/RabbitRemoteControl)](https://github.com/KangLin/RabbitRemoteControl/releases)
[![All releases downloads](https://img.shields.io/github/downloads/KangLin/RabbitRemoteControl/total)](https://github.com/KangLin/RabbitRemoteControl/releases)

-----------------------------------------------------------------------

[:us: English](README.md)

-----------------------------------------------------------------------

- [简介](#简介)
- [编译状态](#编译状态)
- [捐赠](#捐赠)
- [贡献](#贡献)
- [线路图](#线路图)
- [下载](#下载)
- [修改日志](ChangeLog_zh_CN.md)
- [开发](#开发)
  + [编译](#编译)
    - [Linux](docs/Compile/BuildLinux_zh_CN.md)
    - [Windows](docs/Compile/BuildWindows_zh_CN.md)
- [许可协议](#许可协议)

-----------------------------------------------------------------------
### 简介

玉兔远程控制是一个跨平台，支持多协议的远程控制软件。

能让你在**任何地方**使用**任何设备和系统**通过**任意方式**远程管理**任意设备和系统**。

#### 截屏

|主窗口|最近列表窗口|连接窗口|
|--|--|--|
|![主窗口](docs/Image/Screenslot/MainWindows.png)|![最近列表窗口](docs/Image/Screenslot/RecentMenu.png)|![连接窗口](docs/Image/Screenslot/Connect.png)|

#### 项目位置
- 项目主页: [https://kanglin.github.io/RabbitRemoteControl/](https://kanglin.github.io/RabbitRemoteControl/)
- 项目位置:
  + 主库：https://github.com/KangLin/RabbitRemoteControl
  + 中国镜像：https://gitee.com/kl222/RabbitRemoteControl

#### 支持操作系统 (参见：[https://doc.qt.io/qt-5/supported-platforms.html](https://doc.qt.io/qt-5/supported-platforms.html))
- [x] Windows
- [x] Unix/Linux
- [x] Android
- [ ] mac os
- [ ] IPHONE
- [ ] WINCE

本人没有设备，所以没有做编译与测试。请有设备的朋友自行测试。欢迎有兴趣，有能力的朋友参与项目的开发。

如果你想为本人捐赠开发设备，请联系：kl222@126.com

#### 支持协议
##### 远程控制协议
- [x] VNC（虚拟网络计算）: [RFB("remote framebuffer"): https://github.com/rfbproto/rfbproto](https://github.com/rfbproto/rfbproto)
- [x] 远程桌面协议: [RDP: https://github.com/FreeRDP/FreeRDP/wiki/Reference-Documentation](https://github.com/FreeRDP/FreeRDP/wiki/Reference-Documentation)
- [ ] [Spice: https://www.spice-space.org/](https://www.spice-space.org/)
- [x] 终端
- [x] [SSH]()
- [x] [TELNET: http://www.faqs.org/rfcs/rfc855.html](http://www.faqs.org/rfcs/rfc855.html)
- [ ] scrcpy: [https://github.com/Genymobile/scrcpy](https://github.com/Genymobile/scrcpy)

  
|        |Windows           |Unix/linux        |Android           |MAC               |IPHONE            |WINCE             |
|--------|------------------|------------------|------------------|------------------|------------------|------------------|
|vnc     |:heavy_check_mark:|:heavy_check_mark:|                  |                  |                  |                  |
|RDP     |:heavy_check_mark:|:heavy_check_mark:|                  |                  |                  |                  |
|Spice   |                  |                  |                  |                  |                  |                  |
|SSH     |                  |:heavy_check_mark:|                  |                  |                  |                  |
|Terminal|                  |:heavy_check_mark:|                  |                  |                  |                  |
|TELNET  |                  |:heavy_check_mark:|                  |                  |                  |                  |
|Scrcpy  |                  |                  |                  |                  |                  |                  |


##### 网络协议

- [x] 自定义的 P2P 协议
  + ICE
  + STUN
  + TURN
- [x] 代理
  + [x] socks
  + [ ] http
  + [x] 自定义的 P2P 代理协议： [https://github.com/KangLin/RabbitRemoteControl/issues/7](https://github.com/KangLin/RabbitRemoteControl/issues/7)

|             | 代理              | P2P              |
|-------------|------------------|------------------|
|TigerVnc     |:heavy_check_mark:|:heavy_check_mark:|
|LibVNC       |                  |                  |
|FreeRDP      |                  |                  |


### 编译状态

[![编译状态](https://ci.appveyor.com/api/projects/status/q6vsg0u0v4tavkg1?svg=true)](https://ci.appveyor.com/project/KangLin/rabbitremotecontrol)  

#### Github actions

|     | master 分支 | develop 分支 |
|:---:|:-----------:|:------------:|
|Linux|[![Linux 编译状态](https://github.com/kanglin/rabbitremotecontrol/actions/workflows/ubuntu.yml/badge.svg?branch=master)](https://github.com/kanglin/rabbitremotecontrol/actions)|[![Linux 编译状态](https://github.com/kanglin/rabbitremotecontrol/actions/workflows/ubuntu.yml/badge.svg?branch=develop)](https://github.com/kanglin/rabbitremotecontrol/actions)
|Windows|[![Windows 编译状态](https://github.com/kanglin/rabbitremotecontrol/actions/workflows/msvc.yml/badge.svg?branch=master)](https://github.com/kanglin/rabbitremotecontrol/actions)|[![Windows 编译状态](https://github.com/kanglin/rabbitremotecontrol/actions/workflows/msvc.yml/badge.svg?branch=develop)](https://github.com/kanglin/rabbitremotecontrol/actions)
|Android|[![Android 编译状态](https://github.com/kanglin/rabbitremotecontrol/actions/workflows/android.yml/badge.svg?branch=master)](https://github.com/kanglin/rabbitremotecontrol/actions)|[![Android 编译状态](https://github.com/kanglin/rabbitremotecontrol/actions/workflows/android.yml/badge.svg?branch=develop)](https://github.com/kanglin/rabbitremotecontrol/actions)


### 捐赠

本软件如果对你有用，或者你喜欢它，请你捐赠，支持作者。谢谢！

[![捐赠](https://gitee.com/kl222/RabbitCommon/raw/master/Src/Resource/image/Contribute.png "捐赠")](https://gitee.com/kl222/RabbitCommon/raw/master/Src/Resource/image/Contribute.png "捐赠") 

如果不能显示，请打开：
https://gitee.com/kl222/RabbitCommon/raw/master/Src/Resource/image/Contribute.png

### 贡献

- [问题](https://github.com/KangLin/RabbitRemoteControl/issues)
- [![加入聊天 https://gitter.im/RabbitRemoteControl/community](https://badges.gitter.im/RabbitRemoteControl/community.svg)](https://gitter.im/RabbitRemoteControl/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge) 
- [讨论](https://github.com/KangLin/RabbitRemoteControl/discussions)
- [贡献者](https://github.com/KangLin/RabbitRemoteControl/graphs/contributors): 感谢贡献者

如果您想知道如何提供帮助，不要害怕，有很多方法：

* [捐赠](#捐赠) - 为本项目提供资金与设备支持 :smile:
  - 本项目需要具有公网 IP 的服务器
* 向你周围的人传播有关[玉兔远程控件](https://github.com/KangLin/RabbitRemoteControl)的好话，使其更受欢迎 :smile:
* 有意见吗？ 请务必在[讨论区](https://github.com/KangLin/RabbitRemoteControl/discussions)中发声，说明你的需求，帮助我们改进。 :wink:
* 帮助解决 BUG 与功能[Issues](https://github.com/KangLin/RabbitRemoteControl/issues) 。请确保先阅读过[贡献](#贡献)中的内容。 :wink:
* 测试并报告 [BUG](https://github.com/KangLin/RabbitRemoteControl/issues)
* 翻译，它很容易 :smile:
* 审核和测试 pull requests – 你不需要编写代码 :wink:
* 帮助我完成分发到常用的操作系统
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
* 完成还未支持的[协议](支持协议)

### 线路图
- [x] 支持常见已有远程控制协议
  - [x] 客户端
    - [x] 客户端框架
      - [x] 远程桌面
      - [x] 控制终端（Linux下实现，但有BUG）
    - [x] 具体协议支持，详见：[支持协议](#支持协议)
    - [x] 自定义点对点远程控制协议，详见需求: https://github.com/KangLin/RabbitRemoteControl/issues/7
  - [x] 服务器
    - [x] 服务器框架
    - [x] 具体协议支持，详见：[支持协议](#支持协议)
    - [x] 自定义点对点远程控制协议，详见需求: https://github.com/KangLin/RabbitRemoteControl/issues/7
- [x] 自定义点对点远程控制协议，详见需求: https://github.com/KangLin/RabbitRemoteControl/issues/7

|            |       客户端      |      服务器端     |        P2P        |
|------------|------------------|------------------|------------------|
|TigerVNC    |:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|
|LibVNCServer|:heavy_check_mark:|:heavy_check_mark:|                  |
|RDP         |:heavy_check_mark:|                  |                  |
|Spice       |                  |                  |                  |
|SSH         |:heavy_check_mark:|                  |                  |
|Terminal    |:heavy_check_mark:|                  |                  |
|TELNET      |:heavy_check_mark:|                  |                  |
|Scrcpy      |                  |                  |                  |

当前已经完成常见远程控制协议客户端的开发，已初步具备实用功能，需要小伙伴们试用，发现BUG，提出能更方便在实际应用中使用的建议。
终端类远程控制，因为 qtermwidget 仅支持 LINUX/UNIX ，所以需要让其支持其它平台。

下一级段的工作是解决点对点远程控制，也是本项目的特点。详见需求: https://github.com/KangLin/RabbitRemoteControl/issues/7

### 下载

发行版本：[https://github.com/KangLin/RabbitRemoteControl/releases](https://github.com/KangLin/RabbitRemoteControl/releases)

- Windows: 支持 WINDOWS 7 及以后版本
  + 安装包：[RabbitRemoteControl-Setup-v0.0.8.exe](https://github.com/KangLin/RabbitRemoteControl/releases/download/v0.0.8/RabbitRemoteControl-Setup-v0.0.8.exe)
  + 绿色版本：[RabbitRemoteControl_v0.0.8.zip](https://github.com/KangLin/RabbitRemoteControl/releases/download/v0.0.8/RabbitRemoteControl_v0.0.8.zip)
- Ubuntu:
  + Ubuntu 20.04: [rabbitremotecontrol_0.0.8_amd64_Ubuntu-20.04.deb](https://github.com/KangLin/RabbitRemoteControl/releases/download/v0.0.8/rabbitremotecontrol_0.0.8_amd64_Ubuntu-20.04.deb)
  + Ubuntu 18.04: [rabbitremotecontrol_0.0.8_amd64_Ubuntu-18.04.deb](https://github.com/KangLin/RabbitRemoteControl/releases/download/v0.0.8/rabbitremotecontrol_0.0.8_amd64_Ubuntu-18.04.deb)

### 开发
#### 依赖
##### 工具

- [Qt](qt.io)
- C/C++ compiler
  + gcc/g++
  + MSVC
- [CMake](https://cmake.org/)
- Automake、 Autoconf、 Make
- Git

##### 依赖库

- [x] [必选] RabbitCommon：https://github.com/KangLin/RabbitCommon
- [x] [可选] RFB
  + [x] [可选] LibVNCServer: https://github.com/LibVNC/libvncserver
  + [x] [可选] TigerVNC: https://github.com/KangLin/tigervnc
- [x] [可选] FreeRDP: https://github.com/FreeRDP/FreeRDP
- [x] [可选] [SSH]
  + [x] LIBSSH: https://www.libssh.org
  + [ ] LIBSSH2: https://www.libssh2.org/ https://github.com/libssh2/libssh2
- [x] [可选] QTermWidget: https://github.com/lxqt/qtermwidget
- [x] [可选] libtelnet: https://github.com/seanmiddleditch/libtelnet
- [ ] [可选] scrcpy: [https://github.com/Genymobile/scrcpy](https://github.com/Genymobile/scrcpy)
- [x] [可选] libdatachannel: [https://github.com/paullouisageneau/libdatachannel](https://github.com/paullouisageneau/libdatachannel)
- [x] [可选] QXmpp: https://github.com/qxmpp-project/qxmpp
- [x] [可选] QtService: https://github.com/KangLin/qt-solutions/

#### 编译
- [Linux](docs/Compile/BuildLinux_zh_CN.md)
- [Windows](docs/Compile/BuildWindows_zh_CN.md)

### [许可协议](License.md "License.md")
请遵守本协议和[依赖库](#依赖库)的许可协议，并感谢[依赖库](#依赖库)和[工具](#工具)的作者。
