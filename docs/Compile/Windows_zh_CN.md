## 为 Windows 编译
作者：康林 <kl222@126.com>

### 环境
#### 操作系统

    Windows 10 版本 20H2 (19042.985)

#### Qt Creator

版本：v5.0.2。建议使用 v5.0.2 及以后版本，以前版本对 CMake 支持不够。

### 工具

- 编译器
  + Visual Studio
    * 主页
      - https://visualstudio.microsoft.com/vs/
      - http://msdn.microsoft.com/zh-cn/vstudio 
      - https://visualstudio.microsoft.com/zh-hans/downloads/
    * 版本：
      - Visual Studio 2013
      - Visual Studio 2015
      - Visual Studio 2017
      - Visual Studio 2019
      - Visual Studio 2022
    * Visual Studio 各版本密钥：https://github.com/KangLin/Documents/blob/master/c/VisualStudioKey.md
    * 当前使用版本：VS 2017
    
- Windows SDK：https://developer.microsoft.com/en-us/windows/downloads/sdk-archive  
    根据你的操作系统安装相应的 SDK。 CDB必须要安装，QT调试需要。
- [可选] Windows Driver Kit：https://docs.microsoft.com/zh-cn/windows-hardware/drivers/download-the-wdk
   
- Qt
  + Qt 官方发行版本：https://download.qt.io/official_releases/qt/  
    当前使用版本：Qt 5.12.12
  + IDE：Qt Creator。建议使用 v5.0.2 及以后版本，以前版本对 CMake 支持不够。
