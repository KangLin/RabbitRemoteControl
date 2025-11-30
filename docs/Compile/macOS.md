## Compiling for macOS

Author: Eduardo Mozart de Oliveira <eduardomozart182@gmail.com>

### Environment
#### Operating system

- macOS Sequoia, version 15.6.1

#### Qt Creator

Version: v18.0.0 (ARM64) 
It is recommended to use version v5.0.2 or later.  
Prior versions don't have CMake support.

### Tools

#### Install the development tools packages

    ~$ xcode-select --install

  - The Xcode development tools package already includes the installation of the following:
    - Compiler
      - GCC/g++
      - make
      - git

#### brew: https://brew.sh
  
    # Install Homebrew
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

#### getopt
On macOS, the default getopt is the BSD version and does not support long options.
GNU getopt needs to be installed.

    # Install GNU getopt
    brew install gnu-getopt
    
    # Use GNU getopt (current session) temporarily
    export PATH="/usr/local/opt/gnu-getopt/bin:$PATH"
    
    # Permanent use (add to shell configuration)
    echo 'export PATH="/usr/local/opt/gnu-getopt/bin:$PATH"' >> ~/.zshrc
    echo 'alias getopt="/usr/local/opt/gnu-getopt/bin/getopt"' >> ~/.zshrc
    source ~/.zshrc
    
    # Verify installation
    getopt --version

#### automake、autoconf、fakeroot

    ~$ brew install automake fakeroot
  
#### Ninja: [https://ninja-build.org/](https://ninja-build.org/)

    ~$ brew install ninja

#### Git: [https://www.git-scm.com](https://www.git-scm.com/)

    ~$ brew install git

#### CMake: [https://cmake.org](https://cmake.org/)

  Version: Greater than 3.27.0

    ~$ brew install cmake

#### vcpkg: [https://github.com/microsoft/vcpkg](https://github.com/microsoft/vcpkg)

    ~$ git clone https://github.com/microsoft/vcpkg.git
    ~$ cd vcpkg
    ~/vcpkg$ ./bootstrap-vcpkg.sh
    ~/vcpkg$ ./vcpkg integrate install

#### Qt: [https://download.qt.io/official_releases/qt/](https://download.qt.io/official_releases/qt/)
  - Qt (official release)
    Current version: Qt 6.9.3

        ~$ brew install qt

    - Set environment variable or cmake parameters:
      - QT_ROOT
      - Qt6: Qt6_ROOT or Qt6_DIR .
        See: https://doc.qt.io/qt-6/cmake-get-started.html
      - Qt5: Qt5_ROOT or Qt5_DIR .
        See: https://doc.qt.io/qt-5/cmake-get-started.html
      - Environment variable

            eval "$(/opt/homebrew/bin/brew shellenv)"
            export QT_ROOT=$HOMEBREW_PREFIX/Cellar/qt/6.9.2

            # When Qt6
            export Qt6_ROOT=$QT_ROOT
            # When Qt5
            export Qt5_ROOT=$QT_ROOT

            # Or

            # When Qt6
            export Qt6_DIR=$QT_ROOT/lib/cmake/Qt6
            # When Qt5
            export Qt5_DIR=$QT_ROOT/lib/cmake/Qt5

      - CMAKE parameters

            # Qt6
            cmake -DQT_ROOT=/opt/homebrew/Cellar/qt/6.9.2 -DQt6_DIR=/opt/homebrew/Cellar/qt/6.9.2/lib/cmake/Qt6 ......
            # Qt5 
            cmake -DQT_ROOT=/opt/homebrew/Cellar/qt/6.9.2 -DQt5_DIR=/opt/homebrew/Cellar/qt/6.9.2/lib/cmake/Qt5 ......
    
      - Set the Qt version of your current environment

            # Set the Qt version of your current environment
            export QT_SELECT="qt6"
            export QTTOOLDIR="$QT_ROOT/bin"
            export QTLIBDIR="$HOMEBREW_PREFIX/lib"

#### [OPTIONAL] IDE: Qt Creator. It is recommended to use version v5.0.2 or later.  
  Prior versions don't have CMake support.

    ~$ brew install --cask qt-creator
      
#### Doxygen: [http://www.doxygen.nl/](http://www.doxygen.nl/)

    ~$ brew install doxygen

### Library dependencies

