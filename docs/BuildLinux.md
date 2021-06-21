## Compilation on Linux
Author Kang Lin (kl222@126.com)

### Environment
#### Operating system 

    ~$ lsb_release -a
    No LSB modules are available.
    Distributor ID:	Ubuntu
    Description:	Ubuntu 20.04.2 LTS
    Release:	20.04
    Codename:	focal

#### QtCreator
Version: v4.15.0. \
It is recommended to use version v4.15.0 or later. \
Prior versions don't have CMake support.

### Tools

- Compiler
  + GCC/g++
  
        ~$ sudo apt install g++ gcc
        
- Qt
  + System build-in:
  
        ~$ sudo apt install qttools5-dev qttools5-dev-tools qtbase5-dev qtbase5-dev-tools qtmultimedia5-dev qtlocation5-dev libqt5svg5-dev libqtermwidget5-0-dev
        
  + Qt (official release): https://download.qt.io/official_releases/qt/  
    Current version: Qt 5.12.11
  + IDE: Qt Creator. It is recommended to use version v4.15.0 or later. \
  Prior versions don't have CMake support.

        ~$ sudo apt install sudo apt install qtcreator
  
- Git: [http://www.git-scm.com/](http://www.git-scm.com/)

        ~$ sudo apt install git
        
- CMake: [http://www.cmake.org/](http://www.cmake.org/)

        ~$ sudo apt install cmake
        
- automake, autoconf, make

        ~$ sudo apt install automake autoconf make

### Compilation

    # Install library dependencies
    ~$ sudo apt install freerdp2-dev libvncserver-dev libssh-dev libtelnet-dev
    ~$ sudo apt install debhelper fakeroot
    # Install Qt
    ~$ sudo apt install qttools5-dev qttools5-dev-tools qtbase5-dev qtbase5-dev-tools qtmultimedia5-dev qtlocation5-dev libqt5svg5-dev libqtermwidget5-0-dev
    # Install X development libraries
    ~$ sudo apt install libxkbcommon-dev libxkbcommon-x11-dev libx11-xcb-dev libx11-dev libxfixes-dev
    ~$ sudo apt install libutf8proc-dev libpam0g-dev # Need by compile qtermwidget
    # Compile TigerVNC
    ~$ sudo apt install libpixman-1-dev
    ~$ git clone https://github.com/KangLin/tigervnc.git
    ~$ cd tigervnc
    ~$ mkdir build
    ~/tigervnc/build$ cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install
    ~/tigervnc/build$ cmake --build . --target install
    ~/tigervnc/build$ cd ~
    ~$ git clone https://github.com/KangLin/RabbitRemoteControl.git
    ~$ git clone https://github.com/KangLin/RabbitCommon.git
    ~$ cd RabbitRemoteControl
    ~/RabbitRemoteControl$ mkdir build
    ~/RabbitRemoteControl$ cd build
    ~/RabbitRemoteControl/build$ cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install -Dtigervnc_DIR=~/tigervnc/build/install/lib/cmake
    ~/RabbitRemoteControl/build$ cmake --build . --target install
    

### Library dependencies

- [Must] RabbitCommon: [https://github.com/KangLin/RabbitCommon](https://github.com/KangLin/RabbitCommon)
- [Optional] RFB
  + [Optional] libvncserver: [https://github.com/LibVNC/libvncserver](https://github.com/LibVNC/libvncserver)
  + [Optional] TigerVNC: https://github.com/KangLin/tigervnc
- [Optional] FreeRDP: [https://github.com/FreeRDP/FreeRDP](https://github.com/FreeRDP/FreeRDP)
- [Optional] [SSH]
  + libssh: [https://www.libssh.org](https://www.libssh.org)
  + libssh2: https://www.libssh2.org/
                 https://github.com/libssh2/libssh2
- [Optional] qtermwidget: [https://github.com/lxqt/qtermwidget](https://github.com/lxqt/qtermwidget)
- [Optional] libtelnet: [https://github.com/seanmiddleditch/libtelnet](https://github.com/seanmiddleditch/libtelnet)
- [Optional] libdatachannel: [https://github.com/paullouisageneau/libdatachannel](https://github.com/paullouisageneau/libdatachannel)

#### RabbitCommon
This library is placed in the same directory level as the project by default.
If not, you must specify the CMake parameters:
-DRabbitCommon_DIR=[RabbitCommon installation path]

    ~$ git clone https://github.com/KangLin/RabbitCommon.git
    
#### FreeRDP
- Use the system-packaged development library

      ~$ sudo apt install freerdp2-dev
    
- Use vcpkg
  + Source-code location: https://github.com/microsoft/vcpkg/
  + Specify CMake parameters:
    -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
  
        ~$ git clone https://github.com/microsoft/vcpkg.git
        ~$ cd vcpkg
        ~/vcpkg$ ./bootstrap-vcpkg.sh
        ~/vcpkg$ vcpkg install freerdp

- Compile from source code
  + Source-code location: https://github.com/FreeRDP/FreeRDP
  + See: https://github.com/FreeRDP/FreeRDP/wiki/Compilation
  + Specify CMake parameters:
    - -DBUILD_FREERDP=ON
    - -DFreeRDP-Client_DIR=[freerdp installation path]/lib/cmake/FreeRDP-Client2
    - -DFreeRDP_DIR=[freerdp installation path]/lib/cmake/FreeRDP2
    - -DWinPR_DIR=[freerdp installation path]/lib/cmake/WinPR2

          ~$ git clone https://github.com/FreeRDP/FreeRDP.git
          ~$ cd FreeRDP
          ~/FreeRDP$ mkdir build
          ~/FreeRDP/build$ cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install
          ~/FreeRDP/build$ cmake --build . --target install
          
#### libvncserver
- Use the system-packaged development library

      ~$ sudo apt install libvncserver-dev

- Compile from source code
  + Source-code location: [https://github.com/LibVNC/libvncserver](https://github.com/LibVNC/libvncserver)  
  It is recommended to use the patches from: https://github.com/KangLin/libvncserver
  + Specify CMake parameters: -Dvncclient_DIR=[libvncserver installation path]/lib/cmake/LibVncServer

#### TigerVnc
The official program does not support libraries.
See: https://github.com/TigerVNC/tigervnc/issues/1123  
The KangLin fork has support.
Source code location: https://github.com/KangLin/tigervnc  
Specify CMake parameters: -Dtigervnc_DIR=[TigerVNC installation path]/lib/cmake

    ~$ sudo apt install libpixman-1-dev
    ~$ git clone https://github.com/KangLin/tigervnc.git
    ~$ cd tigervnc
    ~/tigervnc$ mkdir build
    ~/tigervnc$ cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install
    ~/tigervnc$ cmake --build . --target install
    
#### libdatachannel
- Use vcpkg

      ~/vcpkg$ vcpkg install libdatachannel
      
- Compile from source code
  + Source code location: [https://github.com/paullouisageneau/libdatachannel](https://github.com/paullouisageneau/libdatachannel)
  + Compilation: [https://github.com/paullouisageneau/libdatachannel/blob/master/BUILDING.md](https://github.com/paullouisageneau/libdatachannel/blob/master/BUILDING.md)
  + Specify CMake parameters: -DLibDataChannel_DIR=[libdatachannel installation path]/share/cmake/libdatachannel

        ~$ git clone https://github.com/paullouisageneau/libdatachannel.git
        ~$ cd libdatachannel
        ~/libdatachannel$ git submodule update --init --recursive
        ~/libdatachannel$ mkdir build
        ~/libdatachannel$ cd build
        ~/libdatachannel/build$ cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install
        ~/libdatachannel/build$ cmake --build . --target install

#### qtermwidget
- Use the system-packaged development library

      ~$ sudo apt install libqtermwidget5-0-dev
      
- Compile from source code
  + Source code location: [https://github.com/lxqt/qtermwidget](https://github.com/lxqt/qtermwidget)
  + Specify CMake parameters: -Dqtermwidget5_DIR=[qtermwidget installation path]/lib/cmake/qtermwidget5

#### libssh
- Use the system-packaged development library

      ~$ sudo apt install libssh-dev 

- Use vcpkg
      
      vcpkg install libssh

- Compile from source code
  + Source-code location: [https://www.libssh.org](https://www.libssh.org)
  + Specify CMake parameters: -Dlibssh_DIR=[libssh installation path]/lib/cmake/libssh

### Compile this project
- Project location: [https://github.com/KangLin/RabbitRemoteControl](https://github.com/KangLin/RabbitRemoteControl)
- Download source code:

      ~$ git clone https://github.com/KangLin/RabbitRemoteControl.git

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
    - install-runtime: Only install runtime libraries and the program
    - install: Install runtime and development libraries and the program

  + Compile from the command-line
  
          ~$ cd RabbitRemoteControl
          ~/RabbitRemoteControl$ mkdir build
          ~/RabbitRemoteControl/build$ cmake .. -DCMAKE_INSTALL_PREIX=`pwd`/install 
          ~/RabbitRemoteControl/build$ cmake --build . --target install-runtime

  + Used by IDE(QtCreator)
    - Open project: Menu→File→Open File or project, Select the CMakeLists.txt of the project
    - Configure: Click Project→"Build & Run" in the toolbar on the left to configure CMake parameters
    - Compile and run: Click "Start Debugging of startup project" in the left toolbar, or press the shortcut key (F5)
