# <img src="App/Resource/Image/App.png" width="48" height="48" alt="玉兔远程控制" title="玉兔远程控制"/>[玉兔远程控制](https://github.com/KangLin/RabbitRemoteControl)

作者: 康林 (kl222@126.com)

-----------------------------------------------------------------------

- 项目位置： https://github.com/KangLin/RabbitRemoteControl
- [项目主页](https://kanglin.github.io/RabbitRemoteControl/)
- 语言
  - [<img src="Resource/Image/English.png" alt="英语" title="英语" width="16" height="16" />英语](README.md)
- 编译状态
  - [![编译状态](https://github.com/KangLin/RabbitRemoteControl/workflows/CMake/badge.svg)](https://github.com/KangLin/RabbitRemoteControl/workflows/CMake/badge.svg)
- [修改日志](ChangeLog_zh_CN.md)

-----------------------------------------------------------------------
### 简介
玉兔远程控制是一个跨平台，支持多协议的远程控制软件。

能让你在**任何地方**使用**任何设备和系统**通过**任意方式**远程管理**任意设备和系统**。

#### 支持操作系统 (参见：[https://doc.qt.io/qt-5/supported-platforms.html](https://doc.qt.io/qt-5/supported-platforms.html))
- [x] Windows
- [x] Unix/Linux
- [x] Android
- [ ] mac os
- [ ] IPHONE
- [ ] WINCE

本人没有设备，所以没有做编译与测试。请有设备的朋友自行测试。欢迎有兴趣，有能力的朋友参与项目的开发。

如果你想为本人捐赠开发设备，请联系：(QQ：16614119)

#### 支持协议
- [x] VNC(虚拟网络控制台): [RFB](https://github.com/rfbproto/rfbproto)
- [x] 远程桌面协议: [RDP](https://github.com/FreeRDP/FreeRDP/wiki/Reference-Documentation)
- [ ] [Spice](https://www.spice-space.org/)
- [ ] [SSH]()
- [x] [TELNET]()
- [ ] 自定义的 P2P 协议
- [x] 终端

#### 依赖
##### 工具
- [x] [Qt](qt.io)
- [x] c compiler
  + gcc
  + MSVC
- [cmake](https://cmake.org/)

##### 依赖库
- [x] [可选] RFB
  + [x] [可选] [libvncserver](https://github.com/LibVNC/libvncserver)
  + [x] [可选] [TigerVnc](https://github.com/KangLin/tigervnc)
- [x] [可选] [FreeRDP](https://github.com/FreeRDP/FreeRDP)
- [ ] [SSH]
  + [ ] [LIBSSH](https://www.libssh.org)
- [x] [qtermwidget](https://github.com/lxqt/qtermwidget)

### 捐赠
本软件如果对你有用，或者你喜欢它，请你捐赠，支持作者。谢谢！

[![捐赠](https://github.com/KangLin/RabbitCommon/raw/master/Src/Resource/image/Contribute.png "捐赠")](https://github.com/KangLin/RabbitCommon/raw/master/Src/Resource/image/Contribute.png "捐赠") 

如果不能显示，请打开：
https://gitee.com/kl222/RabbitCommon/raw/master/Src/Resource/image/Contribute.png

### 贡献

- [讨论](https://github.com/KangLin/RabbitRemoteControl/discussions)
- [问题](https://github.com/KangLin/RabbitRemoteControl/issues)
- [贡献者](https://github.com/KangLin/RabbitRemoteControl/graphs/contributors)

如果您想知道如何提供帮助，不要害怕，有很多方法：

* [捐赠](#捐赠) - 为本项目提供资金与设备支持 :smile:
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
* 完成还未支持的协议

### [许可协议](License.md "License.md")
请遵守本协议和[依赖库](#依赖库)的许可协议，并感谢[依赖库](#依赖库)的作者。

