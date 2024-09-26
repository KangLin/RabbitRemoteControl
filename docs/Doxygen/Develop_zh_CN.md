
## 玉兔远程控制开发文档

作者：康 林 <kl222@126.com>

### 项目位置

+ 主库： https://github.com/KangLin/RabbitRemoteControl
+ 镜像：
  - 中国镜像： https://gitee.com/kl222/RabbitRemoteControl
  - sourceforge: https://sourceforge.net/projects/rabbitremotecontrol
  - gitlab: https://gitlab.com/kl222/RabbitRemoteControl
  - launchpad: https://launchpad.net/rabbitremotecontrol
  - kde: https://invent.kde.org/kanglin/RabbitRemoteControl
  - bitbucket: https://bitbucket.org/kl222/rabbitremotecontrol

### 版本号说明：

- 版本号说明： [https://semver.org/lang/zh-CN/](https://semver.org/lang/zh-CN/)

### 依赖

#### 工具

- [Qt](qt.io)
- C/C++ compiler
  + gcc/g++
  + MSVC
- [CMake](https://cmake.org/)
- Automake、 Autoconf、 Make
- Git

#### 依赖库

- [x] [必选] RabbitCommon：https://github.com/KangLin/RabbitCommon
- [x] [可选] RFB
  + [x] [可选] RabbitVNC: https://github.com/KangLin/RabbitVNC
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

### 目录结构

```
源码根目录
        |- App             # 应用程序
        |   |- Client      # 客户端应用程序　
        |   |- Service     # 服务端应用程序
        |- Channel         # 通道接口
        |- Client          # 客户端接口
        |    |- Terminal
        |- Service         # 服务端接口
        |- Plugins         # 插件
```

### 编译

- [Linux](../Compile/Linux_zh_CN.md)
- [Windows](../Compile/Windows_zh_CN.md)

### 模块

[Topics](topics.html)

### 图标主题

- Qt 主题: https://github.com/KangLin/Documents/blob/master/qt/theme.md
- 图标主题规范: https://specifications.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html
- 图标命名规范: https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