- Git: [https://www.git-scm.com/](https://www.git-scm.com/)  
  [Git 设置](http://blog.csdn.net/kl222/article/details/32903495)
- CMake: [https://www.cmake.org/](https://cmake.org/)
- Doxygen: [http://www.doxygen.nl/](http://www.doxygen.nl/)
- Nsis: [https://nsis.sourceforge.io/Download](https://nsis.sourceforge.io/Download)

### 编译

参见：[编译集成](../../.github/workflows/msvc.yml)

### 依赖库

- [必选] 玉兔公共库: [https://github.com/KangLin/RabbitCommon](https://github.com/KangLin/RabbitCommon)
- [可选] RFB
  + [可选] RabbitVNC: [https://github.com/KangLin/RabbitVNC](https://github.com/KangLin/RabbitVNC)
  + [可选] LibVNCServer: [https://github.com/LibVNC/libvncserver](https://github.com/LibVNC/libvncserver)
  + [可选] TigerVNC: [https://github.com/KangLin/tigervnc](https://github.com/KangLin/tigervnc)
- [可选] FreeRDP: [https://github.com/FreeRDP/FreeRDP](https://github.com/FreeRDP/FreeRDP)
- [可选] SSH
  + libssh: libssh: [https://www.libssh.org](https://www.libssh.org)
  + libssh2:
    - [https://www.libssh2.org](libssh2: https://www.libssh2.org/)
    - [https://github.com/libssh2/libssh2](https://github.com/libssh2/libssh2/)
- [可选] QTermWidget: [https://github.com/lxqt/qtermwidget](https://github.com/lxqt/qtermwidget)
- [可选] libtelnet: [https://github.com/seanmiddleditch/libtelnet](https://github.com/seanmiddleditch/libtelnet)
- [可选] libdatachannel: [https://github.com/paullouisageneau/libdatachannel](https://github.com/paullouisageneau/libdatachannel)
- [可选] QXmpp: [https://github.com/qxmpp-project/qxmpp](https://github.com/qxmpp-project/qxmpp)
- [可选] QtService: [https://github.com/KangLin/qt-solutions](https://github.com/KangLin/qt-solutions)

#### 玉兔公共库
此库默认放在与本项目同级目录下，如果没有在同级目录下，则必须指定 CMake 参数:
-DRabbitCommon_DIR=[RabbitCommon 安装目录]

    git clone https://github.com/KangLin/RabbitCommon.git

#### FreeRDP   
- 使用 vcpkg
  + 源码位置: https://github.com/microsoft/vcpkg/
  
        git clone https://github.com/microsoft/vcpkg.git
        cd vcpkg
        bootstrap-vcpkg.bat
        vcpkg install freerdp

  + 指定 CMake 参数：-DCMAKE_TOOLCHAIN_FILE=[vcpkg 安装目录]/scripts/buildsystems/vcpkg.cmake
  + 使用 vcpk ，不支持 FreeRDP 服务，所以需要从源码编译它。
- 从源码编译
  + 源码位置：https://github.com/FreeRDP/FreeRDP
  + 编译详见：https://github.com/FreeRDP/FreeRDP/wiki/Compilation

          git clone https://github.com/FreeRDP/FreeRDP.git
          cd FreeRDP
          mkdir build
          cmake .. -DCMAKE_INSTALL_PREIX=%CD%/install -DWITH_SERVER=ON
          cmake --build . --target install
          
  + 指定 CMake 参数：
    - -DBUILD_FREERDP=ON
    - -DWinPR_DIR=[freerdp 安装目录]/lib/cmake/WinPR2
    - -DFreeRDP_DIR=[freerdp 安装目录]/lib/cmake/FreeRDP2
    - -DFreeRDP-Client_DIR=[freerdp 安装目录]/lib/cmake/FreeRDP-Client2
  
#### LibVNCServer
- 从源码编译
  + 源码位置：[https://github.com/LibVNC/libvncserver](https://github.com/LibVNC/libvncserver)  
  建议使用补丁: https://github.com/KangLin/libvncserver
  
        cd vcpkg
        vcpkg install zlib openssl libjpeg-turbo 
        git clone https://github.com/KangLin/libvncserver.git
        cd libvncserver
        mkdir build
        cmake .. -DCMAKE_INSTALL_PREIX=%CD%/install -DCMAKE_TOOLCHAIN_FILE=[vcpkg 安装目录]/scripts/buildsystems/vcpkg.cmake
        cmake --build . --target install
  
  + 指定 CMake 参数：-DLibVNCServer_DIR=[LibVNCServer 安装目录]/lib/cmake/LibVNCServer
  
#### RabbitVNC
- 从源码编译
源码位置: https://github.com/KangLin/RabbitVNC  

      cd vcpkg
      vcpkg install zlib openssl libjpeg-turbo pixman
      git clone https://github.com/KangLin/RabbitVNC.git
      cd RabbitVNC
      mkdir build
      cmake .. -DCMAKE_INSTALL_PREIX=%CD%/install -DCMAKE_TOOLCHAIN_FILE=[vcpkg 安装目录]/scripts/buildsystems/vcpkg.cmake
      cmake --build . --target install

- 指定 CMake 参数：-DRabbitVNC_DIR=[RabbitVNC 安装目录]/lib/cmake/RabbitVNC

#### TigerVNC
- 从源码编译

官方只是个应用程序，不支持库。详见：https://github.com/TigerVNC/tigervnc/issues/1123  
所以本人在官方基础上做了修改。源码位置：https://github.com/KangLin/tigervnc  

    cd vcpkg
    vcpkg install zlib openssl pixman libjpeg-turbo
    git clone https://github.com/KangLin/tigervnc.git
    cd tigervnc
    mkdir build
    cmake .. -DCMAKE_INSTALL_PREIX=%CD%/install -DCMAKE_TOOLCHAIN_FILE=[vcpkg 安装目录]/scripts/buildsystems/vcpkg.cmake
    cmake --build . --target install
    
- 指定 CMake 参数：-Dtigervnc_DIR=[TigerVNC 安装目录]/lib/cmake/tigervnc
  
#### libdatachannel
- 使用 vcpkg
  + 源码位置: https://github.com/microsoft/vcpkg/
  
        cd vcpkg
        vcpkg install libdatachannel
        
  + 指定 CMake 参数：-DCMAKE_TOOLCHAIN_FILE=[vcpkg 安装目录]/scripts/buildsystems/vcpkg.cmake
- 从源码编译
  + 源码位置： [https://github.com/paullouisageneau/libdatachannel](https://github.com/paullouisageneau/libdatachannel)
  + 编译详见： [https://github.com/paullouisageneau/libdatachannel/blob/master/BUILDING.md](https://github.com/paullouisageneau/libdatachannel/blob/master/BUILDING.md)
  
        git clone https://github.com/paullouisageneau/libdatachannel.git
        cd libdatachannel
        git submodule update --init --recursive
        mkdir build
        cd build
        cmake .. -DCMAKE_INSTALL_PREIX=%CD%/install
        cmake --build . --target install

  + 指定 CMake 参数: -Dlibdatachannel_DIR=[libdatachannel 安装目录]/lib/cmake/LibDataChannel

#### QXmpp
- 从源码编译
  + 源码位置： [https://github.com/qxmpp-project/qxmpp](https://github.com/qxmpp-project/qxmpp)
  
        git clone https://github.com/qxmpp-project/qxmpp.git
        cd qxmpp
        mkdir build
        cd build
        cmake .. -DCMAKE_INSTALL_PREIX=%CD%/install -DQt5_DIR=[Qt 安装目录]/lib/cmake/Qt5
        cmake --build . --target install

  + 指定 CMake 参数: -DQXmpp_DIR=[QXmpp 安装目录]/lib/cmake/qxmpp
  
#### QTermWidget (暂不支持 Windows）     
- 从源码编译
  + 源码位置： [https://github.com/lxqt/qtermwidget](https://github.com/lxqt/qtermwidget)
  + 指定 CMake 参数：-Dqtermwidget5_DIR=[qtermwidget 安装目录]/lib/cmake/qtermwidget5

#### libssh
- 使用 vcpkg
  + 源码位置: https://github.com/microsoft/vcpkg/
  
        cd vcpkg
        vcpkg install libssh

  + 指定 CMake 参数：-DCMAKE_TOOLCHAIN_FILE=[vcpkg 安装目录]/scripts/buildsystems/vcpkg.cmake
- 从源码编译
  + 源码位置：[https://www.libssh.org](https://www.libssh.org)
  + 指定 CMake 参数：-Dlibssh_DIR=[libssh 安装目录]/lib/cmake/libssh

#### QtService
- 从源码编译
  + 源码位置：: https://github.com/KangLin/qt-solutions/
  
        ~$ git clone https://github.com/KangLin/qt-solutions.git
        ~$ cd qt-solutions
        ~/qt-solutions$ mkdir build
        ~/qt-solutions$ cd build
        ~/qt-solutions/build$ cmake .. -DCMAKE_INSTALL_PREIX=%CD%/install
        ~/qt-solutions/build$ cmake --build . --target install
        
  + 指定 CMake 参数: -DQtService_DIR=[QtService 安装目录]/lib/cmake/QtService

### 编译本项目
- 项目位置：[https://github.com/KangLin/RabbitRemoteControl](https://github.com/KangLin/RabbitRemoteControl)
- 下载源码

      git clone https://github.com/KangLin/RabbitRemoteControl.git

- CMake 参数
  + RabbitCommon_DIR：RabbitCommon 源码位置
  + BUILD_DOCS: 编译文档。默认为 OFF
  + BUILD_FREERDP：是否编译 FreeRDP。 默认为 OFF
  + BUILD_QUIWidget: 用无边框窗口做为主窗口。默认为 OFF
  + BUILD_SHARED_LIBS: 编译动态库。默认为 ON
  + WinPR_DIR:PATH: [FreeRDP 安装目录]/lib/cmake/WinPR2
  + FreeRDP_DIR: [FreeRDP 安装目录]/lib/cmake/FreeRDP2
  + FreeRDP-Client_DIR: [FreeRDP 安装目录]/lib/cmake/FreeRDP-Client2
  + RabbitVNC_DIR: [RabbitVNC 安装目录]/lib/cmake/RabbitVNC
  + tigervnc_DIR: [TigerVNC 安装目录]/lib/cmake/tigervnc
  + LibVNCServer_DIR: [libvncserver 安装目录]/lib/cmake/LibVNCServer
  + libdatachannel_DIR: [libdatachannel 安装目录]/lib/cmake/LibDataChannel
  + QXmpp_DIR=[QXmpp 安装目录]/lib/cmake/qxmpp
  + qtermwidget5_DIR: [QTermWidget 安装目录]/lib/cmake/qtermwidget5
  + libssh_DIR: [libssh 安装目录]/lib/cmake/libssh
  + QtService_DIR: [QtService 安装目录]/lib/cmake/QtService
  
- 如果使用 vcpkg，增加下面参数
  + CMAKE_TOOLCHAIN_FILE: [vcpkg 安装目录]/scripts/buildsystems/vcpkg.cmake
  
- 编译
  + 安装目标
    - install-runtime: 只安装运行库和程序
    - install: 安装所有库（运行库与开发库）和程序

  + 命令行编译
  
          cd RabbitRemoteControl
          mkdir build
          cmake .. -DCMAKE_INSTALL_PREIX=%CD%/install -DBUILD_FREERDP=ON [可选依赖库] -DCMAKE_TOOLCHAIN_FILE=[vcpkg 安装目录]/scripts/buildsystems/vcpkg.cmake
          cmake --build . --target install-runtime
          makensis Install.nsi  ;打包

  + IDE (Qt Creator) 编译
    - 设置 vcpkg: 选项→Kits→Cmake Configureration: 增加 CMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
    - 打开项目: 菜单→文件→打开文件或项目，选择项目根目录中的 CMakeLists.txt 
    - 配置：点左侧工具栏上的 项目→编译与运行，配置 CMake 参数
    - 编译与运行： 点左侧工具栏上的 “开始调试” 或者按快捷键 F5
