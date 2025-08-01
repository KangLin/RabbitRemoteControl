## Compiling for Windows

Author: Kang Lin <kl222@126.com>  
Translator: Allan Nordhøy <epost@anotheragency.no>

### Environment
#### Operating system 

    Windows 10, version 20H2 (19042.985)

#### QtCreator

Version: v14.0.2
It is recommended to use version v5.0.2 or later.  
Prior versions don't have CMake support.

### Tools

- Compiler
  + Visual Studio
    * Home
      - https://visualstudio.microsoft.com/vs/
      - http://msdn.microsoft.com/zh-cn/vstudio 
      - https://visualstudio.microsoft.com/zh-hans/downloads/
    * Version：
      - Visual Studio 2013
      - Visual Studio 2015
      - Visual Studio 2017
      - Visual Studio 2019
      - Visual Studio 2022
    * Visual Studio Key：https://github.com/KangLin/Documents/blob/master/c/VisualStudioKey.md
    * Current version：VS 2017
- Windows SDK: https://developer.microsoft.com/en-us/windows/downloads/sdk-archive  
    Install the SDK that corresponds to your operating system. (CDB must be installed, and Qt debugging is required.)
- [OPTIONAL] Windows Driver Kit: https://docs.microsoft.com/zh-cn/windows-hardware/drivers/download-the-wdk
- DirectX: https://support.microsoft.com/zh-cn/topic/%E5%A6%82%E4%BD%95%E5%AE%89%E8%A3%85%E6%9C%80%E6%96%B0%E7%89%88%E6%9C%AC%E7%9A%84-directx-d1f5ffa5-dae2-246c-91b1-ee1e973ed8c2  
  Required to run Qt.
- Qt
  - Qt (official release): https://download.qt.io/official_releases/qt/  
    Current version: Qt 6.8.2
    - Set environment variable or cmake parameters: QT_ROOT, Qt6_DIR or Qt6_DIR
      - Environment variable

            export QT_ROOT=Qt install root
            # When Qt6
            export Qt6_DIR=$QT_ROOT
            # When Qt5
            export Qt5_DIR=$QT_ROOT

      - CMAKE parameters

            # Qt6
            cmake -DQT_ROOT=[Qt install root] -DQt6_DIR=[Qt install root] ......
            # Qt5 
            cmake -DQT_ROOT=[Qt install root] -DQt5_DIR=[Qt install root] ......
    
  - IDE: Qt Creator. It is recommended to use version v5.0.2 or later.  
    Current version: 14.0.2
  Prior versions don't have CMake support.
