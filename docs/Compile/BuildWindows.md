## Compilation on Windows
Author: Kang Lin (kl222@126.com)

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

### Compilation

### Library dependencies

- [Must] RabbitCommon: [https://github.com/KangLin/RabbitCommon](https://github.com/KangLin/RabbitCommon)
- [Optional] RFB
  + [Optional] libvncserver: [https://github.com/LibVNC/libvncserver](https://github.com/LibVNC/libvncserver)
  + [Optional] TigerVNC: https://github.com/KangLin/tigervnc
- [Optional] FreeRDP: [https://github.com/FreeRDP/FreeRDP](https://github.com/FreeRDP/FreeRDP)
- [Optional] SSH
  + libssh: [https://www.libssh.org](https://www.libssh.org)
  + libssh2: https://www.libssh2.org/
                 https://github.com/libssh2/libssh2
- [Optional] QTermWidget: [https://github.com/lxqt/qtermwidget](https://github.com/lxqt/qtermwidget)
- [Optional] libtelnet: [https://github.com/seanmiddleditch/libtelnet](https://github.com/seanmiddleditch/libtelnet)
- [Optional] libdatachannel: [https://github.com/paullouisageneau/libdatachannel](https://github.com/paullouisageneau/libdatachannel)

#### RabbitCommon
This library is placed in the same directory level as the project by default.
If not, you must specify the CMake parameters:
-DRabbitCommon_DIR=[RabbitCommon installation path]

    git clone https://github.com/KangLin/RabbitCommon.git

#### FreeRDP
- Use vcpkg
  + Source-code location: https://github.com/microsoft/vcpkg/
  + Specify CMake parameters: -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake

        git clone https://github.com/microsoft/vcpkg.git
        cd vcpkg
        bootstrap-vcpkg.bat
        vcpkg install freerdp

- Compile from source code
  + Source-code location: https://github.com/FreeRDP/FreeRDP
  + Compilation instructions: https://github.com/FreeRDP/FreeRDP/wiki/Compilation
  + Specify CMake parameters: 
    - -DBUILD_FREERDP=ON
    - -DFreeRDP-Client_DIR=[freerdp installation path]/lib/cmake/FreeRDP-Client2
    - -DFreeRDP_DIR=[freerdp installation path]/lib/cmake/FreeRDP2
    - -DWinPR_DIR=[freerdp installation path]/lib/cmake/WinPR2

          git clone https://github.com/FreeRDP/FreeRDP.git
          cd FreeRDP
          mkdir build
          cmake .. -DCMAKE_INSTALL_PREIX=%CD%/install
          cmake --build . --target install

#### libvncserver
- Compile from sorce code
  + Source-code location: [https://github.com/LibVNC/libvncserver](https://github.com/LibVNC/libvncserver)  
  It is recommended to use the patches from: https://github.com/KangLin/libvncserver
  + Specify CMake parameters: -Dvncclient_DIR=[libvncserver installation path]/lib/cmake/LibVncServer

        cd vcpkg
        vcpkg install zlib openssl libjpeg-turbo
        git clone https://github.com/KangLin/libvncserver.git
        cd libvncserver
        mkdir build
        cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
        cmake --build . --target install

#### TigerVNC
The official program does not support libraries.  
See: https://github.com/TigerVNC/tigervnc/issues/1123  
The Kanglin fork has support.  
Source-code location: https://github.com/KangLin/tigervnc  
Specify CMake parameters: -Dtigervnc_DIR=[TigerVNC installation path]/lib/cmake

    cd vcpkg
    vcpkg install zlib openssl pixman libjpeg-turbo
    git clone https://github.com/KangLin/tigervnc.git
    cd tigervnc
    mkdir build
    cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
    cmake --build . --target install

#### libdatachannel
- Use vcpkg
  + Source-code location: https://github.com/microsoft/vcpkg/
  + Specify CMake parameters: -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake

        cd vcpkg
        vcpkg install libdatachannel

- Compile from source code
  + Source-code location:  [https://github.com/paullouisageneau/libdatachannel](https://github.com/paullouisageneau/libdatachannel)
  + Compilation instructions: [https://github.com/paullouisageneau/libdatachannel/blob/master/BUILDING.md](https://github.com/paullouisageneau/libdatachannel/blob/master/BUILDING.md)
  + Specify CMake parameters: -DLibDataChannel_DIR=[libdatachannel installation path]/share/cmake/libdatachannel

        git clone https://github.com/paullouisageneau/libdatachannel.git
        cd libdatachannel
        git submodule update --init --recursive
        mkdir build
        cd build
        cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install
        cmake --build . --target install

#### qtermwidget (Not currently supported on Windows)
- Compile from source code
  + Source-code location:  [https://github.com/lxqt/qtermwidget](https://github.com/lxqt/qtermwidget)
  + Specify CMake parameters: -Dqtermwidget5_DIR=[qtermwidget installation path]/lib/cmake/qtermwidget5

#### libssh
- Use vcpkg
  + Source-code location: https://github.com/microsoft/vcpkg/
  + Specify CMake parameters: -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake

        cd vcpkg
        vcpkg install libssh

- Compile from source code
  + Source code location:  [https://www.libssh.org](https://www.libssh.org)
  + Specify CMake parameters: -Dlibssh_DIR=[libssh installation path]/lib/cmake/libssh

### Compile this project
- Project location: [https://github.com/KangLin/RabbitRemoteControl](https://github.com/KangLin/RabbitRemoteControl)
- Download the source code:

      git clone https://github.com/KangLin/RabbitRemoteControl.git

- CMake parameters:
  + RabbitCommon_DIR: RabbitCommon source-code location
  + BUILD_FREERDP：If compiling FreeRDP
  + WinPR_DIR: [FreeRDP installation path]/lib/cmake/WinPR2
  + FreeRDP_DIR: [FreeRDP installation path]/lib/cmake/FreeRDP2
  + FreeRDP-Client_DIR: [FreeFRP installation path]/lib/cmake/FreeRDP-Client2
  + TigerVNC_DIR: [TigerVNC installation path]/lib/cmake
  + vncclient_DIR: [libvncserver installation path]/lib/cmake/LibVncServer
  + LibDataChannel_DIR: [libdatachannel installation path]/share/cmake/libdatachannel
  + QTermWidget_5_DIR: [QTermWidget installation path]/lib/cmake/qtermwidget5
  + libssh_DIR: [libssh installation path]/lib/cmake/libssh
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
    - Set vcpkg: Menu→Options→Kits→Cmake Configuration: add MAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
    - Open project: Menu→File→Open File or project, Select the CMakeLists.txt of the project
    - Configure: Click Project→"Build & Run" in the toolbar on the left to configure CMake parameters
    - Compile and run: Click "Start Debugging of startup project" in the left toolbar, or press the shortcut key (F5)
