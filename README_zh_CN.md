# 玉兔远程控制

作者: 康林 (kl222@126.com)

-----------------------------------------------------------------------

- [<img src="Resource/Image/English.png" alt="英语" title="英语" width="16" height="16" />英语](README.md)
- [![编译状态](https://travis-ci.org/KangLin/RabbitRemoteControl.svg?branch=master)](https://travis-ci.org/KangLin/RabbitRemoteControl)
- [![Build status](https://ci.appveyor.com/api/projects/status/jai7jf3xr2vb44q8?svg=true)](https://ci.appveyor.com/project/KangLin/rabbitremotecontrol)

-----------------------------------------------------------------------
### 本软件的由来
做为一个软件工程师，在日常工作中，经常需要使用远程控制软件。最初在局域网中的 WINDOWS 操作系统
使用 WINDOWS 自带的远程桌面。后面要开发 linxu ，就只能使用 VNC。在LINUX系统上的开源远程控制
方案比较多，但是 WINDOWS 下的就很少有开源方案。后来，需要在公网远程控制内网的主机。原来的在局域网
的方案全部失效。选用了 TEAMVIWER。最近，TEAMVIWER 不仅需要收集隐私信息，还要付费。因为我没有付费，
限制了我使用。没有替代的方案。所以决定自己写一个开源的远程控制软件。**能够在任何地方任意系统上通过任意网络访问任意系统**。  
本软件才刚开始开发。欢迎有兴趣的朋友参与。

### 简介
玉兔远程控制是一个跨平台，支持多协议的远程控制软件。**能够在任意系统上通过任意网络访问任意系统**。

#### 支持操作系统 (参见：[https://doc.qt.io/qt-5/supported-platforms.html](https://doc.qt.io/qt-5/supported-platforms.html))
- [x] Windows
- [x] Unix/Linux
- [x] Android
- [ ] mac os
- [ ] IPHONE
- [ ] WINCE

#### 支持协议
- [x] VNC(虚拟网络控制台): [RFB](https://github.com/rfbproto/rfbproto)
- [x] 远程桌面协议: [RDP](https://github.com/FreeRDP/FreeRDP/wiki/Reference-Documentation)
- [ ] [Spice](https://www.spice-space.org/)
- [ ] [SSH]()
- [ ] [TELNET]()
- [ ] 自定义的 P2P 协议

#### 依赖库
- [x] [可选] RFB
  + [ ] [可选] [libvncserver](https://github.com/LibVNC/libvncserver)
  + [x] [可选] [TigerVnc](https://github.com/KangLin/tigervnc)
- [x] [可选] [FreeRDP](https://github.com/FreeRDP/FreeRDP)
- [ ] [SSH]
  + [ ] [LIBSSH](https://www.libssh.org)

##### 参考
- [ ] [qtermwidget](https://github.com/lxqt/qtermwidget)

### 捐赠
本软件如果对你有用，或者你喜欢它，请你捐赠，支持作者。谢谢！
 
[![捐赠](https://gitee.com/kl222/RabbitCommon/raw/master/Src/Resource/image/Contribute.png "捐赠")](https://gitee.com/kl222/RabbitCommon/raw/master/Src/Resource/image/Contribute.png "捐赠") 

### 贡献

- [讨论](https://github.com/KangLin/RabbitRemoteControl/discussions)
- [问题](https://github.com/KangLin/RabbitRemoteControl/issues)
- [贡献者](https://github.com/KangLin/RabbitRemoteControl/graphs/contributors)

### [许可协议](License.md "License.md")
请遵守本协议和[依赖库](#依赖库)的许可协议，并感谢[依赖库](#依赖库)的作者。

