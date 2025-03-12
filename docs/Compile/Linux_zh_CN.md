## 为 Linux 编译

作者：康林 <kl222@126.com>

### 快速开始

    # 仅编译 AppImage
    ./Script/build_linux.sh --appimage
    # 仅编译 deb 包
    ./Script/build_linux.sh --deb

详见：[脚本](#脚本)

### 环境
#### 操作系统

- Ubuntu

      ~$ lsb_release -a
      No LSB modules are available.
      Distributor ID:	Ubuntu
      Description:	Ubuntu 24.04.1 LTS
      Release:	24.04
      Codename:	noble

- Debian

      ~$ lsb_release -a
      No LSB modules are available.
      Distributor ID:	Debian
      Description:	Debian GNU/Linux 12 (bookworm)
      Release:	12
      Codename:	bookworm

#### Qt Creator

版本：v15.0.0。建议使用 v5.0.2 及以后版本，以前版本对 CMake 支持不够。

### 工具

#### 安装开发工具软件包

      ~$ sudo apt install build-essential

  - 开发工具软件包已经包括安装以下内容：
    - 编译器
      + GCC/G++

            ~$ sudo apt install g++ gcc

    - automake、autoconf、make、fakeroot

          ~$ sudo apt install automake autoconf make fakeroot

#### GIT: [http://www.git-scm.com](http://www.git-scm.com)

      ~$ sudo apt install git

#### CMAKE: [http://www.cmake.org](http://www.cmake.org)

  版本: 大于 3.27.0

      ~$ sudo apt install cmake

#### 打包工具: debhelper

      ~$ sudo apt install debhelper

#### Qt
  - Qt 官方发行版本： https://download.qt.io/official_releases/qt/
    - 则需要设置环境变量（或者 CMAKE 参数）：
      - QT_ROOT
      - Qt6：Qt6_ROOT 或者 Qt6_DIR 。
        详见：https://doc.qt.io/qt-6/cmake-get-started.html
      - Qt5：Qt5_ROOT 或者 Qt5_DIR 。
        详见：https://doc.qt.io/qt-5/cmake-get-started.html
      - 环境变量

            export QT_ROOT=Qt 安装位置
            # 编译 AppImage 需要
            export QMAKE=$QT_ROOT/bin/qmake

            # 当使用 Qt6 时
            export Qt6_ROOT=$QT_ROOT
            # 当使用 Qt5 时
            export Qt5_ROOT=$QT_ROOT

            # 或者
            # 当使用 Qt6 时
            export Qt6_DIR=$QT_ROOT/lib/cmake/Qt6
            # 当使用 Qt5 时
            export Qt5_DIR=$QT_ROOT/lib/cmake/Qt5

      - CMAKE 参数

            #当使用 Qt6 时
            cmake -DQT_ROOT=[Qt 安装位置] -DQt6_DIR=[Qt 安装位置] ......
            #当使用 Qt5 时
            cmake -DQT_ROOT=[Qt 安装位置] -DQt5_DIR=[Qt 安装位置] ......
            # 编译 AppImage 需要
            export QMAKE=$QT_ROOT/bin/qmake

  - 系统自带：
    - Qt5:

          ~$ sudo apt install qttools5-dev qttools5-dev-tools qtbase5-dev qtbase5-dev-tools qtmultimedia5-dev qtlocation5-dev libqt5svg5-dev libqtermwidget5-0-dev

    - Qt6: 详见： [Script/build_depend.sh](../../Script/build_depend.sh)

          ~$ sudo apt install qmake6 qt6-tools-dev qt6-tools-dev-tools qt6-base-dev qt6-base-dev-tools qt6-qpa-plugins libqt6svg6-dev qt6-l10n-tools qt6-translations-l10n qt6-scxml-dev qt6-multimedia-dev libqt6serialport6-dev

    - 系统安装多个分发版本 Qt 时。例如：同时安装 Qt5 和 Qt6 。
      系统使用 qtchooser 工具来选择当前的 Qt 版本。
  
          l@l:/home/RabbitRemoteControl$ qtchooser 
          Usage:
            qtchooser { -l | -list-versions | -print-env }
            qtchooser -install [-f] [-local] <name> <path-to-qmake>
            qtchooser -run-tool=<tool name> [-qt=<Qt version>] [program arguments]
            <executable name> [-qt=<Qt version>] [program arguments]

          Environment variables accepted:
           QTCHOOSER_RUNTOOL  name of the tool to be run (same as the -run-tool argument)
           QT_SELECT          version of Qt to be run (same as the -qt argument)
  
      - 查看当前系统安装的 Qt 版本

            l@l:/home/RabbitRemoteControl$ qtchooser -l
            4
            5
            default
            qt4-x86_64-linux-gnu
            qt4
            qt5-x86_64-linux-gnu
            qt5
            qt6

            # 查看当前环境的 Qt 版本
            l@l:/home/RabbitRemoteControl$ qtchooser --print-env
            QT_SELECT="default"
            QTTOOLDIR="[Paths]"
            QTLIBDIR="Prefix=/usr"
    
      - 设置当前环境的 Qt 版本

            export QT_SELECT=qt6  #设置当前环境 Qt 版本为 6

            # 查看当前环境的 Qt 版本
            l@l:/home/RabbitRemoteControl$ qtchooser --print-env
            QT_SELECT="qt6"
            QTTOOLDIR="/usr/lib/qt6/bin"
            QTLIBDIR="/usr/lib/aarch64-linux-gnu"

#### [可选] IDE: Qt Creator。建议使用 v5.0.2 及以后版本，以前版本对 CMake 支持不够。

      ~$ sudo apt install qtcreator

#### OpenGL: 运行 Qt 需要

      ~$ sudo apt install libglu1-mesa-dev

#### Doxygen: [http://www.doxygen.nl/](http://www.doxygen.nl/)

      ~$ sudo apt install doxygen

### 依赖库

- [必选] 玉兔公共库: [https://github.com/KangLin/RabbitCommon](https://github.com/KangLin/RabbitCommon)
- [必选] [Openssl:](https://github.com/openssl/openssl) 运行 Qt 需要
- [可选] FreeRDP: [https://github.com/FreeRDP/FreeRDP](https://github.com/FreeRDP/FreeRDP)
- [可选] RFB
  + [可选] RabbitVNC: [https://github.com/KangLin/RabbitVNC](https://github.com/KangLin/RabbitVNC)
  + [可选] LibVNCServer: [https://github.com/LibVNC/libvncserver](https://github.com/LibVNC/libvncserver)
  + [可选] TigerVNC: [https://github.com/KangLin/tigervnc](https://github.com/KangLin/tigervnc)
- [可选] QTermWidget: [https://github.com/lxqt/qtermwidget](https://github.com/lxqt/qtermwidget)
- [可选] SSH
  + libssh: 
    - [https://www.libssh.org](https://www.libssh.org/)
    - [libssh API](https://api.libssh.org/stable/index.html)
  + libssh2:
    - [https://www.libssh2.org](https://www.libssh2.org/)
    - [https://github.com/libssh2/libssh2](https://github.com/libssh2/libssh2/)
- [可选] libtelnet: [https://github.com/seanmiddleditch/libtelnet](https://github.com/seanmiddleditch/libtelnet)
- [可选] libdatachannel: [https://github.com/paullouisageneau/libdatachannel](https://github.com/paullouisageneau/libdatachannel)
- [可选] QXmpp: [https://github.com/qxmpp-project/qxmpp](https://github.com/qxmpp-project/qxmpp)
- [可选] QtService: https://github.com/KangLin/qt-solutions/
- [可选] PcapPlusPlus: [https://github.com/seladb/PcapPlusPlus](https://github.com/seladb/PcapPlusPlus)。插件 WakeOnLan 需要。
- [可选] FFMPEG: [https://ffmpeg.org/](https://ffmpeg.org/)。 Qt 多媒体功能需要

#### 玉兔公共库

- 此库默认放在与本项目同级目录下

      ~$ git clone https://github.com/KangLin/RabbitRemoteControl.git
      ~$ git clone https://github.com/KangLin/RabbitCommon.git

- 如果没有在同级目录下，则必须指定 CMake 参数或者环境变量:

      ~$ git clone https://github.com/KangLin/RabbitRemoteControl.git
      # 设置环境变量
      ~$ export RabbitCommon_ROOT=[RabbitCommon 安装目录]
      ~$ cd RabbitRemoteControl
      # 或者设置 CMake 参数
      ~/RabbitRemoteControl$ cmake -DRabbitCommon_ROOT=[RabbitCommon 安装目录] ......

#### FreeRDP

- 使用系统预编译开发库

      ~$ sudo apt install freerdp3-dev
    
- 使用 vcpkg
  + 源码位置: https://github.com/microsoft/vcpkg/
  
        ~$ git clone https://github.com/microsoft/vcpkg.git
        ~$ cd vcpkg
        ~/vcpkg$ ./bootstrap-vcpkg.sh
        ~/vcpkg$ vcpkg install freerdp

  + 指定 CMake 参数：-DCMAKE_TOOLCHAIN_FILE=[vcpkg 安装目录]/scripts/buildsystems/vcpkg.cmake
  + 使用 vcpk ， FreeRDP 服务不是默认功能，所以需要你手动指定。
- 从源码编译
  + 源码位置：https://github.com/FreeRDP/FreeRDP
  + 编译详见：https://github.com/FreeRDP/FreeRDP/wiki/Compilation

          ~$ git clone https://github.com/FreeRDP/FreeRDP.git
          ~$ cd FreeRDP
          ~/FreeRDP$ mkdir build
          ~/FreeRDP/build$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`pwd`/install -DWITH_SERVER=ON
          ~/FreeRDP/build$ cmake --build . --config Release --target install
          
- 当 FreeRDP 从源码编译时，编译本项需要指定的 CMake 参数：
  - -DBUILD_FREERDP=ON
  - -DFreeRDP-Client_DIR=[freerdp 安装目录]/lib/cmake/FreeRDP-Client3
  - -DFreeRDP_DIR=[freerdp 安装目录]/lib/cmake/FreeRDP3
  - -DWinPR_DIR=[freerdp 安装目录]/lib/cmake/WinPR3
  - -DFreeRDP-Shadow_DIR=[freerdp 安装目录]/lib/cmake/FreeRDP-Shadow3
  - -DFreeRDP-Server_DIR=[freerdp 安装目录]/lib/cmake/FreeRDP-Server3

#### LibVNCServer

- 使用系统预编译开发库

      ~$ sudo apt install libvncserver-dev

- 从源码编译
  + 源码位置：[https://github.com/LibVNC/libvncserver](https://github.com/LibVNC/libvncserver)
- 当 LibVNCServer 从源码编译时，编译本项需要指定的 CMake 参数：

      -DLibVNCServer_DIR=[LibVNCServer 安装目录]/lib/cmake/LibVNCServer

#### RabbitVNC

- 从源码编译
  - 源码位置: https://github.com/KangLin/RabbitVNC  

        ~$ sudo apt install libpixman-1-dev
        ~$ git clone https://github.com/KangLin/RabbitVNC.git
        ~$ cd RabbitVNC
        ~/RabbitVNC$ mkdir build
        ~/RabbitVNC$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`pwd`/install
        ~/RabbitVNC$ cmake --build . --config Release --target install
    
- 当 RabbitVNC 从源码编译时，编译本项需要指定的 CMake 参数：

      -DRabbitVNC_DIR=[RabbitVNC 安装目录]/lib/cmake/RabbitVNC

#### TigerVNC

- 从源码编译

官方只是个应用程序，不支持库。详见：https://github.com/TigerVNC/tigervnc/issues/1123  
所以本人在官方基础上做了修改。源码位置：https://github.com/KangLin/tigervnc  

    ~$ sudo apt install libpixman-1-dev
    ~$ git clone https://github.com/KangLin/tigervnc.git
    ~$ cd tigervnc
    ~/tigervnc$ mkdir build
    ~/tigervnc$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`pwd`/install
    ~/tigervnc$ cmake --build . --config Release --target install
    
- 当 TigerVNC 从源码编译时，编译本项需要指定的 CMake 参数：

      -Dtigervnc_DIR=[TigerVNC 安装目录]/lib/cmake/tigervnc

#### libdatachannel

- 使用 vcpkg
  + 源码位置: https://github.com/microsoft/vcpkg/

        ~$ git clone https://github.com/microsoft/vcpkg.git
        ~$ cd vcpkg
        ~/vcpkg$ vcpkg install libdatachannel
      
  + 指定 CMake 参数：-DCMAKE_TOOLCHAIN_FILE=[vcpkg 安装目录]/scripts/buildsystems/vcpkg.cmake
- 从源码编译
  + 源码位置： [https://github.com/paullouisageneau/libdatachannel](https://github.com/paullouisageneau/libdatachannel)
  + 编译详见： [https://github.com/paullouisageneau/libdatachannel/blob/master/BUILDING.md](https://github.com/paullouisageneau/libdatachannel/blob/master/BUILDING.md)

        ~$ git clone https://github.com/paullouisageneau/libdatachannel.git
        ~$ cd libdatachannel
        ~/libdatachannel$ git submodule update --init --recursive
        ~/libdatachannel$ mkdir build
        ~/libdatachannel$ cd build
        ~/libdatachannel/build$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`pwd`/install
        ~/libdatachannel/build$ cmake --build . --config Release --target install

- 当 libdatachannel 从源码编译时，编译本项需要指定的 CMake 参数：

      -DLibDataChannel_DIR=[libdatachannel 安装目录]/lib/cmake/LibDataChannel

#### QXmpp

- 使用系统预编译开发库

      ~$ sudo apt install libqxmpp-dev

- 从源码编译
  + 源码位置： [https://github.com/qxmpp-project/qxmpp](https://github.com/qxmpp-project/qxmpp)
  
        ~$ git clone https://github.com/qxmpp-project/qxmpp.git
        ~$ cd qxmpp
        ~/qxmpp$ mkdir build
        ~/qxmpp$ cd build
        ~/qxmpp/build$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`pwd`/install -DQt5_DIR=[Qt5 安装目录]/lib/cmake/Qt5
        ~/qxmpp/build$ cmake --build . --config Release --target install

- 当 QXmpp 从源码编译时，编译本项需要指定的 CMake 参数：

      -DQXmpp_DIR=[QXmpp 安装目录]/lib/cmake/qxmpp
  
#### QTermWidget

- 使用系统预编译开发库

      ~$ sudo apt install libqtermwidget5-0-dev
      
- 从源码编译
  + 源码位置： [https://github.com/lxqt/qtermwidget](https://github.com/lxqt/qtermwidget)
- 当 QTermWidget 从源码编译时，编译本项需要指定的 CMake 参数：

      -Dqtermwidget5_DIR=[qtermwidget 安装目录]/lib/cmake/qtermwidget5

#### libssh

- 使用系统预编译开发库

      ~$ sudo apt install libssh-dev 

- 使用 vcpkg
  + 源码位置: https://github.com/microsoft/vcpkg/

        ~$ git clone https://github.com/microsoft/vcpkg.git
        ~$ cd vcpkg
        ~/vcpkg$ vcpkg install libssh

  + 指定 CMake 参数：-DCMAKE_TOOLCHAIN_FILE=[vcpkg 安装目录]/scripts/buildsystems/vcpkg.cmake

- 从源码编译
  + 源码位置： [https://www.libssh.org](https://www.libssh.org)
- 当 libssh 从源码编译时，编译本项需要指定的 CMake 参数：

      -Dlibssh_DIR=[libssh 安装目录]/lib/cmake/libssh

#### QtService

- 从源码编译
  + 源码位置：: https://github.com/KangLin/qt-solutions/
  
        ~$ git clone https://github.com/KangLin/qt-solutions.git
        ~$ cd qt-solutions
        ~/qt-solutions$ mkdir build
        ~/qt-solutions$ cd build
        ~/qt-solutions/build$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`pwd`/install
        ~/qt-solutions/build$ cmake --build . --config Release --target install
        
- 当 QtService 从源码编译时，编译本项需要指定的 CMake 参数：

      -DQtService_DIR=[QtService 安装目录]/lib/cmake/QtService

### PcapPlusPlus

- 从源码编译
  + 源码位置：https://github.com/seladb/PcapPlusPlus

        ~$ git clone https://github.com/seladb/PcapPlusPlus.git

- 使用 vcpkg
  + 源码位置：https://github.com/microsoft/vcpkg/

        ~$ git clone https://github.com/microsoft/vcpkg.git
        ~$ cd vcpkg
        ~/vcpkg$ ./vcpkg install pcapplusplus

- 当 PcapPlusPlus 从源码编译时，编译本项需要指定的 CMake 参数：

      -DPcapPlusPlus_DIR=[PcapPlusPlus 安装目录]/lib/cmake/pcapplusplus

#### libpcap

PcapPlusPlus 依赖此库。

- 使用系统预编译开发库

      ~$ sudo apt install libpcap-dev

- 源码位置：https://github.com/the-tcpdump-group/libpcap

### 编译本项目

#### 源码

- 项目位置：[https://github.com/KangLin/RabbitRemoteControl](https://github.com/KangLin/RabbitRemoteControl)
- 下载源码

      ~$ git clone https://github.com/KangLin/RabbitRemoteControl.git

#### CMake 参数或者环境变量

- CMake 参数或者环境变量
  + [Qt](#Qt)
    + QT_ROOT: Qt 安装位置
    + Qt6: 详见：https://doc.qt.io/qt-6/cmake-get-started.html
      + Qt6_ROOT: 与 QT_ROOT 相同
      + Qt6_DIR: $QT_ROOT/lib/cmake/Qt6
    + Qt5: 详见：https://doc.qt.io/qt-6/cmake-get-started.html
      + Qt5_ROOT: 与 QT_ROOT 相同
      + Qt5_DIR: $QT_ROOT/lib/cmake/Qt5
  + RabbitCommon_ROOT: RabbitCommon 源码位置
  + BUILD_CLIENT: 编译客户端。默认为 ON
  + BUILD_SERVICE: 编译服务器端。默认依赖是否有 QtService
  + BUILD_PLUGINS: 编译插件。默认为 ON
  + BUILD_APP: 编译应用程序。默认为 ON
  + BUILD_DOCS: 编译文档。默认为 OFF
  + BUILD_SHARED_LIBS: 编译动态库。默认为 ON
  + BUILD_FREERDP：是否编译 FreeRDP。 默认为 OFF
  + WinPR_DIR: [freerdp 安装目录]/lib/cmake/WinPR2
  + FreeRDP_DIR: [freerdp 安装目录]/lib/cmake/FreeRDP2
  + FreeRDP-Client_DIR: [freerdp 安装目录]/lib/cmake/FreeRDP-Client2
  + BUILD_RABBITVNC: 编译 RabbitVNC 插件。默认为 ON
  + RabbitVNC_DIR: [RabbitVNC 安装目录]/lib/cmake/RabbitVNC
  + BUILD_TigerVNC: 编译 TigerVNC。默认为 ON
  + tigervnc_DIR: [TigerVNC 安装目录]/lib/cmake/tigervnc
  + BUILD_LibVNCServer: 编译 LibVNCServer。默认为 ON
  + LibVNCServer_DIR: [libvncserver 安装目录]/lib/cmake/LibVNCServer
  + LibDataChannel_DIR: [libdatachannel 安装目录]/lib/cmake/LibDataChannel
  + QXmpp_DIR=[QXmpp 安装目录]/lib/cmake/qxmpp
  + qtermwidget5_DIR: [qtermwidget 安装目录]/lib/cmake/qtermwidget5
  + libssh_DIR: [libssh 安装目录]/lib/cmake/libssh
  + QtService_DIR: [QtService 安装目录]/lib/cmake/QtService
- 如果使用 vcpkg，增加下面参数
  + CMAKE_TOOLCHAIN_FILE: [vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
  + VCPKG_MANIFEST_FEATURES: vcpkg 中的清单功能

#### 编译

- 命令行编译
  - 不用 vcpkg
     
        ~$ cd RabbitRemoteControl
        ~/RabbitRemoteControl$ mkdir build
        ~/RabbitRemoteControl/build$ cmake .. -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=`pwd`/install 
        ~/RabbitRemoteControl/build$ cmake --build . --config Release --target install

  - 使用 vcpkg
     
        ~$ cd RabbitRemoteControl
        ~/RabbitRemoteControl$ mkdir build
        ~/RabbitRemoteControl/build$ cmake .. -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=`pwd`/install \
            -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake \
            [可选依赖库]
        ~/RabbitRemoteControl/build$ cmake --build . --config Release --target install

- IDE(Qt Creator) 编译
  - 打开项目: “菜单→文件→打开文件或项目”，选择项目根目录中的 CMakeLists.txt 
  - 配置：点左侧工具栏上的 “项目→构建与运行”，配置 CMake 参数
    - 如果要安装，还需要　“项目→构建与运行→构建步骤→目标”　中，选中　install
  - 编译与运行： 点左侧工具栏上的 “开始调试” 或者按快捷键 “F5”
  - 如果用 vcpkg: 选项→Kits→Cmake Configureration: 增加 CMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake

- 使用脚本 [build_debpackage.sh](../../Script/build_debpackage.sh)
  - 设置 [CMake 参数或者环境变量](#CMake-参数或者环境变量)为环境变量。例如：
  
        export RabbitVNC_DIR=[RabbitVNC 安装目录]/lib/cmake/RabbitVNC
        export tigervnc_DIR=[TigerVNC 安装目录]/lib/cmake/tigervnc
        export QtService_DIR=[QtService 安装目录]/lib/cmake/QtService
        
  - 使用脚本 [build_debpackage.sh](../../Script/build_debpackage.sh)

        ./Script/build_debpackage.sh [$QT_ROOT] [$RabbitCommon_ROOT]

#### 运行

- 程序安装在 install/bin 下
  
      ~$ cd RabbitRemoteControl
      ~/RabbitRemoteControl$ cd build/install/bin
      ~/RabbitRemoteControl$ ./RabbitRemoteControl.sh
      # 或者
      ~/RabbitRemoteControl$ ./RabbitRemoteControlApp

  **注意：** 如果插件没有加载。则可能是插件的依赖库没有安装到系统。
  你可以把依赖库的目录加入到环境变量 LD_LIBRARY_PATH 中。
  你也可以把依赖库的目录加入到 /etc/ld.so.conf 文件中，
  然后运行 ldconfig 把依赖库加入到系统中。

- Qt 变量
  - QT_DEBUG_PLUGINS: 调试插件

    `export QT_DEBUG_PLUGINS=1`

  - QT_QPA_PLATFORM: 加载指定的平台。例如使用
    [vnc](https://github.com/qt-rick/qtbase-vnc-platform-plug)

        export QT_QPA_PLATFORM=vnc

### 编译例子

#### Ubuntu

    ```bash
    # 安装开发工具软件包
    ~$ sudo apt install build-essential
    # 安装工具
    ~$ sudo apt install git cmake debhelper doxygen
    # [可选] 安装 Qt5
    ~$ sudo apt install qttools5-dev qttools5-dev-tools qtbase5-dev \
        qtbase5-dev-tools qtmultimedia5-dev qtlocation5-dev libqt5svg5-dev \
        libqtermwidget5-0-dev
    # 安装 X 开发库
    ~$ sudo apt install libxkbcommon-dev libxkbcommon-x11-dev libx11-xcb-dev \
        libx11-dev libxfixes-dev
    ~$ sudo apt install libutf8proc-dev libpam0g-dev #编译 qtermwidget 需要
    # 安装依赖库
    ~$ sudo apt install freerdp2-dev libvncserver-dev libssh-dev libtelnet-dev
    ~$ sudo apt install debhelper fakeroot
    # 安装 pixman ,RabbitVNC 和 TigerVNC 需要它
    ~$ sudo apt install libpixman-1-dev
    # 编译 RabbitVNC
    ~$ git clone https://github.com/KangLin/RabbitVNC.git
    ~$ cd RabbitVNC
    ~$ mkdir build
    ~/RabbitVNC/build$ cmake .. -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=`pwd`/install
    ~/RabbitVNC/build$ cmake --build . --config Release --target install
    ~/RabbitVNC/build$ cd ~
    # 编译 TigerVNC
    ~$ git clone https://github.com/KangLin/tigervnc.git
    ~$ cd tigervnc
    ~$ mkdir build
    ~/tigervnc/build$ cmake .. -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=`pwd`/install
    ~/tigervnc/build$ cmake --build . --config Release --target install
    ~/tigervnc/build$ cd ~
    ~$ sudo apt install libqxmpp-dev
    # 编译 libdatachannel
    ~$ git clone https://github.com/paullouisageneau/libdatachannel.git
    ~$ cd libdatachannel
    ~/libdatachannel$ git submodule update --init --recursive
    ~/libdatachannel$ mkdir build
    ~/libdatachannel$ cd build
    ~/libdatachannel/build$ cmake .. -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=`pwd`/install
    ~/libdatachannel/build$ cmake --build . --config Release --target install
    ~/libdatachannel/build$ cd ~
    # 编译 QtService
    ~$ git clone https://github.com/KangLin/qt-solutions.git
    ~$ cd qt-solutions/qtservice
    ~/qt-solutions/qtservice$ mkdir build
    ~/qt-solutions/qtservice$ cd build
    ~/qt-solutions/qtservice/build$ cmake .. -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=`pwd`/install
    ~/qt-solutions/qtservice/build$ cmake --build . --config Release --target install
    ~/qt-solutions/qtservice/build$ cd ~
    ~$ git clone https://github.com/KangLin/RabbitCommon.git
    ~$ git clone https://github.com/KangLin/RabbitRemoteControl.git
    ~$ cd RabbitRemoteControl
    ~/RabbitRemoteControl$ mkdir build
    ~/RabbitRemoteControl$ cd build
    ~/RabbitRemoteControl/build$ cmake .. -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=`pwd`/install \
        -DBUILD_FREERDP=ON \
        -DRabbitVNC_DIR=~/RabbitVNC/build/install/lib/cmake/RabbitVNC \
        -Dtigervnc_DIR=~/tigervnc/build/install/lib/cmake/tigervnc \
        -DLibDataChannel_DIR=~/libdatachannel/build/install/lib/cmake/LibDataChannel \
        -DQtService_DIR=~/qt-solutions/qtservice/build/lib/cmake/QtService
    ~/RabbitRemoteControl/build$ cmake --build . --config Release --target install
    ```

详见：[脚本](#脚本)

### [AppImage](https://github.com/linuxdeploy/linuxdeploy)

- 构建

      ~/RabbitRemoteControl/build$ cmake .. \
          -DCMAKE_INSTALL_PREFIX=/usr \
          -DCMARK_SHARED=OFF \
          -DCMARK_TESTS=OFF \
          -DCMARK_STATIC=ON \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=/usr \
          -DBUILD_QUIWidget=OFF \
          -DBUILD_APP=ON \
          -DBUILD_FREERDP=ON
      cmake --build . --parallel $(nproc)
      cmake --install . --config Release --component DependLibraries --prefix AppDir/usr
      cmake --install . --config Release --component Runtime --prefix AppDir/usr
      cmake --install . --config Release --component Application --prefix AppDir/usr
      cmake --install . --config Release --component Plugin --prefix AppDir/usr

- 建立 AppImage

      # See: https://github.com/linuxdeploy/linuxdeploy-plugin-qt
      #export QMAKE=${QT_ROOT}/bin/qmake
      export EXTRA_PLATFORM_PLUGINS="libqxcb.so"
      # Icons from theme are not displayed in QtWidgets Application: https://github.com/linuxdeploy/linuxdeploy-plugin-qt/issues/17
      export EXTRA_QT_MODULES="svg"
      ./linuxdeploy-`uname -m`.AppImage --appdir=AppDir -v0 \
          --deploy-deps-only=AppDir/usr/plugins/Client \
          --deploy-deps-only=AppDir/usr/lib/`uname -m`-linux-gnu \
          --plugin qt \
          --output appimage

- 运行

      sudo chmod u+x ./Rabbit_Remote_Control-`uname -m`.AppImage
      ./Rabbit_Remote_Control-`uname -m`.AppImage

- 其它
  - 解压 AppImage

        ./Rabbit_Remote_Control-`uname -m`.AppImage --appimage-extract # 解压 AppImage 文件

  - FUSE 问题

    第一次执行AppImage文件的时候可能会碰到 PUSE 相关的问题，报错如下：

    ```
    dlopen(): error loading libfuse.so.2

    AppImages require FUSE to run.
    You might still be able to extract the contents of this AppImage
    if you run it with the --appimage-extract option.
    See https://github.com/AppImage/AppImageKit/wiki/FUSE

    for more information
    ```

    解决方案：安装 fuse2
    
        sudo apt install fuse

  - 在 docker 中运行时出现 FUSE 错误
    
    在创建容器的时候加上参数: `--privileged`
    
        docker run --privileged --interactive ubuntu
    
- 脚本：[build_appimage.sh](../../Script/build_appimage.sh)

### snap

- 构建:
  - Parts 生命周期: https://snapcraft.io/docs/parts-lifecycle
  - https://snapcraft.io/docs/how-snapcraft-builds
        
        ~$ git clone https://github.com/KangLin/RabbitRemoteControl.git
        ~$ cd RabbitRemoteControl
        ~$ snapcraft -v 

    这些生命周期步骤中的每一个都可以从命令行运行，
    并且该命令可以是特定于部件的命令，也可以是应用于项目中的所有部件的命令。
    
    ```
    snapcraft pull [<part-name>]
    snapcraft build [<part-name>]
    snapcraft stage [<part-name>]
    snapcraft prime [<part-name>]
    snapcraft pack or snapcraft
    ```

  - 迭代构建: https://snapcraft.io/docs/iterating-over-a-build
    通过以下命令，您可以单步执行此封装环境：
    - --shell：将快照构建到指定之前的生命周期步骤，并在环境中打开一个 shell
      例如，运行 snapcraft prime [<part-name>] --shell 将运行到暂存步骤并打开一个 shell）。
    - --shell-after：构建对指定生命周期步骤的快照，并在环境中打开 shell。
     （例如，运行 snapcraft prime [<part-name>] --shell-after 将运行到 prime 步骤，然后进入 shell）。
    - --debug 在发生错误后在环境中打开一个 shell。

          $ # 例如 prime freerdp
          $ snapcraft prime freerdp --shell-after -v --debug

  - 清理
  
        snapcraft clean

- 调试
  - 开发模式安装 （--devmode）。未签名的。

        snap install ./rabbitremotecontrol_0.0.27_amd64.snap --devmode

  - 运行

        rabbitremotecontrol

  - 卸载

        snap remove rabbitremotecontrol

### Flatpak

- 安装构建和运行 flatpak 所需的软件：flatpak 和 flatpak-builder
  - 在 Fedora 上我们必须运行：

        $ sudo dnf install flatpak flatpak-builder
        # 其它工具
        $ sudo dnf install autogen git cmake gcc autoconf

  - 在 Debian/Ubuntu 或基于它的众多发行版之一上，相反：

        $ sudo apt-get update && sudo apt-get install flatpak flatpak-builder

  - 在 Archlinux 上我们可以使用 pacman 来执行相同的操作：

        $ sudo pacman -Sy flatpak flatpak-builder

- 添加 [Flathub](https://flathub.org/) 仓库：

      $ flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo

  - 查看flatpak仓库的详细信息

        $ flatpak remotes --show-details

- 构建

      $ flatpak-builder build-dir io.github.KangLin.RabbitRemoteControl.json
      ** (flatpak-builder:37): WARNING **: 03:05:45.408: Unknown property branch for type BuilderSourceDir
      error: org.kde.Sdk/x86_64/6.8 not installed
      Failed to init: Unable to find sdk org.kde.Sdk version 6.8

提示 sdk 没有安装。指定的运行时和 sdk 不会自动安装，我们必须手动安装。
要仅为我们的用户从 flathub 存储库安装它们，我们使用以下命令：

      $ flatpak install org.kde.Sdk//6.8 org.kde.Platform//6.8

安装完所有的 sdk 和运行库后，再构建：

      $ flatpak-builder --force-clean build-dir io.github.KangLin.RabbitRemoteControl.json
      # 或者 用户模式
      $ flatpak-builder --user --force-clean --install build-dir io.github.KangLin.RabbitRemoteControl.json 

- 安装

应用程序构建完成，我们就可以安装它。我们所要做的就是运行以下命令：

      $ flatpak-builder --user --force-clean --install build-dir io.github.KangLin.RabbitRemoteControl.json

- 运行

      $ flatpak run io.github.KangLin.RabbitRemoteControl

- 创建存储库

如果到目前为止一切都按预期工作，我们可以继续将我们的应用程序放入存储库中。
在这里，我们将创建自己的存储库，但如果您想将应用程序上线，
则需要按照要上传到的网站（例如 FlatHub）的提交说明进行操作。
我们将再次构建应用程序，但这次添加 --repo 选项。我们将我们的存储库称为“Rabbit”。


    $ flatpak-builder --repo=Rabbit --force-clean build-dir io.github.KangLin.RabbitRemoteControl.json

- 添加到 Flatpak

接下来，我们可以将新存储库添加到 Flatpak，然后从存储库安装我们的应用程序。


    $ flatpak --user remote-add --no-gpg-verify Rabbit Rabbit
    $ flatpak --user install Rabbit io.github.KangLin.RabbitRemoteControl

请注意，我们在此处使用 --user 选项以避免安装应用程序以供系统范围使用。如果您愿意，可以忽略此选项。

- 问题
  - 在 docker 中运行时出现 FUSE 错误
  
        Initializing build dir
        Committing stage init to cache
        Starting build of io.github.KangLin.RabbitRemoteControl
        fuse: device not found, try 'modprobe fuse' first
        Error: Failure spawning rofiles-fuse, exit_status: 1024

    在创建容器的时候加上参数: `--privileged`

        docker run --privileged --interactive ubuntu

- 文档
  - [flatpak 清单文件](https://docs.flatpak.org/en/latest/flatpak-builder-command-reference.html#flatpak-manifest)
  - [提交到 Flathub](https://docs.flathub.org/docs/for-app-authors/submission)

### 脚本

- 构建脚本
  - [build_depend.sh](../../Script/build_depend.sh): 构建、安装依赖库。
  - [build_debpackage.sh](../../Script/build_debpackage.sh): 构建 deb 安装包。
    执行前，请设置环境变量。参见：[CMake 参数或者环境变量](#CMake-参数或者环境变量)
  - [build_appimage.sh](../../Script/build_appimage.sh): 构建 AppImage 包。
    执行前，请设置环境变量。参见：[CMake 参数或者环境变量](#CMake-参数或者环境变量)
  - 示例
    - 使用 build_linux.sh

          # 仅构建 AppImage
          ./Script/build_linux.sh --appimage
          # 仅构建 deb 包
          ./Script/build_linux.sh --deb
          # 如果同时构建 deb 包和 AppImage，请用 docker 。否则 qt 可能会产生污染。
          ./Script/build_linux.sh --docker --deb --appimage

    - 分步构建

          # 如果是从 [Qt官网](download.qt.io) 安装的 Qt
          export QT_ROOT=
          export Qt6_ROOT=$QT_ROOT
          export QMAKE=$QT_ROOT/bin/qmake
          # 如果是系统自带的 Qt
          export QT_SELECT=qt6

          # 注意：下面环境变量一定要使用绝对路径
          export BUILD_DIR=`pwd`/build
          export INSTALL_DIR=$BUILD_DIR/install
          export SOURCE_DIR=$BUILD_DIR/source
          export TOOLS_DIR=$BUILD_DIR/tools

          # 安装依赖库
          sudo ./Script/build_depend.sh --apt_update --base --default --rabbitcommon \
              --tigervnc --pcapplusplus \
              --install ${INSTALL_DIR} \
              --source ${SOURCE_DIR} \
              --tools ${TOOLS_DIR} \
              --build ${BUILD_DIR}

          # 设置环境变量
          export RabbitCommon_ROOT=${SOURCE_DIR}/RabbitCommon
          export tigervnc_DIR=${INSTALL_DIR}/lib/cmake/tigervnc
          export PcapPlusPlus_DIR=${INSTALL_DIR}/lib/cmake/pcapplusplus

          # 构建 deb 包
          ./Script/build_debpackage.sh

          # 构建 AppImage
          ./Script/build_appimage.sh

- [deploy.sh](../../deploy.sh): 分发版本。仅由管理员使用。
