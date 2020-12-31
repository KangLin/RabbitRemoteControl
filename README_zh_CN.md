# 玉兔远程控制

作者: 康林 (kl222@126.com)

-----------------------------------------------------------------------

- 语言
  - [<img src="Resource/Image/English.png" alt="英语" title="英语" width="16" height="16" />英语](README.md)
- 编译状态
  - [![编译状态](https://travis-ci.org/KangLin/RabbitRemoteControl.svg?branch=master)](https://travis-ci.org/KangLin/RabbitRemoteControl)
  - [![编译状态](https://ci.appveyor.com/api/projects/status/jai7jf3xr2vb44q8?svg=true)](https://ci.appveyor.com/project/KangLin/rabbitremotecontrol)
  - [![编译状态](https://github.com/KangLin/RabbitRemoteControl/workflows/CMake/badge.svg)](https://github.com/KangLin/RabbitRemoteControl/workflows/CMake/badge.svg)
- [修改日志](ChangeLog.md)

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

本人没有设备所以没有做编译与测试。请有设备的朋友自行测试。

如果你想为本人捐赠开发设备，请联系：(QQ：16614119)

#### 支持协议
- [x] VNC(虚拟网络控制台): [RFB](https://github.com/rfbproto/rfbproto)
- [x] 远程桌面协议: [RDP](https://github.com/FreeRDP/FreeRDP/wiki/Reference-Documentation)
- [ ] [Spice](https://www.spice-space.org/)
- [ ] [SSH]()
- [ ] [TELNET]()
- [ ] 自定义的 P2P 协议

#### 依赖库
- [x] [可选] RFB
  + [x] [可选] [libvncserver](https://github.com/LibVNC/libvncserver)
  + [x] [可选] [TigerVnc](https://github.com/KangLin/tigervnc)
- [x] [可选] [FreeRDP](https://github.com/FreeRDP/FreeRDP)
- [ ] [SSH]
  + [ ] [LIBSSH](https://www.libssh.org)

##### 参考
- [ ] [qtermwidget](https://github.com/lxqt/qtermwidget)

### 捐赠
本软件如果对你有用，或者你喜欢它，请你捐赠，支持作者。谢谢！
[![捐赠](https://github.com/KangLin/RabbitCommon/raw/master/Src/Resource/image/Contribute.png "捐赠")](https://github.com/KangLin/RabbitCommon/raw/master/Src/Resource/image/Contribute.png "捐赠") 

如果不能显示，请打开：
https://gitee.com/kl222/RabbitCommon/raw/master/Src/Resource/image/Contribute.png

[![捐赠](https://gitee.com/kl222/RabbitCommon/raw/master/Src/Resource/image/Contribute.png "捐赠")](https://gitee.com/kl222/RabbitCommon/raw/master/Src/Resource/image/Contribute.png "捐赠") 

### 贡献

- [讨论](https://github.com/KangLin/RabbitRemoteControl/discussions)
- [问题](https://github.com/KangLin/RabbitRemoteControl/issues)
- [贡献者](https://github.com/KangLin/RabbitRemoteControl/graphs/contributors)

### [许可协议](License.md "License.md")
请遵守本协议和[依赖库](#依赖库)的许可协议，并感谢[依赖库](#依赖库)的作者。