- Git: [https://www.git-scm.com/](https://www.git-scm.com/)  
- CMake: [https://www.cmake.org/](https://cmake.org/)
  Version: 3.27.0
- Doxygen: [http://www.doxygen.nl/](http://www.doxygen.nl/)
- Nsis: [https://nsis.sourceforge.io/Download](https://nsis.sourceforge.io/Download)
- vcpkg: [https://github.com/microsoft/vcpkg](https://github.com/microsoft/vcpkg)

### Compilation

See: [Compile integration](../../.github/workflows/msvc.yml)

### Library dependencies

- [MUST] RabbitCommon: [https://github.com/KangLin/RabbitCommon](https://github.com/KangLin/RabbitCommon)
- [MUST] [Openssl:](https://github.com/openssl/openssl) Required to run Qt.
- [OPTIONAL] RFB
  + [OPTIONAL] RabbitVNC: [https://github.com/KangLin/RabbitVNC](https://github.com/KangLin/RabbitVNC)
  + [OPTIONAL] LibVNCServer: [https://github.com/LibVNC/libvncserver](https://github.com/LibVNC/libvncserver)
  + [OPTIONAL] TigerVNC: [https://github.com/KangLin/tigervnc](https://github.com/KangLin/tigervnc)
- [OPTIONAL] FreeRDP: [https://github.com/FreeRDP/FreeRDP](https://github.com/FreeRDP/FreeRDP)
- [OPTIONAL] SSH
  + libssh: 
    - [https://www.libssh.org](https://www.libssh.org)
    - [libssh API](https://api.libssh.org/stable/index.html)
  + libssh2: 
    - https://www.libssh2.org/  
    - https://github.com/libssh2/libssh2
- [OPTIONAL] QTermWidget: [https://github.com/lxqt/qtermwidget](https://github.com/lxqt/qtermwidget)
- [OPTIONAL] QtSsh: [https://github.com/condo4/QtSsh.git](https://github.com/condo4/QtSsh.git)
- [OPTIONAL] QTelnet: [https://github.com/silderan/QTelnet.git](https://github.com/silderan/QTelnet.git)
- [OPTIONAL] libtelnet: [https://github.com/seanmiddleditch/libtelnet](https://github.com/seanmiddleditch/libtelnet)
- [OPTIONAL] libdatachannel: [https://github.com/paullouisageneau/libdatachannel](https://github.com/paullouisageneau/libdatachannel)
- [OPTIONAL] QXmpp: [https://github.com/qxmpp-project/qxmpp](https://github.com/qxmpp-project/qxmpp)
- [OPTIONAL] QtService: https://github.com/KangLin/qt-solutions/
- [OPTIONAL] PcapPlusPlus: [https://github.com/seladb/PcapPlusPlus](https://github.com/seladb/PcapPlusPlus)
- [OPTIONAL] [FFMPEG:](https://ffmpeg.org/) Multimedia capabilities required

#### RabbitCommon

- This library is placed in the same directory level as the project by default.

      git clone https://github.com/KangLin/RabbitRemoteControl.git
      git clone https://github.com/KangLin/RabbitCommon.git

- If not, you must specify the CMake parameters or environment variable:

      git clone https://github.com/KangLin/RabbitRemoteControl.git
      ; Set environment variable
      set RabbitCommon_ROOT=[RabbitCommon install root]
      cd RabbitRemoteControl
      ; Or set CMake parameters
      cmake -DRabbitCommon_ROOT=[RabbitCommon installation path] ......

#### FreeRDP

- Use vcpkg
  + Source-code location: https://github.com/microsoft/vcpkg/
  
        git clone https://github.com/microsoft/vcpkg.git
        cd vcpkg
        bootstrap-vcpkg.bat
        vcpkg install freerdp

  + Specify the CMake parameters: -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
  + With vcpk, the FreeRDP service is not a default feature,
    so you need to specify it manually.
- Compile from source code
  + Source-code location: https://github.com/FreeRDP/FreeRDP
  + Compilation instructions: https://github.com/FreeRDP/FreeRDP/wiki/Compilation
 
          git clone https://github.com/FreeRDP/FreeRDP.git
          cd FreeRDP
          mkdir build
          cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%CD%/install -DWITH_SERVER=ON
          cmake --build . --config Release --target install

- When FreeRDP is compiled from source,
  the CMake parameter needs to be specified to compile this project:
  - -DBUILD_FREERDP=ON
  - -DFreeRDP-Client_DIR=[freerdp installation path]/lib/cmake/FreeRDP-Client3
  - -DFreeRDP_DIR=[freerdp installation path]/lib/cmake/FreeRDP3
  - -DWinPR_DIR=[freerdp installation path]/lib/cmake/WinPR3
  - -DFreeRDP-Shadow_DIR=[freerdp installation path]/lib/cmake/FreeRDP-Shadow3
  - -DFreeRDP-Server_DIR=[freerdp installation path]/lib/cmake/FreeRDP-Server3

#### LibVNCServer

- Compile from source code
  + Source-code location: [https://github.com/LibVNC/libvncserver](https://github.com/LibVNC/libvncserver)  
  It is recommended to use the patches from: https://github.com/KangLin/libvncserver
  
        cd vcpkg
        vcpkg install zlib openssl libjpeg-turbo
        git clone https://github.com/KangLin/libvncserver.git
        cd libvncserver
        mkdir build
        cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%CD%/install -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
        cmake --build . --config Release --target install

- When LibVNCServer is compiled from source,
  the CMake parameter needs to be specified to compile this project:

      -DLibVNCServer_DIR=[LibVNCServer installation path]/lib/cmake/LibVNCServer
  
#### RabbitVNC

- Compile from source code
  - Source-code location: https://github.com/KangLin/RabbitVNC  

        cd vcpkg
        vcpkg install zlib openssl libjpeg-turbo pixman
        git clone https://github.com/KangLin/RabbitVNC.git
        cd RabbitVNC
        mkdir build
        cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%CD%/install -DCMAKE_TOOLCHAIN_FILE=[vcpkg 安装目录]/scripts/buildsystems/vcpkg.cmake
        cmake --build . --config Release --target install
    
- When RabbitVNC is compiled from source,
  the CMake parameter needs to be specified to compile this project:

      -DRabbitVNC_DIR=[RabbitVNC installation path]/lib/cmake/tigervnc

#### TigerVNC

- Compile from source code

The official program does not support libraries.  
See: https://github.com/TigerVNC/tigervnc/issues/1123  
The Kanglin fork has support.  
Source-code location: https://github.com/KangLin/tigervnc  

    cd vcpkg
    vcpkg install zlib openssl pixman libjpeg-turbo
    git clone https://github.com/KangLin/tigervnc.git
    cd tigervnc
    mkdir build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%CD%/install -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
    cmake --build . --config Release --target install

- When TigerVNC is compiled from source,
  the CMake parameter needs to be specified to compile this project:

      -Dtigervnc_DIR=[TigerVNC installation path]/lib/cmake/tigervnc

#### libdatachannel

- Use vcpkg
  + Source-code location: https://github.com/microsoft/vcpkg/
  
        cd vcpkg
        vcpkg install libdatachannel

  + Specify the CMake parameters: -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake

- Compile from source code
  + Source-code location:  [https://github.com/paullouisageneau/libdatachannel](https://github.com/paullouisageneau/libdatachannel)
  + Compilation instructions: [https://github.com/paullouisageneau/libdatachannel/blob/master/BUILDING.md](https://github.com/paullouisageneau/libdatachannel/blob/master/BUILDING.md)
 
        git clone https://github.com/paullouisageneau/libdatachannel.git
        cd libdatachannel
        git submodule update --init --recursive
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%CD%/install
        cmake --build . --config Release --target install

- When libdatachannel is compiled from source,
  the CMake parameter needs to be specified to compile this project:

      -DLibDataChannel_DIR=[libdatachannel installation path]/lib/cmake/LibDataChannel

#### QXmpp

- Compile from source code
  + Source-code location: [https://github.com/qxmpp-project/qxmpp](https://github.com/qxmpp-project/qxmpp)
  
        git clone https://github.com/qxmpp-project/qxmpp.git
        cd qxmpp
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%CD%/install -DQt5_DIR=[Qt install path]/lib/cmake/Qt5
        cmake --build . --config Release --target install

- When QXmpp is compiled from source,
  the CMake parameter needs to be specified to compile this project:

      -DQXmpp_DIR=[QXmpp installation path]/lib/cmake/qxmpp
  
#### QTermWidget (Not currently supported on Windows)
- Compile from source code
  + Source-code location:  [https://github.com/lxqt/qtermwidget](https://github.com/lxqt/qtermwidget)
- When QTermWidget is compiled from source,
  the CMake parameter needs to be specified to compile this project:

      -Dqtermwidget5_DIR=[qtermwidget installation path]/lib/cmake/qtermwidget5

#### libssh

- Use vcpkg
  + Source-code location: https://github.com/microsoft/vcpkg/
  
        cd vcpkg
        vcpkg install libssh

  + Specify the CMake parameters: -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
- Compile from source code
  + Source code location:  [https://www.libssh.org](https://www.libssh.org)
- When libssh is compiled from source,
  the CMake parameter needs to be specified to compile this project:

      -Dlibssh_DIR=[libssh installation path]/lib/cmake/libssh

#### QtService

- Compile from source code
  + Source-code location: https://github.com/KangLin/qt-solutions/
  
        git clone https://github.com/KangLin/qt-solutions.git
        cd qt-solutions
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%CD%/install
        cmake --build . --config Release --target install
        
- When QtService is compiled from source,
  the CMake parameter needs to be specified to compile this project:

      -DQtService_DIR=[QtService installation path]/lib/cmake/QtService

### PcapPlusPlus

- Compile from source code
  + Source-code location: https://github.com/seladb/PcapPlusPlus

        git clone https://github.com/seladb/PcapPlusPlus.git

- Use vcpkg
  + Source-code location: https://github.com/microsoft/vcpkg/

        git clone https://github.com/microsoft/vcpkg.git
        cd vcpkg
        vcpkg install pcapplusplus

**Note**: PcapPlusPlus in vcpkg relies on WinPcap,
but WinPcap does not work in Windows 10.
So compile from source, relying on the npcap library.

- When PcapPlusPlus is compiled from source,
  the CMake parameter needs to be specified to compile this project:

      -DPcapPlusPlus_DIR=[PcapPlusPlus install path]/lib/cmake/pcapplusplus

### Compile this project

- Project location: [https://github.com/KangLin/RabbitRemoteControl](https://github.com/KangLin/RabbitRemoteControl)
- Download the source code:

      git clone https://github.com/KangLin/RabbitRemoteControl.git

- The CMake parameters:
  + QT_ROOT: Qt install root
  + Qt6_DIR or Qt5_DIR: Is same QT_ROOT
  + RabbitCommon_ROOT: RabbitCommon source-code location
  + BUILD_CLIENT: Compile client. the default is ON
  + BUILD_SERVICE: Compile service. the default dependency is whether there is a QtService
  + BUILD_PLUGINS: Compile plugins. the default is ON
  + BUILD_APP: Compile applaction. the default is ON
  + BUILD_DOCS: Build docmenets. the default is OFF
  + BUILD_SHARED_LIBS: Compile shared libraries. the default is ON
  + BUILD_FREERDP：If compile FreeRDP. the default is OFF
  + WinPR_DIR: [FreeRDP installation path]/lib/cmake/WinPR2
  + FreeRDP_DIR: [FreeRDP installation path]/lib/cmake/FreeRDP2
  + FreeRDP-Client_DIR: [FreeFRP installation path]/lib/cmake/FreeRDP-Client2
  + BUILD_RABBITVNC: Compile RabbitVNC. the default is ON
  + RabbitVNC_DIR: [RabbitVNC installation path]/lib/cmake/RabbitVNC
  + BUILD_TigerVNC: Compile TigerVNC. the default is ON
  + TigerVNC_DIR: [TigerVNC installation path]/lib/cmake/tigervnc
  + BUILD_LibVNCServer: Compile LibVNCServer. the default is ON
  + LibVNCServer_DIR: [libvncserver installation path]/lib/cmake/LibVNCServer
  + libdatachannel_DIR: [libdatachannel installation path]/lib/cmake/LibDataChannel
  + QXmpp_DIR=[QXmpp installation path]/lib/cmake/qxmpp
  + QTermWidget5_DIR: [QTermWidget installation path]/lib/cmake/qtermwidget5
  + libssh_DIR: [libssh installation path]/lib/cmake/libssh
  + QtService_DIR: [QtService installation path]/lib/cmake/QtService
- If using vcpkg, please set the CMake parameters:
  + Because of using vcpkg manifest mode, so the depend libraries in `vcpkg.json`
  + CMAKE_TOOLCHAIN_FILE: [vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
  + X_VCPKG_APPLOCAL_DEPS_INSTALL: ON #When installing, copy the dependent libraries to the installation directory
  + VCPKG_MANIFEST_FEATURES: This variable can be set to a list of features to activate when installing from your manifest.
      Note: All vcpkg-affecting variables must be defined before the first project() directive such as in a CMakePresets.json's "cacheVariables" map, via the command line, or set() statements.
      So don't place it in CMakeLists.txt of the project.
      See: https://learn.microsoft.com/vcpkg/users/buildsystems/cmake-integration#settings-reference
- Compile
  + Compile from the command-line

          cd RabbitRemoteControl
          mkdir build
          cmake .. -DCMAKE_BUILD_TYPE=Release ^
              -DCMAKE_INSTALL_PREFIX=%CD%/install ^
              -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake ^
              -DX_VCPKG_APPLOCAL_DEPS_INSTALL=ON ^
              -DBUILD_FREERDP=ON [options libraries]
          cmake --build . --config Release --target install

          ; Package
          cmake --install . --config Release --component Runtime --strip
          cmake --install . --config Release --component Application --strip
          cmake --install . --config Release --component Plugin --strip
          cmake --install . --config Release --component DependLibraries --strip
          makensis Windows.nsi

  + Using an IDE (Qt Creator)
    - Set vcpkg: Menu→Edit→Preferences→Kits→Cmake Configuration:
      + Add CMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
      + Set X_VCPKG_APPLOCAL_DEPS_INSTALL=ON
    - Open project: Menu→File→Open File or project, Select the CMakeLists.txt of the project
    - Configure: Click Project→"Build & Run" in the toolbar on the left to configure CMake parameters
      - If need install: select install in target
    - Compile and run: Click "Start Debugging of startup project" in the left toolbar, or press the shortcut key (F5)

**Note:** If the plugin does not load.
It may be that the plugin's dependencies are not installed on the system.
You can copy the dependency libraries into the application's bin directory.

See: [Compile integration](../../.github/workflows/msvc.yml)
