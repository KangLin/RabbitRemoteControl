
## Rabbit Remote Control Development Document

Author: Kang Lin kl222@126.com

### Project location

+ Main repository: https://github.com/KangLin/RabbitRemoteControl
+ Mirror repository:
  - gitee: https://gitee.com/kl222/RabbitRemoteControl
  - sourceforge: https://sourceforge.net/projects/rabbitremotecontrol
  - gitlab: https://gitlab.com/kl222/RabbitRemoteControl
  - launchpad: https://launchpad.net/rabbitremotecontrol
  - kde: https://invent.kde.org/kanglin/RabbitRemoteControl
  - bitbucket: https://bitbucket.org/kl222/rabbitremotecontrol
    
### Semantic Versioning:

- Semantic Versioning: [https://semver.org/](https://semver.org/)

### User manual

- [User manual](https://github.com/KangLin/RabbitRemoteControl/wiki/UserManual)

### Dependent

#### Tools
- [Qt](qt.io)
- C/C++ compiler
  + gcc/g++
  + MSVC
- [CMake](https://cmake.org/)
- Automake、 Autoconf、 Make
- Git

#### Dependent libraries
- [x] [MUST] RabbitCommon：https://github.com/KangLin/RabbitCommon
- [x] VNC
  + [x] [Optional] RabbitVNC: https://github.com/KangLin/RabbitVNC
  + [x] [Optional] LibVNCServer: https://github.com/LibVNC/libvncserver
  + [x] [Optional] TigerVNC: https://github.com/KangLin/tigervnc
- [x] [Optional] FreeRDP: https://github.com/FreeRDP/FreeRDP
- [x] [Optional] [SSH]
  + [x] LIBSSH: https://www.libssh.org
  + [ ] LIBSSH2: https://www.libssh2.org/ https://github.com/libssh2/libssh2
- [x] [Optional] QTermWidget: https://github.com/lxqt/qtermwidget
- [x] [Optional] libtelnet: https://github.com/seanmiddleditch/libtelnet
- [ ] [Optional] scrcpy: [https://github.com/Genymobile/scrcpy](https://github.com/Genymobile/scrcpy)
- [x] [Optional] libdatachannel: [https://github.com/paullouisageneau/libdatachannel](https://github.com/paullouisageneau/libdatachannel)
- [x] [OPTIONAL] QXmpp: https://github.com/qxmpp-project/qxmpp
- [x] [Optional] QtService: https://github.com/KangLin/qt-solutions/

### Source directory

```
Source root
        |- App            # Applications
        |   |- Client     # Client applications
        |   |- Service    # Service applications
        |- Channel        # Channel interface
        |- Client         # Client interface
        |    |- Terminal
        |- Service        # Service interface
        |- Plugins        # Plugins
```

### Compile

- [Linux](../Compile/Linux.md)
- [Windows](../Compile/Windows.md)

### Topics

[Topics](topics.html)

### Icon theme

- Icon Theme Specification: https://specifications.freedesktop.org/icon-theme/latest/
- Icon Naming Specification: https://specifications.freedesktop.org/icon-naming/
