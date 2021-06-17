## Compile on Windows
Author: Kang Lin (kl222@126.com)

### Environment
#### Operating system 

    Windows 10

#### QtCreator
Version: v4.15.0. It is recommended to use the version later than v4.15.0.
The previous version does not have enough support for cmake.

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
    * Current version：vs 2017
    
- Windows sdk: https://developer.microsoft.com/en-us/windows/downloads/sdk-archive  
    Install the corresponding SDK according to your operating system. CDB must be installed, and QT debugging is required. 
- Windows Driver Kit: https://docs.microsoft.com/zh-cn/windows-hardware/drivers/download-the-wdk
   
- Qt
  + Qt office release: https://download.qt.io/official_releases/qt/  
    Current version: Qt5.12.11
  + IDE: QtCreator. It is recommended to use the version later than v4.15.0.
    The previous version does not have enough support for cmake.
  
- git: [http://www.git-scm.com/](http://www.git-scm.com/)  
  
- cmake: [http://www.cmake.org/](http://www.cmake.org/)

### Compile

### Dependent libraries

- [Must] RabbitCommon: [https://github.com/KangLin/RabbitCommon](https://github.com/KangLin/RabbitCommon)
- [Optional] RFB
  + [Optional] libvncserver: [https://github.com/LibVNC/libvncserver](https://github.com/LibVNC/libvncserver)
  + [Optional] TigerVnc: https://github.com/KangLin/tigervnc
- [Optional] FreeRDP: [https://github.com/FreeRDP/FreeRDP](https://github.com/FreeRDP/FreeRDP)
- [Optional] [SSH]
  + LIBSSH: [https://www.libssh.org](https://www.libssh.org)
  + LIBSSH2: https://www.libssh2.org/
                 https://github.com/libssh2/libssh2
- [Optional] qtermwidget: [https://github.com/lxqt/qtermwidget](https://github.com/lxqt/qtermwidget)
- [Optional] libtelnet: [https://github.com/seanmiddleditch/libtelnet](https://github.com/seanmiddleditch/libtelnet)
- [Optional] libdatachannel: [https://github.com/paullouisageneau/libdatachannel](https://github.com/paullouisageneau/libdatachannel)

#### RabbitCommon
This library is placed in the same level directory as the project by default.
If it is not in the same level directory, you must specify the CMake parameters:
-DRabbitCommon_DIR=[RabbitCommon installation path]

    git clone https://github.com/KangLin/RabbitCommon.git

#### FreeRDP
- Use vcpkg
  + Source code location: https://github.com/microsoft/vcpkg/
  + Specify CMake parameters: -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake

        git clone https://github.com/microsoft/vcpkg.git
        cd vcpkg
        bootstrap-vcpkg.bat
        vcpkg install freerdp

- Compile from sorce code
  + Source code location: https://github.com/FreeRDP/FreeRDP
  + Compile see: https://github.com/FreeRDP/FreeRDP/wiki/Compilation
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
  + Source code location: [https://github.com/LibVNC/libvncserver](https://github.com/LibVNC/libvncserver)  
  It is recommended to use patches: https://github.com/KangLin/libvncserver
  + Specify CMake parameters: -Dvncclient_DIR=[libvncserver installation path]/lib/cmake/LibVncServer

        cd vcpkg
        vcpkg install zlib openssl libjpeg-turbo
        git clone https://github.com/KangLin/libvncserver.git
        cd libvncserver
        mkdir build
        cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
        cmake --build . --target install

#### TigerVnc
The official is just an application and does not support libraries.  
See: https://github.com/TigerVNC/tigervnc/issues/1123  
So I made changes on the official basis.  
Source code location: https://github.com/KangLin/tigervnc  
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

      vcpkg install libdatachannel

- Compile from sorce code
  + Source code location:  [https://github.com/paullouisageneau/libdatachannel](https://github.com/paullouisageneau/libdatachannel)
  + Compile see: [https://github.com/paullouisageneau/libdatachannel/blob/master/BUILDING.md](https://github.com/paullouisageneau/libdatachannel/blob/master/BUILDING.md)
  + Specify CMake parameters: -DLibDataChannel_DIR=[libdatachannel installation path]/share/cmake/libdatachannel

        git clone https://github.com/paullouisageneau/libdatachannel.git
        cd libdatachannel
        git submodule update --init --recursive
        mkdir build
        cd build
        cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install
        cmake --build . --target install

#### qtermwidget(Not currently supported on Windows)
- Compile from sorce code
  + Source code location:  [https://github.com/lxqt/qtermwidget](https://github.com/lxqt/qtermwidget)
  + Specify CMake parameters: -Dqtermwidget5_DIR=[qtermwidget installation path]/lib/cmake/qtermwidget5

#### libssh
- Use vcpkg

      vcpkg install libssh

- Compile from sorce code
  + Source code location:  [https://www.libssh.org](https://www.libssh.org)
  + Specify CMake parameters: -Dlibssh_DIR=[libssh installation path]/lib/cmake/libssh

### Compile this project
- Project location: [https://github.com/KangLin/RabbitRemoteControl](https://github.com/KangLin/RabbitRemoteControl)
- Download source code:

      git clone https://github.com/KangLin/RabbitRemoteControl.git

- Specify CMake parameters:
  + RabbitCommon_DIR: RabbitCommon source code location
  + BUILD_FREERDP：If compile FreeRDP
  + FreeRDP-Client_DIR: [freerdp installation path]/lib/cmake/FreeRDP-Client2
  + FreeRDP_DIR: [freerdp installation path]/lib/cmake/FreeRDP2
  + WinPR_DIR: [freerdp installation path]/lib/cmake/WinPR2
  + tigervnc_DIR: [TigerVNC installation path]/lib/cmake
  + vncclient_DIR: [libvncserver installation path]/lib/cmake/LibVncServer
  + LibDataChannel_DIR: [libdatachannel installation path]/share/cmake/libdatachannel
  + qtermwidget5_DIR: [qtermwidget installation path]/lib/cmake/qtermwidget5
  + libssh_DIR: [libssh installation path]/lib/cmake/libssh

- Compile
  + Install target
    - install-runtime: Only install runtime libraries and program
    - install: Install runtime and development libraries and program

  + Compile in command line

          cd RabbitRemoteControl
          mkdir build
          cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
          cmake --build . --target install-runtime
          makensis Install.nsi  ;Package

  + Used by IDE(QtCreator)
    - Open project: Menu->File->Open File or project, Select CMakeLists.txt of the project
    - Configure：Click Project->Build&Run on the toolbar on the left to configure CMake parameters
    - Compile and run: Click "Start Debugging of startup project" on the left toolbar or press the shortcut key F5
