## Compilation on Windows
Author: Kang Lin <kl222@126.com>  
Translator: Allan Nordhøy <epost@anotheragency.no>

### Environment
#### Operating system 

    Windows 10, version 20H2 (19042.985)

#### QtCreator
Version: v4.15.0. \
It is recommended to use version v4.15.0 or later. \
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
    * Visual Studio Key：https://blog.csdn.net/kl222/article/details/84939135
    * Current version：VS 2017

- Windows SDK: https://developer.microsoft.com/en-us/windows/downloads/sdk-archive  
    Install the SDK that corresponds to your operating system. (CDB must be installed, and Qt debugging is required.)
- Windows Driver Kit: https://docs.microsoft.com/zh-cn/windows-hardware/drivers/download-the-wdk

- Qt
  + Qt (official release): https://download.qt.io/official_releases/qt/  
    Current version: Qt 5.12.11
  + IDE: Qt Creator. It is recommended to use version v4.15.0 or later. \
  Prior versions don't have CMake support.
- Git: [https://www.git-scm.com/](https://www.git-scm.com/)  

- CMake: [https://www.cmake.org/](https://cmake.org/)

- Doxygen: [http://www.doxygen.nl/](http://www.doxygen.nl/)

### Compilation

### Library dependencies

- [MUST] RabbitCommon: [https://github.com/KangLin/RabbitCommon](https://github.com/KangLin/RabbitCommon)
- [OPTIONAL] RFB
  + [OPTIONAL] LibVNCServer: [https://github.com/LibVNC/libvncserver](https://github.com/LibVNC/libvncserver)
  + [OPTIONAL] TigerVNC: https://github.com/KangLin/tigervnc
- [OPTIONAL] FreeRDP: [https://github.com/FreeRDP/FreeRDP](https://github.com/FreeRDP/FreeRDP)
- [OPTIONAL] SSH
  + libssh: [https://www.libssh.org](https://www.libssh.org)
  + libssh2: 
    - https://www.libssh2.org/  
    - https://github.com/libssh2/libssh2
- [OPTIONAL] QTermWidget: [https://github.com/lxqt/qtermwidget](https://github.com/lxqt/qtermwidget)
- [OPTIONAL] libtelnet: [https://github.com/seanmiddleditch/libtelnet](https://github.com/seanmiddleditch/libtelnet)
- [OPTIONAL] libdatachannel: [https://github.com/paullouisageneau/libdatachannel](https://github.com/paullouisageneau/libdatachannel)
- [OPTIONAL] QXmpp: [https://github.com/qxmpp-project/qxmpp](https://github.com/qxmpp-project/qxmpp)
- [OPTIONAL] QtService: https://github.com/KangLin/qt-solutions/

#### RabbitCommon
This library is placed in the same directory level as the project by default.
If not, you must specify the CMake parameters:
-DRabbitCommon_DIR=[RabbitCommon installation path]

    git clone https://github.com/KangLin/RabbitCommon.git

#### FreeRDP
- Use vcpkg
  + Source-code location: https://github.com/microsoft/vcpkg/
  
        git clone https://github.com/microsoft/vcpkg.git
        cd vcpkg
        bootstrap-vcpkg.bat
        vcpkg install freerdp

  + Specify the CMake parameters: -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake

- Compile from source code
  + Source-code location: https://github.com/FreeRDP/FreeRDP
  + Compilation instructions: https://github.com/FreeRDP/FreeRDP/wiki/Compilation
 
          git clone https://github.com/FreeRDP/FreeRDP.git
          cd FreeRDP
          mkdir build
          cmake .. -DCMAKE_INSTALL_PREIX=%CD%/install
          cmake --build . --target install

  + Specify the CMake parameters: 
    - -DBUILD_FREERDP=ON
    - -DFreeRDP-Client_DIR=[freerdp installation path]/lib/cmake/FreeRDP-Client2
    - -DFreeRDP_DIR=[freerdp installation path]/lib/cmake/FreeRDP2
    - -DWinPR_DIR=[freerdp installation path]/lib/cmake/WinPR2
  
#### LibVNCServer
- Compile from sorce code
  + Source-code location: [https://github.com/LibVNC/libvncserver](https://github.com/LibVNC/libvncserver)  
  It is recommended to use the patches from: https://github.com/KangLin/libvncserver
  
        cd vcpkg
        vcpkg install zlib openssl libjpeg-turbo
        git clone https://github.com/KangLin/libvncserver.git
        cd libvncserver
        mkdir build
        cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
        cmake --build . --target install

  + Specify the CMake parameters: -DLibVNCServer_DIR=[LibVNCServer installation path]/lib/cmake/LibVNCServer
  
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
    cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
    cmake --build . --target install

- Specify the CMake parameters: -Dtigervnc_DIR=[TigerVNC installation path]/lib/cmake

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
        cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install
        cmake --build . --target install

  + Specify the CMake parameters: -Dlibdatachannel_DIR=[libdatachannel installation path]/share/cmake/libdatachannel

#### QXmpp
- Compile from source code
  + Source-code location: [https://github.com/qxmpp-project/qxmpp](https://github.com/qxmpp-project/qxmpp)
  
        git clone https://github.com/qxmpp-project/qxmpp.git
        cd qxmpp
        mkdir build
        cd build
        cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install -DQt5_DIR=[Qt install path]/lib/cmake/Qt5
        cmake --build . --target install

  + Specify the CMake parameters: -DQXmpp_DIR=[libdatachannel installation path]/lib/cmake/qxmpp
  
#### qtermwidget (Not currently supported on Windows)
- Compile from source code
  + Source-code location:  [https://github.com/lxqt/qtermwidget](https://github.com/lxqt/qtermwidget)
  + Specify the CMake parameters: -Dqtermwidget5_DIR=[qtermwidget installation path]/lib/cmake/qtermwidget5

#### libssh
- Use vcpkg
  + Source-code location: https://github.com/microsoft/vcpkg/
  
        cd vcpkg
        vcpkg install libssh

  + Specify the CMake parameters: -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
- Compile from source code
  + Source code location:  [https://www.libssh.org](https://www.libssh.org)
  + Specify the CMake parameters: -Dlibssh_DIR=[libssh installation path]/lib/cmake/libssh

#### QtService
- Compile from source code
  + Source-code location: https://github.com/KangLin/qt-solutions/
  
        ~$ git clone https://github.com/KangLin/qt-solutions.git
        ~$ cd qt-solutions
        ~/qt-solutions$ mkdir build
        ~/qt-solutions$ cd build
        ~/qt-solutions/build$ cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install
        ~/qt-solutions/build$ cmake --build . --target install
        
  + Specify the CMake parameters: -DQtService_DIR=[libssh installation path]/lib/cmake/QtService

### Compile this project
- Project location: [https://github.com/KangLin/RabbitRemoteControl](https://github.com/KangLin/RabbitRemoteControl)
- Download the source code:

      git clone https://github.com/KangLin/RabbitRemoteControl.git

- The CMake parameters:
  + RabbitCommon_DIR: RabbitCommon source-code location
  + BUILD_FREERDP：If compiling FreeRDP
  + WinPR_DIR: [FreeRDP installation path]/lib/cmake/WinPR2
  + FreeRDP_DIR: [FreeRDP installation path]/lib/cmake/FreeRDP2
  + FreeRDP-Client_DIR: [FreeFRP installation path]/lib/cmake/FreeRDP-Client2
  + TigerVNC_DIR: [TigerVNC installation path]/lib/cmake
  + LibVNCServer_DIR: [libvncserver installation path]/lib/cmake/LibVNCServer
  + libdatachannel_DIR: [libdatachannel installation path]/share/cmake/libdatachannel
  + QXmpp_DIR=[libdatachannel installation path]/lib/cmake/qxmpp
  + QTermWidget_5_DIR: [QTermWidget installation path]/lib/cmake/qtermwidget5
  + libssh_DIR: [libssh installation path]/lib/cmake/libssh
  + QtService_DIR: [libssh installation path]/lib/cmake/QtService
  
- If using vcpkg, please set the CMake parameters:
  + CMAKE_TOOLCHAIN_FILE: [vcpkg installation path]/scripts/buildsystems/vcpkg.cmake

- Compile
  + Install target
    - install-runtime: Only install runtime libraries and the program
    - install: Install runtime and development libraries and the program

  + Compile from the command-line

          cd RabbitRemoteControl
          mkdir build
          cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
          cmake --build . --target install-runtime
          makensis Install.nsi  ;Package

  + Using an IDE (Qt Creator)
    - Set vcpkg: Menu→Options→Kits→Cmake Configuration: add CMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
    - Open project: Menu→File→Open File or project, Select the CMakeLists.txt of the project
    - Configure: Click Project→"Build & Run" in the toolbar on the left to configure CMake parameters
    - Compile and run: Click "Start Debugging of startup project" in the left toolbar, or press the shortcut key (F5)
