## 为 Linux 编译
作者：康林 <kl222@126.com>

### 环境
#### 操作系统

    ~$ lsb_release -a
    No LSB modules are available.
    Distributor ID:	Ubuntu
    Description:	Ubuntu 20.04.2 LTS
    Release:	20.04
    Codename:	Focal Fossa

#### Qt Creator

版本：v5.0.2。建议使用 v5.0.2 及以后版本，以前版本对 CMake 支持不够。

### 工具

- 编译器
  + GCC/G++

        ~$ sudo apt install g++ gcc
        
- Qt
  + 系统自带：
  
        ~$ sudo apt install qttools5-dev qttools5-dev-tools qtbase5-dev qtbase5-dev-tools qtmultimedia5-dev qtlocation5-dev libqt5svg5-dev libqtermwidget5-0-dev
        
  + Qt 官方发行版本： https://download.qt.io/official_releases/qt/  
    当前使用版本: Qt 5.12.12

- [可选] IDE: Qt Creator。建议使用 v5.0.2 及以后版本，以前版本对 CMake 支持不够。
  
      ~$ sudo apt install sudo apt install qtcreator
  
- GIT: [http://www.git-scm.com](http://www.git-scm.com)

        ~$ sudo apt install git
        
- CMAKE: [http://www.cmake.org](http://www.cmake.org)

        ~$ sudo apt install cmake
        
- AutoMake、AutoConf、Make

        ~$ sudo apt install automake autoconf make

- Doxygen: [http://www.doxygen.nl/](http://www.doxygen.nl/)

- 打包工具: debhelper、 fakeroot

### 编译

```bash
    # 安装依赖库
    ~$ sudo apt install freerdp2-dev libvncserver-dev libssh-dev libtelnet-dev
    ~$ sudo apt install debhelper fakeroot
    # 安装 Qt
    ~$ sudo apt install qttools5-dev qttools5-dev-tools qtbase5-dev qtbase5-dev-tools qtmultimedia5-dev qtlocation5-dev libqt5svg5-dev libqtermwidget5-0-dev
    # 安装 X 开发库
    ~$ sudo apt install libxkbcommon-dev libxkbcommon-x11-dev libx11-xcb-dev libx11-dev libxfixes-dev
    ~$ sudo apt install libutf8proc-dev libpam0g-dev #编译 qtermwidget 需要
    # 安装 pixman ,RabbitVNC 和 TigerVNC 需要它
    ~$ sudo apt install libpixman-1-dev
    # 编译 RabbitVNC
    ~$ git clone https://github.com/KangLin/RabbitVNC.git
    ~$ cd RabbitVNC
    ~$ mkdir build
    ~/RabbitVNC/build$ cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install
    ~/RabbitVNC/build$ cmake --build . --target install
    ~/RabbitVNC/build$ cd ~
    # 编译 TigerVNC
    ~$ git clone https://github.com/KangLin/tigervnc.git
    ~$ cd tigervnc
    ~$ mkdir build
    ~/tigervnc/build$ cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install
    ~/tigervnc/build$ cmake --build . --target install
    ~/tigervnc/build$ cd ~
    ~$ sudo apt install libqxmpp-dev
    # 编译 libdatachannel
    ~$ git clone https://github.com/paullouisageneau/libdatachannel.git
    ~$ cd libdatachannel
    ~/libdatachannel$ git submodule update --init --recursive
    ~/libdatachannel$ mkdir build
    ~/libdatachannel$ cd build
    ~/libdatachannel/build$ cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install
    ~/libdatachannel/build$ cmake --build . --target install
    ~/libdatachannel/build$ cd ~
    # 编译 QtService
    ~$ git clone https://github.com/KangLin/qt-solutions.git
    ~$ cd qt-solutions/qtservice
    ~/qt-solutions/qtservice$ mkdir build
    ~/qt-solutions/qtservice$ cd build
    ~/qt-solutions/qtservice/build$ cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install
    ~/qt-solutions/qtservice/build$ cmake --build . --target install
    ~/qt-solutions/qtservice/build$ cd ~
    ~$ git clone https://github.com/KangLin/RabbitCommon.git
    ~$ git clone https://github.com/KangLin/RabbitRemoteControl.git
    ~$ cd RabbitRemoteControl
    ~/RabbitRemoteControl$ mkdir build
    ~/RabbitRemoteControl$ cd build
    ~/RabbitRemoteControl/build$ cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install -DRabbitVNC_DIR=~/RabbitVNC/build/install/lib/cmake/RabbitVNC -Dtigervnc_DIR=~/tigervnc/build/install/lib/cmake/tigervnc -DBUILD_FREERDP=ON -DLibDataChannel_DIR=~/libdatachannel/build/install/lib/cmake/LibDataChannel -DQtService_DIR=~/qt-solutions/qtservice/build/lib/cmake/QtService
    ~/RabbitRemoteControl/build$ cmake --build . --target install
```

参见：[编译集成](../../.github/workflows/ubuntu.yml)

### 依赖库

- [必选] 玉兔公共库: [https://github.com/KangLin/RabbitCommon](https://github.com/KangLin/RabbitCommon)
- [可选] RFB
  + [可选] RabbitVNC: [https://github.com/KangLin/RabbitVNC](https://github.com/KangLin/RabbitVNC)
  + [可选] LibVNCServer: [https://github.com/LibVNC/libvncserver](https://github.com/LibVNC/libvncserver)
  + [可选] TigerVNC: [https://github.com/KangLin/tigervnc](https://github.com/KangLin/tigervnc)
- [可选] FreeRDP: [https://github.com/FreeRDP/FreeRDP](https://github.com/FreeRDP/FreeRDP)
- [可选] SSH
  + libssh: [https://www.libssh.org](https://www.libssh.org/)
  + libssh2:
    - [https://www.libssh2.org](https://www.libssh2.org/)
    - [https://github.com/libssh2/libssh2](https://github.com/libssh2/libssh2/)
- [可选] QTermWidget: [https://github.com/lxqt/qtermwidget](https://github.com/lxqt/qtermwidget)
- [可选] libtelnet: [https://github.com/seanmiddleditch/libtelnet](https://github.com/seanmiddleditch/libtelnet)
- [可选] libdatachannel: [https://github.com/paullouisageneau/libdatachannel](https://github.com/paullouisageneau/libdatachannel)
- [可选] QXmpp: [https://github.com/qxmpp-project/qxmpp](https://github.com/qxmpp-project/qxmpp)
- [可选] QtService: https://github.com/KangLin/qt-solutions/

#### 玉兔公共库
此库默认放在与本项目同级目录下，如果没有在同级目录下，则必须指定 CMake 参数:
-DRabbitCommon_DIR=[RabbitCommon 安装目录]

    ~$ git clone https://github.com/KangLin/RabbitCommon.git
    
#### FreeRDP
- 使用系统预编译开发库

      ~$ sudo apt install freerdp2-dev
    
- 使用 vcpkg
  + 源码位置: https://github.com/microsoft/vcpkg/
  
        ~$ git clone https://github.com/microsoft/vcpkg.git
        ~$ cd vcpkg
        ~/vcpkg$ ./bootstrap-vcpkg.sh
        ~/vcpkg$ vcpkg install freerdp

  + 指定 CMake 参数：-DCMAKE_TOOLCHAIN_FILE=[vcpkg 安装目录]/scripts/buildsystems/vcpkg.cmake
  + 使用 vcpk ，不支持 FreeRDP 服务，所以需要从源码编译它。
- 从源码编译
  + 源码位置：https://github.com/FreeRDP/FreeRDP
  + 编译详见：https://github.com/FreeRDP/FreeRDP/wiki/Compilation

          ~$ git clone https://github.com/FreeRDP/FreeRDP.git
          ~$ cd FreeRDP
          ~/FreeRDP$ mkdir build
          ~/FreeRDP/build$ cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install -DWITH_SERVER=ON
          ~/FreeRDP/build$ cmake --build . --target install
          
  + 指定 CMake 参数：
    - -DBUILD_FREERDP=ON
    - -DFreeRDP-Client_DIR=[freerdp 安装目录]/lib/cmake/FreeRDP-Client2
    - -DFreeRDP_DIR=[freerdp 安装目录]/lib/cmake/FreeRDP2
    - -DWinPR_DIR=[freerdp 安装目录]/lib/cmake/WinPR2

#### LibVNCServer
- 使用系统预编译开发库

      ~$ sudo apt install libvncserver-dev

- 从源码编译
  + 源码位置：[https://github.com/LibVNC/libvncserver](https://github.com/LibVNC/libvncserver)
  + 指定 CMake 参数：-DLibVNCServer_DIR=[LibVNCServer 安装目录]/lib/cmake/LibVNCServer

#### RabbitVNC
- 从源码编译
源码位置: https://github.com/KangLin/RabbitVNC  

      ~$ sudo apt install libpixman-1-dev
      ~$ git clone https://github.com/KangLin/RabbitVNC.git
      ~$ cd RabbitVNC
      ~/RabbitVNC$ mkdir build
      ~/RabbitVNC$ cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install
      ~/RabbitVNC$ cmake --build . --target install
    
- 指定 CMake 参数：-DRabbitVNC_DIR=[RabbitVNC 安装目录]/lib/cmake/RabbitVNC

#### TigerVNC
- 从源码编译

官方只是个应用程序，不支持库。详见：https://github.com/TigerVNC/tigervnc/issues/1123  
所以本人在官方基础上做了修改。源码位置：https://github.com/KangLin/tigervnc  

    ~$ sudo apt install libpixman-1-dev
    ~$ git clone https://github.com/KangLin/tigervnc.git
    ~$ cd tigervnc
    ~/tigervnc$ mkdir build
    ~/tigervnc$ cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install
    ~/tigervnc$ cmake --build . --target install
    
- 指定 CMake 参数：-Dtigervnc_DIR=[TigerVNC 安装目录]/lib/cmake/tigervnc

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
        ~/libdatachannel/build$ cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install -DQt5_DIR=[Qt 安装目录]/lib/cmake/Qt5
        ~/libdatachannel/build$ cmake --build . --target install

  + 指定 CMake 参数: -DLibDataChannel_DIR=[libdatachannel 安装目录]/lib/cmake/LibDataChannel

#### QXmpp
- 从源码编译
  + 源码位置： [https://github.com/qxmpp-project/qxmpp](https://github.com/qxmpp-project/qxmpp)
  
        ~$ git clone https://github.com/qxmpp-project/qxmpp.git
        ~$ cd qxmpp
        ~/qxmpp$ mkdir build
        ~/qxmpp$ cd build
        ~/qxmpp/build$ cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install
        ~/qxmpp/build$ cmake --build . --target install

  + 指定 CMake 参数: -DQXmpp_DIR=[QXmpp 安装目录]/lib/cmake/qxmpp
  
#### QTermWidget
- 使用系统预编译开发库

      ~$ sudo apt install libqtermwidget5-0-dev
      
- 从源码编译
  + 源码位置： [https://github.com/lxqt/qtermwidget](https://github.com/lxqt/qtermwidget)
  + 指定 CMake 参数：-Dqtermwidget5_DIR=[qtermwidget 安装目录]/lib/cmake/qtermwidget5

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
  + 指定 CMake 参数：-Dlibssh_DIR=[libssh 安装目录]/lib/cmake/libssh

#### QtService
- 从源码编译
  + 源码位置：: https://github.com/KangLin/qt-solutions/
  
        ~$ git clone https://github.com/KangLin/qt-solutions.git
        ~$ cd qt-solutions
        ~/qt-solutions$ mkdir build
        ~/qt-solutions$ cd build
        ~/qt-solutions/build$ cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install
        ~/qt-solutions/build$ cmake --build . --target install
        
  + 指定 CMake 参数: -DQtService_DIR=[QtService 安装目录]/lib/cmake/QtService
  
### 编译本项目
- 项目位置：[https://github.com/KangLin/RabbitRemoteControl](https://github.com/KangLin/RabbitRemoteControl)
- 下载源码

      ~$ git clone https://github.com/KangLin/RabbitRemoteControl.git

- CMake 参数
  + RabbitCommon_DIR: RabbitCommon 源码位置
  + BUILD_DOCS: 编译文档。默认为 OFF
  + BUILD_FREERDP：是否编译 FreeRDP。 默认为 OFF
  + BUILD_QUIWidget: 用无边框窗口做为主窗口。默认为 OFF
  + BUILD_SHARED_LIBS: 编译动态库。默认为 ON
  + WinPR_DIR:PATH: [freerdp 安装目录]/lib/cmake/WinPR2
  + FreeRDP_DIR: [freerdp 安装目录]/lib/cmake/FreeRDP2
  + FreeRDP-Client_DIR: [freerdp 安装目录]/lib/cmake/FreeRDP-Client2
  + RabbitVNC_DIR: [RabbitVNC 安装目录]/lib/cmake/RabbitVNC
  + tigervnc_DIR: [TigerVNC 安装目录]/lib/cmake/tigervnc
  + LibVNCServer_DIR: [libvncserver 安装目录]/lib/cmake/LibVNCServer
  + LibDataChannel_DIR: [libdatachannel 安装目录]/lib/cmake/LibDataChannel
  + QXmpp_DIR=[QXmpp 安装目录]/lib/cmake/qxmpp
  + qtermwidget5_DIR: [qtermwidget 安装目录]/lib/cmake/qtermwidget5
  + libssh_DIR: [libssh 安装目录]/lib/cmake/libssh
  + QtService_DIR: [QtService 安装目录]/lib/cmake/QtService
  
- 如果使用 vcpkg，增加下面参数
  + CMAKE_TOOLCHAIN_FILE: [vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
  
- 安装目标
  + install-runtime: 只安装运行库和程序
  + install: 安装所有库（运行库与开发库）和程序
  
- 编译
  + 命令行编译
     - 不用 vcpkg
     
           ~$ cd RabbitRemoteControl
           ~/RabbitRemoteControl$ mkdir build
           ~/RabbitRemoteControl/build$ cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install 
           ~/RabbitRemoteControl/build$ cmake --build . --target install-runtime

     - 使用 vcpkg
     
           ~$ cd RabbitRemoteControl
           ~/RabbitRemoteControl$ mkdir build
           ~/RabbitRemoteControl/build$ cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install [可选依赖库] -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
           ~/RabbitRemoteControl/build$ cmake --build . --target install-runtime

  + IDE(Qt Creator) 编译
    - 打开项目: 菜单→文件→打开文件或项目，选择项目根目录中的 CMakeLists.txt 
    - 配置：点左侧工具栏上的 项目→编译与运行，配置 CMake 参数
    - 编译与运行： 点左侧工具栏上的 “开始调试” 或者按快捷键 F5
    - 如果用 vcpkg: 选项→Kits→Cmake Configureration: 增加 CMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake

  + 使用脚本 build_debpackage.sh
    - 设置[编译本项目](#编译本项目) → CMake 参数为环境变量。例如：
  
          export RabbitVNC_DIR=[RabbitVNC 安装目录]/lib/cmake/RabbitVNC
          export tigervnc_DIR=[TigerVNC 安装目录]/lib/cmake/tigervnc
          export QtService_DIR=[QtService 安装目录]/lib/cmake/QtService
        
    - 使用脚本 build_debpackage.sh

          ./build_debpackage.sh $QT_ROOT $RabbitCommon_DIR