- [MUST] RabbitCommon: [https://github.com/KangLin/RabbitCommon](https://github.com/KangLin/RabbitCommon)
- [MUST] [Openssl:](https://github.com/openssl/openssl) Required to run Qt.
- [OPTIONAL] FreeRDP: [https://github.com/FreeRDP/FreeRDP](https://github.com/FreeRDP/FreeRDP)
- [OPTIONAL] RFB
  + [OPTIONAL] RabbitVNC: [https://github.com/KangLin/RabbitVNC](https://github.com/KangLin/RabbitVNC)
  + [OPTIONAL] LibVNCServer: [https://github.com/LibVNC/libvncserver](https://github.com/LibVNC/libvncserver)
  + [OPTIONAL] TigerVNC: [https://github.com/KangLin/tigervnc](https://github.com/KangLin/tigervnc)
- [OPTIONAL] QTermWidget: [https://github.com/lxqt/qtermwidget](https://github.com/lxqt/qtermwidget)
- [OPTIONAL] QtSsh: [https://github.com/condo4/QtSsh.git](https://github.com/condo4/QtSsh.git)
- [OPTIONAL] [SSH]
  + libssh:
    - [https://www.libssh.org](https://www.libssh.org/)
    - [libssh API](https://api.libssh.org/stable/index.html)
  + libssh2:
    - [https://www.libssh2.org](https://www.libssh2.org/)
    - [https://github.com/libssh2/libssh2](https://github.com/libssh2/libssh2/)
- [OPTIONAL] QTelnet: [https://github.com/silderan/QTelnet.git](https://github.com/silderan/QTelnet.git)
- [OPTIONAL] libtelnet: [https://github.com/seanmiddleditch/libtelnet](https://github.com/seanmiddleditch/libtelnet)
- [OPTIONAL] libdatachannel: [https://github.com/paullouisageneau/libdatachannel](https://github.com/paullouisageneau/libdatachannel)
- [OPTIONAL] QXmpp: [https://github.com/qxmpp-project/qxmpp](https://github.com/qxmpp-project/qxmpp)
- [OPTIONAL] QtService: [https://github.com/KangLin/qt-solutions](https://github.com/KangLin/qt-solutions)
- [OPTIONAL] PcapPlusPlus: [https://github.com/seladb/PcapPlusPlus](https://github.com/seladb/PcapPlusPlus).
  The WakeOnLan plugin required.
- [OPTIONAL] FFMPEG: [https://ffmpeg.org/](https://ffmpeg.org/).
  QtMultimedia required.
- [OPTIONAL] qtkeychain: [https://github.com/frankosterfeld/qtkeychain](https://github.com/frankosterfeld/qtkeychain)
- [OPTIONAL] libcurl: [https://curl.se](https://curl.se)

#### RabbitCommon

- This library is placed in the same directory level as the project by default.

      ~$ git clone https://github.com/KangLin/RabbitRemoteControl.git
      ~$ git clone https://github.com/KangLin/RabbitCommon.git

- If not, you must specify the CMake parameters or environment variable:

      ~$ git clone https://github.com/KangLin/RabbitRemoteControl.git
      # Set environment variable
      ~$ export RabbitCommon_ROOT=[RabbitCommon install root]
      ~$ cd RabbitRemoteControl
      # Or set CMake parameters
      ~/RabbitRemoteControl$ cmake -DRabbitCommon_ROOT=[RabbitCommon installation path] ......

#### FreeRDP

- Use the brew-packaged development library

      ~$ brew install freerdp
    
- Use vcpkg
  + Source-code location: https://github.com/microsoft/vcpkg/
  
        ~$ git clone https://github.com/microsoft/vcpkg.git
        ~$ cd vcpkg
        ~/vcpkg$ ./vcpkg install freerdp

  + Specify the CMake parameters:
    `-DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake`
    
  + With vcpkg, the FreeRDP service is not a default feature,
    so you need to specify it manually.

- Compile from source code
  + Source-code location: https://github.com/FreeRDP/FreeRDP
  + Compilation instructions: https://github.com/FreeRDP/FreeRDP/wiki/Compilation
  
          ~$ git clone https://github.com/FreeRDP/FreeRDP.git
          ~$ cd FreeRDP
          ~/FreeRDP$ mkdir build
          ~/FreeRDP/build$ cmake .. -DCMAKE_BUILD_TYPE=Release \
              -DCMAKE_INSTALL_PREFIX=`pwd`/install -DWITH_SERVER=ON
          ~/FreeRDP/build$ cmake --build . --config Release --target install
          
- When FreeRDP is compiled from source,
  the CMake parameter needs to be specified to compile this project:
  - -DBUILD_FREERDP=ON
  - -DFreeRDP-Client_DIR=[freerdp installation path]/lib/cmake/FreeRDP-Client3
  - -DFreeRDP_DIR=[freerdp installation path]/lib/cmake/FreeRDP3
  - -DWinPR_DIR=[freerdp installation path]/lib/cmake/WinPR3
  - -DFreeRDP-Shadow_DIR=[freerdp installation path]/lib/cmake/FreeRDP-Shadow3
  - -DFreeRDP-Server_DIR=[freerdp installation path]/lib/cmake/FreeRDP-Server3

#### LibVNCServer

- Use the brew-packaged development library

      ~$ brew install libvncserver

- Compile from source code
  + Source-code location: [https://github.com/LibVNC/libvncserver](https://github.com/LibVNC/libvncserver)  
  It is recommended to use the patches from: https://github.com/KangLin/libvncserver
- When LibVNCServer is compiled from source,
  the CMake parameter needs to be specified to compile this project:
  
      -DLibVNCServer_DIR=[LibVNCServer installation path]/lib/cmake/LibVNCServer

#### RabbitVNC

- Compile from source code
Source-code location: https://github.com/KangLin/RabbitVNC  

      ~$ brew install pixman
      ~$ git clone https://github.com/KangLin/RabbitVNC.git
      ~$ cd RabbitVNC
      ~/RabbitVNC$ mkdir build
      ~/RabbitVNC$ cd build
      ~/RabbitVNC$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`pwd`/install -DCMAKE_POLICY_VERSION_MINIMUM=3.5
      ~/RabbitVNC$ cmake --build . --config Release --target install
    
- When RabbitVNC is compiled from source,
  the CMake parameter needs to be specified to compile this project:
  
      -DRabbitVNC_DIR=[RabbitVNC installation path]/lib/cmake/RabbitVNC

#### TigerVNC

- Compile from source code

The official program does not support libraries.
See: https://github.com/TigerVNC/tigervnc/issues/1123  
The KangLin fork has support.
Source-code location: https://github.com/KangLin/tigervnc  

    ~$ brew install pixman
    ~$ git clone https://github.com/KangLin/tigervnc.git
    ~$ cd tigervnc
    ~/tigervnc$ mkdir build
    ~/tigervnc$ cd build
    ~/tigervnc/build$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`pwd`/install
    ~/tigervnc/build$ cmake --build .. --config Release --target install
    
- When TigerVNC is compiled from source,
  the CMake parameter needs to be specified to compile this project:

      -Dtigervnc_DIR=[TigerVNC installation path]/lib/cmake/tigervnc

#### libdatachannel

- Use vcpkg
  + Source-code location: https://github.com/microsoft/vcpkg/

        ~$ git clone https://github.com/microsoft/vcpkg.git
        ~$ cd vcpkg
        ~/vcpkg$ ./vcpkg install libdatachannel
      
  + Specify the CMake parameters:
    `-DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake`
  
- Compile from source code
  + Source-code location: [https://github.com/paullouisageneau/libdatachannel](https://github.com/paullouisageneau/libdatachannel)
  + Compilation: [https://github.com/paullouisageneau/libdatachannel/blob/master/BUILDING.md](https://github.com/paullouisageneau/libdatachannel/blob/master/BUILDING.md)
  
        ~$ git clone https://github.com/paullouisageneau/libdatachannel.git
        ~$ cd libdatachannel
        ~/libdatachannel$ git submodule update --init --recursive
        ~/libdatachannel$ mkdir build
        ~/libdatachannel$ cd build
        ~/libdatachannel/build$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`pwd`/install
        ~/libdatachannel/build$ cmake --build . --config Release --target install

- When libdatachannel is compiled from source,
  the CMake parameter needs to be specified to compile this project:
  
      -DLibDataChannel_DIR=[libdatachannel installation path]/lib/cmake/LibDataChannel

#### QXmpp

- Use the brew-packaged development library

      ~$ brew install qxmpp

- Compile from source code
  + Source-code location: [https://github.com/qxmpp-project/qxmpp](https://github.com/qxmpp-project/qxmpp)
  
        ~$ git clone https://github.com/qxmpp-project/qxmpp.git
        ~$ cd qxmpp
        ~/qxmpp$ mkdir build
        ~/qxmpp$ cd build
        ~/qxmpp/build$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`pwd`/install -DQt5_DIR=[Qt install path]/lib/cmake/Qt5
        ~/qxmpp/build$ cmake --build . --config Release --target install

- When QXmpp is compiled from source,
  the CMake parameter needs to be specified to compile this project:
  
      -DQXmpp_DIR=[libdatachannel installation path]/lib/cmake/qxmpp

#### QTermWidget
      
- Compile from source code
  + Source-code location: [https://github.com/lxqt/qtermwidget](https://github.com/lxqt/qtermwidget)

        ~$ git clone https://github.com/lxqt/lxqt-build-tools.git
        ~$ cd lxqt-build-tools
        ~/lxqt-build-tools$ mkdir build
        ~/lxqt-build-tools$ cd build
        ~/lxqt-build-tools/build$ cmake ..
        ~/lxqt-build-tools/build$ make
        ~/lxqt-build-tools/build$ make install

        ~$ brew install vulkan-headers
        ~$ git clone https://github.com/lxqt/qtermwidget.git
        ~$ cd qtermwidget
        ~/qtermwidget$ mkdir build
        ~/qtermwidget$ cd build
        ~/qtermwidget/build$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`pwd`/install
        ~/qtermwidget/build$ cmake --build . --config Release --target install

- When QTermWidget is compiled from source,
  the CMake parameter needs to be specified to compile this project:
  
      -Dqtermwidget6_DIR=[qtermwidget installation path]/lib/cmake/qtermwidget5

#### libssh

- Use the brew-packaged development library

      ~$ brew install libssh 

- Use vcpkg
  + Source-code location: https://github.com/microsoft/vcpkg/
 
        ~$ git clone https://github.com/microsoft/vcpkg.git
        ~$ cd vcpkg
        ~/vcpkg$ ./vcpkg install libssh

  + Specify the CMake parameters:
    `-DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake`

- Compile from source code
  + Source-code location: [https://www.libssh.org](https://www.libssh.org)
- When libssh is compiled from source,
  the CMake parameter needs to be specified to compile this project:
  
      -Dlibssh_DIR=[libssh installation path]/lib/cmake/libssh

#### QtService

- Compile from source code
  + Source-code location: https://github.com/KangLin/qt-solutions/
  
        ~$ git clone https://github.com/KangLin/qt-solutions.git
        ~$ cd qt-solutions/qtservice
        ~/qt-solutions/qtservice$ mkdir build
        ~/qt-solutions/qtservice$ cd build
        ~/qt-solutions/qtservice/build$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`pwd`/install
        ~/qt-solutions/qtservice/build$ cmake --build . --config Release --target install
  
- When QtService is compiled from source,
  the CMake parameter needs to be specified to compile this project:

      -DQtService_DIR=[QtService installation path]/lib/cmake/QtService

### PcapPlusPlus

- Compile from source code
  + Source-code location: https://github.com/seladb/PcapPlusPlus

        ~$ git clone https://github.com/seladb/PcapPlusPlus.git

- Use vcpkg
  + Source-code location: https://github.com/microsoft/vcpkg/

        ~$ git clone https://github.com/microsoft/vcpkg.git
        ~$ cd vcpkg
        ~/vcpkg$ ./vcpkg install pcapplusplus

  + Specify the CMake parameters:
    `-DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake`

- When PcapPlusPlus is compiled from source,
  the CMake parameter needs to be specified to compile this project:

      -DPcapPlusPlus_DIR=[PcapPlusPlus install path]/lib/cmake/pcapplusplus

#### libpcap

It is dependent on PcapPlusPlus

- Use the brew-packaged development library

      ~$ brew install libpcap

- Source-code location: https://github.com/the-tcpdump-group/libpcap

#### cmark

- Use the brew-packaged development library

      ~$ brew install cmark

- When cmark is compiled from source,
  the CMake parameter needs to be specified to compile this project:

      -Dcmark_DIR=[PcapPlusPlus install path]/lib/cmake/cmark

### Compile this project

#### Source code

- Project location: [https://github.com/KangLin/RabbitRemoteControl](https://github.com/KangLin/RabbitRemoteControl)
- Download the source code:

      ~$ git clone https://github.com/KangLin/RabbitRemoteControl.git

#### CMake parameters or environment variable

- CMake parameters or environment variable
  + [Qt](#Qt)
    + QT_ROOT: Qt install root
    + Qt6: See: https://doc.qt.io/qt-6/cmake-get-started.html
      + Qt6_ROOT: Is same QT_ROOT
      + Qt6_DIR: $QT_ROOT/lib/cmake/Qt6
    + Qt5: See: https://doc.qt.io/qt-5/cmake-get-started.html
      + Qt5_ROOT: Is same QT_ROOT
      + Qt5_DIR: $QT_ROOT/lib/cmake/Qt5
  + RabbitCommon_ROOT: RabbitCommon source code location
  + BUILD_CLIENT: Compile client. the default is ON
  + BUILD_SERVICE: Compile service. the default dependency is whether there is a QtService
  + BUILD_PLUGINS: Compile plugins. the default is ON
  + BUILD_APP: Compile applaction. the default is ON
  + BUILD_DOCS: Build docmenets. the default is OFF
  + BUILD_SHARED_LIBS: Compile shared libraries. the default is ON
  + BUILD_FREERDP: If compile FreeRDP. the default is OFF
  + WinPR_DIR: [freerdp installation path]/lib/cmake/WinPR3
  + FreeRDP_DIR: [freerdp installation path]/lib/cmake/FreeRDP3
  + FreeRDP-Client_DIR: [freerdp installation path]/lib/cmake/FreeRDP-Client3
  + BUILD_RABBITVNC: Compile RabbitVNC. the default is ON
  + RabbitVNC_DIR: [RabbitVNC installation path]/lib/cmake/RabbitVNC
  + BUILD_TigerVNC: Compile TigerVNC. the default is ON
  + TigerVNC_DIR: [TigerVNC installation path]/lib/cmake/tigervnc
  + BUILD_LibVNCServer: Compile LibVNCServer. the default is ON
  + LibVNCServer_DIR: [libvncserver installation path]/lib/cmake/LibVNCServer
  + LibDataChannel_DIR: [libdatachannel installation path]/lib/cmake/LibDataChannel
  + QXmpp_DIR=[QXmpp installation path]/lib/cmake/qxmpp
  + qtermwidget5_DIR: [qtermwidget installation path]/lib/cmake/qtermwidget5
  + libssh_DIR: [libssh installation path]/lib/cmake/libssh
  + QtService_DIR: [QtService installation path]/lib/cmake/QtService
  + PcapPlusPlus_DIR: [PcapPlusPlus install path]/lib/cmake/pcapplusplus
- If using vcpkg, please set the CMake parameters:
  + CMAKE_TOOLCHAIN_FILE: [vcpkg installation path]/scripts/buildsystems/vcpkg.cmake

#### Compilation

- Compile from the command line
  - Not using vcpkg

        ~$ cd RabbitRemoteControl
        ~/RabbitRemoteControl$ mkdir build
        ~/RabbitRemoteControl/build$ cmake .. \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=`pwd`/install [...]
        ~/RabbitRemoteControl/build$ cmake --build . \
            --config Release --target install

  - If using vcpkg

        ~$ cd RabbitRemoteControl
        ~/RabbitRemoteControl$ mkdir build
        ~/RabbitRemoteControl/build$ cmake .. -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=`pwd`/install \
            -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake \
            [options libraries]
        ~/RabbitRemoteControl/build$ cmake --build . \
            --config Release --target install

- Using an IDE (Qt Creator)
  - Qt Versions: Menu→ Preferences→ Kits→ Qt Versions→ Add→ `/opt/homebrew/opt/qt/bin/qmake`
  - Open project: Menu→ File→ Open File or project, Select the CMakeLists.txt of the project
  - Configure: Click Project→ "Build & Run" in the toolbar on the left to configure CMake parameters
  - Compile and run: Click "Start Debugging of startup project" in the left toolbar, or press the shortcut key (F5)
    - If needed, install: select install in target
  - If using vcpkg:
    - Menu→ Preferences→ Kits→ CMake Configuration: add `-DCMAKE_TOOLCHAIN_FILE:FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake`
    - Menu→ About Plugins→ vcpkg: enable
    - Menu→ Preferences→ CMake→ Vcpkg→ Vcpkg Path: add `[vcpkg installation path]`
  - If using Ninja from Homebrew: Menu→ Preferences→ Kits→ Cmake Configureration: add `-DCMAKE_MAKE_PROGRAM:FILE=/opt/homebrew/bin/ninja`

#### Run

- The application is installed in install/bin

      ~$ cd RabbitRemoteControl
      ~/RabbitRemoteControl$ cd build/install/bin
      ~/RabbitRemoteControl$ ./RabbitRemoteControl.sh
      # Or
      ~/RabbitRemoteControl$ ./RabbitRemoteControlApp

**Note:** If the plugin does not load.
The plugin's dependencies may not be installed on the system.
You can add the path of the dependency libraries to the environment variable `LD_LIBRARY_PATH` and/or `DYLD_FRAMEWORK_PATH`.
You can also add the path of the dependency libraries to the `/etc/ld.so.conf` file,
and then run ldconfig to add the dependency libraries to the system.
