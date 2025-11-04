## Compiling for Linux

Author: Kang Lin <kl222@126.com>  
Translator: Allan Nordhøy <epost@anotheragency.no>

### Quickly started

    # Only build AppImage
    ./Script/build_linux.sh --appimage
    # Only build deb package
    ./Script/build_linux.sh --deb
    # Only build rpm package
    ./Script/build_linux.sh --rpm

See: [Script](#Script)

### Environment
#### Operating system

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

- Fedora

      $ cat /etc/redhat-release 
      Fedora release 42 (Adams)

#### Qt Creator

Version: v17.0.0 
It is recommended to use version v5.0.2 or later.  
Prior versions don't have CMake support.

### Tools

#### Install the development tools package

      ~$ sudo apt install build-essential

  - The development tools package already includes the installation of the following:
    - Compiler
      - GCC/g++
  
            ~$ sudo apt install g++ gcc
            
      - automake、autoconf、make、fakeroot

            ~$ sudo apt install automake autoconf make fakeroot

#### Git: [https://www.git-scm.com](https://www.git-scm.com/)

      ~$ sudo apt install git
        
#### CMake: [https://cmake.org](https://cmake.org/)

  Version: Greater than 3.27.0

      ~$ sudo apt install cmake

#### Package tools: debhelper

      ~$ sudo apt install debhelper

#### Qt
  - Qt (official release): https://download.qt.io/official_releases/qt/  
    Current version: Qt 6.9.2
    - Set environment variable or cmake parameters:
      - QT_ROOT
      - Qt6: Qt6_ROOT or Qt6_DIR .
        See: https://doc.qt.io/qt-6/cmake-get-started.html
      - Qt5: Qt5_ROOT or Qt5_DIR .
        See: https://doc.qt.io/qt-5/cmake-get-started.html
      - Environment variable

            export QT_ROOT=Qt install root
            # Needed by complied AppImage
            export QMAKE=$QT_ROOT/bin/qmake

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
            cmake -DQT_ROOT=[Qt install root] -DQt6_DIR=[Qt install root]/lib/cmake/Qt6 ......
            # Qt5 
            cmake -DQT_ROOT=[Qt install root] -DQt5_DIR=[Qt install root]/lib/cmake/Qt5 ......
            # Needed by complied AppImage
            export QMAKE=$QT_ROOT/bin/qmake

  - System build-in:  
    - Qt5:

          ~$ sudo apt install qttools5-dev qttools5-dev-tools qtbase5-dev qtbase5-dev-tools qtmultimedia5-dev qtlocation5-dev libqt5svg5-dev libqtermwidget5-0-dev

    - Qt6: See: [Script/build_depend.sh](../../Script/build_depend.sh)

          ~$ sudo apt install qmake6 qt6-tools-dev qt6-tools-dev-tools qt6-base-dev qt6-base-dev-tools qt6-qpa-plugins libqt6svg6-dev qt6-l10n-tools qt6-translations-l10n qt6-scxml-dev qt6-multimedia-dev libqt6serialport6-dev

    - When multiple distributions of Qt are installed on the system.
      For example: Install Qt5 and Qt6 at the same time.
      The system uses the qtchooser tool to select the current Qt version.
  
          l@l:/home/RabbitRemoteControl$ qtchooser 
          Usage:
            qtchooser { -l | -list-versions | -print-env }
            qtchooser -install [-f] [-local] <name> <path-to-qmake>
            qtchooser -run-tool=<tool name> [-qt=<Qt version>] [program arguments]
            <executable name> [-qt=<Qt version>] [program arguments]

          Environment variables accepted:
           QTCHOOSER_RUNTOOL  name of the tool to be run (same as the -run-tool argument)
           QT_SELECT          version of Qt to be run (same as the -qt argument)
  
      - List which version of Qt is currently installed on your system

            l@l:/home/RabbitRemoteControl$ qtchooser -l
            4
            5
            default
            qt4-x86_64-linux-gnu
            qt4
            qt5-x86_64-linux-gnu
            qt5
            qt6

            # View the Qt version of your current environment
            l@l:/home/RabbitRemoteControl$ qtchooser --print-env
            QT_SELECT="default"
            QTTOOLDIR="[Paths]"
            QTLIBDIR="Prefix=/usr"
    
      - Set the Qt version of your current environment

            export QT_SELECT=qt6  # Set the Qt6 version of your current environment

            # View the Qt version of your current environment
            l@l:/home/RabbitRemoteControl$ qtchooser --print-env
            QT_SELECT="qt6"
            QTTOOLDIR="/usr/lib/qt6/bin"
            QTLIBDIR="/usr/lib/aarch64-linux-gnu"

  - QtWebEngine
    By default, WebM (open source) is included, while x264 and x265 are not included due to copyright reasons.
    - Check supportYou can access chrome://media-internals or chrome://gpu in your QtWebEngine application to see the currently supported decoding formats.
    - In the QtWebEngine program, visit:
      - https://www.webmfiles.org/demo-files/ to upload or play H264/H265 video files to test support
      - https://html5test.com/
      - https://webrtc.github.io/test-pages/
      - https://browserleaks.com/webrtc
    - Recompile QtWebEngine, including the appropriate decoders.  
      Related compilation parameters:
      - -webengine-proprietary-codecsEnable  
        proprietary codec support (H264, MP3, AAC, etc.).
      - -webengine-ffmpegSpecify  
        using a custom ffmpeg.
      
            ./configure -webengine-proprietary-codecs
            make -j$(nproc)make install

#### [OPTIONAL] IDE: Qt Creator. It is recommended to use version v5.0.2 or later.  
  Prior versions don't have CMake support.

      ~$ sudo apt install qtcreator

#### OpenGL: Required to run Qt.

      ~$ sudo apt install libglu1-mesa-dev
      
#### Doxygen: [http://www.doxygen.nl/](http://www.doxygen.nl/)

      ~$ sudo apt install doxygen

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

- Use the system-packaged development library

      ~$ sudo apt install freerdp3-dev
    
- Use vcpkg
  + Source-code location: https://github.com/microsoft/vcpkg/
  
        ~$ git clone https://github.com/microsoft/vcpkg.git
        ~$ cd vcpkg
        ~/vcpkg$ ./bootstrap-vcpkg.sh
        ~/vcpkg$ ./vcpkg install freerdp

  + Specify the CMake parameters:
    -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
  + With vcpk, the FreeRDP service is not a default feature,
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

- Use the system-packaged development library

      ~$ sudo apt install libvncserver-dev

- Compile from source code
  + Source-code location: [https://github.com/LibVNC/libvncserver](https://github.com/LibVNC/libvncserver)  
  It is recommended to use the patches from: https://github.com/KangLin/libvncserver
- When LibVNCServer is compiled from source,
  the CMake parameter needs to be specified to compile this project:
  
      -DLibVNCServer_DIR=[LibVNCServer installation path]/lib/cmake/LibVNCServer

#### RabbitVNC

- Compile from source code
Source-code location: https://github.com/KangLin/RabbitVNC  

      ~$ sudo apt install libpixman-1-dev
      ~$ git clone https://github.com/KangLin/RabbitVNC.git
      ~$ cd RabbitVNC
      ~/RabbitVNC$ mkdir build
      ~/RabbitVNC$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`pwd`/install
      ~/RabbitVNC$ cmake --build . --config Release --target install
    
- When RabbitVNC is compiled from source,
  the CMake parameter needs to be specified to compile this project:
  
      -DRabbitVNC_DIR=[RabbitVNC installation path]/lib/cmake/tigervnc

#### TigerVNC

- Compile from source code

The official program does not support libraries.
See: https://github.com/TigerVNC/tigervnc/issues/1123  
The KangLin fork has support.
Source-code location: https://github.com/KangLin/tigervnc  

    ~$ sudo apt install libpixman-1-dev
    ~$ git clone https://github.com/KangLin/tigervnc.git
    ~$ cd tigervnc
    ~/tigervnc$ mkdir build
    ~/tigervnc$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`pwd`/install
    ~/tigervnc$ cmake --build . --config Release --target install
    
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
    -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake
  
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

- Use the system-packaged development library

      ~$ sudo apt install libqxmpp-dev

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

- Use the system-packaged development library

      ~$ sudo apt install libqtermwidget5-0-dev
      
- Compile from source code
  + Source-code location: [https://github.com/lxqt/qtermwidget](https://github.com/lxqt/qtermwidget)
- When QTermWidget is compiled from source,
  the CMake parameter needs to be specified to compile this project:
  
      -Dqtermwidget5_DIR=[qtermwidget installation path]/lib/cmake/qtermwidget5

- When install, Need to copy resources to install directory

      if [ -d "${INSTALL_DIR}/share/qtermwidget6" ]; then
          cp -r ${INSTALL_DIR}/share/qtermwidget6 ${INSTALL_APP_DIR}/share/
      else
          echo "${INSTALL_DIR}/share/qtermwidget6 is not exist"
      fi

#### libssh

- Use the system-packaged development library

      ~$ sudo apt install libssh-dev 

- Use vcpkg
  + Source-code location: https://github.com/microsoft/vcpkg/
 
        ~$ git clone https://github.com/microsoft/vcpkg.git
        ~$ cd vcpkg
        ~/vcpkg$ ./vcpkg install libssh

  + Specify the CMake parameters:
  -DCMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake

- Compile from source code
  + Source-code location: [https://www.libssh.org](https://www.libssh.org)
- When libssh is compiled from source,
  the CMake parameter needs to be specified to compile this project:
  
      -Dlibssh_DIR=[libssh installation path]/lib/cmake/libssh

#### QtService

- Compile from source code
  + Source-code location: https://github.com/KangLin/qt-solutions/
  
        ~$ git clone https://github.com/KangLin/qt-solutions.git
        ~$ cd qt-solutions
        ~/qt-solutions$ mkdir build
        ~/qt-solutions$ cd build
        ~/qt-solutions/build$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`pwd`/install
        ~/qt-solutions/build$ cmake --build . --config Release --target install
  
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

- When PcapPlusPlus is compiled from source,
  the CMake parameter needs to be specified to compile this project:

      -DPcapPlusPlus_DIR=[PcapPlusPlus install path]/lib/cmake/pcapplusplus

#### libpcap

It is depended by PcapPlusPlus

- Use the system-packaged development library

      ~$ sudo apt install libpcap-dev

- Sourc-code location: https://github.com/the-tcpdump-group/libpcap

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

- Compile from the command-line
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
  - Open project: Menu→ File→ Open File or project, Select the CMakeLists.txt of the project
  - Configure: Click Project→ "Build & Run" in the toolbar on the left to configure CMake parameters
  - Compile and run: Click "Start Debugging of startup project" in the left toolbar, or press the shortcut key (F5)
    - If need install: select install in target
  - If using vcpkg: Menu→ Options→ Kits→ Cmake Configureration: add CMAKE_TOOLCHAIN_FILE=[vcpkg installation path]/scripts/buildsystems/vcpkg.cmake

- Use script [build_debpackage.sh](../../Script/build_debpackage.sh)
  - Set [CMake parameters or environment variable](#CMake-parameters-or-environment-variable) as environment variable. eg:

        export RabbitVNC_DIR=[RabbitVNC installation path]/lib/cmake/RabbitVNC
        export tigervnc_DIR=[TigerVNC installation path]/lib/cmake/tigervnc

  - Use [build_debpackage.sh](../../Script/build_debpackage.sh)

        ./Script/build_debpackage.sh $QT_ROOT $RabbitCommon_ROOT

#### Run

- The application is installed in install/bin

      ~$ cd RabbitRemoteControl
      ~/RabbitRemoteControl$ cd build/install/bin
      ~/RabbitRemoteControl$ ./RabbitRemoteControl.sh
      # Or
      ~/RabbitRemoteControl$ ./RabbitRemoteControlApp
      
**Note:** If the plugin does not load.
It may be that the plugin's dependencies are not installed on the system.
You can add the path of the dependency libraries to the environment variable LD_LIBRARY_PATH.
You can also add the path of the dependency libraries to the /etc/ld.so.conf file,
and then run ldconfig to add the dependency libraries to the system.

#### Compilation example

##### Ubuntu

    ```bash
    #Install the development tools package
    ~$ sudo apt install build-essential
    # Install tools
    ~$ sudo apt install git cmake debhelper doxygen
    # Install Qt5
    ~$ sudo apt install qttools5-dev qttools5-dev-tools qtbase5-dev \
        qtbase5-dev-tools qtmultimedia5-dev qtlocation5-dev libqt5svg5-dev \
        libqtermwidget5-0-dev

    # Install dependency libraries
    ~$ sudo apt install freerdp2-dev libvncserver-dev libssh-dev libtelnet-dev
    # Install X development libraries
    ~$ sudo apt install libxkbcommon-dev libxkbcommon-x11-dev libx11-xcb-dev \
        libx11-dev libxfixes-dev
    ~$ sudo apt install libutf8proc-dev libpam0g-dev # Need by compile qtermwidget
    # Install libpixman, RabbitVNC and TigerVNC need it.
    ~$ sudo apt install libpixman-1-dev libjpeg-dev
    # Compile RabbitVNC
    ~$ git clone https://github.com/KangLin/RabbitVNC.git
    ~$ cd RabbitVNC
    ~$ mkdir build
    ~/RabbitVNC/build$ cmake .. -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=`pwd`/install
    ~/RabbitVNC/build$ cmake --build . --config Release --target install
    ~/RabbitVNC/build$ cd ~
    # Compile TigerVNC
    ~$ git clone https://github.com/KangLin/tigervnc.git
    ~$ cd tigervnc
    ~$ mkdir build
    ~/tigervnc/build$ cmake .. -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=`pwd`/install
    ~/tigervnc/build$ cmake --build . --config Release --target install
    ~/tigervnc/build$ cd ~
    ~$ sudo apt install libqxmpp-dev
    # Compile libdatachannel
    ~$ git clone https://github.com/paullouisageneau/libdatachannel.git
    ~$ cd libdatachannel
    ~/libdatachannel$ git submodule update --init --recursive
    ~/libdatachannel$ mkdir build
    ~/libdatachannel$ cd build
    ~/libdatachannel/build$ cmake .. -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=`pwd`/install
    ~/libdatachannel/build$ cmake --build . --config Release --target install
    ~/libdatachannel/build$ cd ~
    # Compile QtService
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
        -DRabbitVNC_DIR=~/RabbitVNC/build/install/lib/cmake/RabbitVNC \
        -Dtigervnc_DIR=~/tigervnc/build/install/lib/cmake/tigervnc \
        -DBUILD_FREERDP=ON \
        -DLibDataChannel_DIR=~/libdatachannel/build/install/lib/cmake/LibDataChannel \
        -DQtService_DIR=~/qt-solutions/qtservice/build/lib/cmake/QtService
    ~/RabbitRemoteControl/build$ cmake --build . --config Release --target install
    ```

See: [Script](#Script)

##### [AppImage](https://github.com/linuxdeploy/linuxdeploy)

- Compile

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

- Build AppImage

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
  
- Run AppImage

      sudo chmod u+x ./Rabbit_Remote_Control-`uname -m`.AppImage
      ./Rabbit_Remote_Control-`uname -m`.AppImage

- Other
  - Extract AppImage
  
        ./Rabbit_Remote_Control-`uname -m`.AppImage --appimage-extract

  - FUSE issue
  
    When you run the AppImage file for the first time,
    you may encounter PUSE-related issues,
    and the following error message is displayed:
    
    ```
    dlopen(): error loading libfuse.so.2

    AppImages require FUSE to run.
    You might still be able to extract the contents of this AppImage
    if you run it with the --appimage-extract option.
    See https://github.com/AppImage/AppImageKit/wiki/FUSE

    for more information
    ```

    Solution: Install fuse2
        
        sudo apt install fuse

  - Fuse issue in docker
  
    When creating a container, add the parameter: `--privileged`
    
        docker run --privileged --interactive ubuntu

##### snap

- Install snapcraft

      sudo snap install snapcraft --classic

- Build
  - Parts lifecycle: https://snapcraft.io/docs/parts-lifecycle
  - https://snapcraft.io/docs/how-snapcraft-builds  
    Each of these lifecycle steps can be run from the command line,
    and the command can be part specific or apply to all parts in a project.

        snapcraft pull [<part-name>]
        snapcraft build [<part-name>]
        snapcraft stage [<part-name>]
        snapcraft prime [<part-name>]
        snapcraft pack or snapcraft
        
  - Iterating over a build: https://snapcraft.io/docs/iterating-over-a-build
    The following commands enable you to step into this encapsulated environment:
    - --shell: builds your snap to the lifecycle step prior to that specified and opens a shell into the environment (e.g. running snapcraft prime --shell will run up to the stage step and open a shell).
    - --shell-after: builds your snap to the lifecycle step specified and opens a shell into the environment. (eg. running snapcraft prime --shell-after will run up to the prime step and then drop into a shell).
    - --debug, opens a shell inside the environment after an error occurs.  
      For example, to open a shell just before the prime phase, use the following command:
    - -v: show build verbose

          $ snapcraft prime -v --debug --shell
          Using 'snap/snapcraft.yaml': Project assets will be searched for from
          the 'snap' directory.
          Launching a VM.
          Launched: snapcraft-test
          [...]
          Pulling part-test
          Building part-test
          Staging part-test
          snapcraft-test #

  - Build clean

        snapcraft clean

- Test
  - Use --devmode

        snap install ./rabbitremotecontrol_0.0.27_amd64.snap --devmode
 
  - Run
 
        rabbitremotecontrol

  - Remove

        snap remove rabbitremotecontrol

### Flatpak

- Install the software you need to build and run Flatpak: flatpak and flatpak-builder
  - Fedora:

        $ sudo dnf install flatpak flatpak-builder

  - Debian/Ubuntu:

        $ sudo apt-get update && sudo apt-get install flatpak flatpak-builder

  - Archlinux

        $ sudo pacman -Sy flatpak flatpak-builder

- Add [Flathub](https://flathub.org/) repositories:

      $ flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo

  - Show details

        $ flatpak remotes --show-details

- Build

      $ flatpak-builder build-dir RabbitRemoteControl/io.github.KangLin.RabbitRemoteControl.yml
      ** (flatpak-builder:37): WARNING **: 03:05:45.408: Unknown property branch for type BuilderSourceDir
      error: org.kde.Sdk/x86_64/6.8 not installed
      Failed to init: Unable to find sdk org.kde.Sdk version 6.8

Install sdk and runtime:

      $ flatpak install org.kde.Sdk//6.8 org.kde.Platform//6.8

After install sdk and runtime, Run again:

      $ flatpak-builder --force-clean build-dir RabbitRemoteControl/io.github.KangLin.RabbitRemoteControl.yml
      # or user
      $ flatpak-builder --user --force-clean --install build-dir /home/RabbitRemoteControl/Package/io.github.KangLin.RabbitRemoteControl.yml 

- Install

      $ flatpak-builder --user --force-clean --install build-dir RabbitRemoteControl/io.github.KangLin.RabbitRemoteControl.yml

- Run

      $ flatpak run io.github.KangLin.RabbitRemoteControl

- Issue
  - Fuse issue in docker

    When creating a container, add the parameter: `--privileged`

        docker run --privileged -v /home/RabbitRemoteControl:/home/RabbitRemoteControl -it ubuntu

  - QtWebEngine: https://github.com/flathub/io.qt.qtwebengine.BaseApp

- Documents
  - [flatpak manifest](https://docs.flatpak.org/en/latest/flatpak-builder-command-reference.html#flatpak-manifest)
  - [Sandbox Permissions](https://docs.flatpak.org/en/latest/sandbox-permissions.html)
  - [Submission](https://docs.flathub.org/docs/for-app-authors/submission)

### Script

- Build script
  - [build_linux.sh](../../Script/build_linux.sh): Build the project.
  - [build_depend.sh](../../Script/build_depend.sh): Build, Install the dependency libraries
  - [build_debpackage.sh](../../Script/build_debpackage.sh): Build deb package.
    Before executing, set environment variables.
    See: [CMake parameters or environment variable](#CMake-parameters-or-environment-variable)
  - [build_appimage.sh](../../Script/build_appimage.sh): Build AppImage package.
    Before executing, set environment variables.
    See: [CMake parameters or environment variable](#CMake-parameters-or-environment-variable)
  - [build_linux.sh](../../Script/build_linux.sh): Build for linux.
  - example
    - Use build_linux.sh
    
          # Only build AppImage
          ./Script/build_linux.sh --appimage
          # Only build deb package
          ./Script/build_linux.sh --deb
          # Only build rpm package
          ./Script/build_linux.sh --rpm
          # Build deb, rpm package and AppImage with docker
          ./Script/build_linux.sh --docker --deb --appimage -rpm

    - Compiled separately

          # If use Qt be installed from [Qt](download.qt.io)
          export QT_ROOT=
          export Qt6_ROOT=$QT_ROOT
          export QMAKE=$QT_ROOT/bin/qmake
          # If use the system-packaged Qt
          export QT_SELECT=qt6

          # Note:  The following environment variables must be absolute path
          export BUILD_DIR=`pwd`/build
          export INSTALL_DIR=$BUILD_DIR/install
          export SOURCE_DIR=$BUILD_DIR/source
          export TOOLS_DIR=$BUILD_DIR/tools

          # Install dependency libraries
          sudo ./Script/build_depend.sh --apt_update --base --default --rabbitcommon \
              --tigervnc --pcapplusplus \
              --install ${INSTALL_DIR} \
              --source ${SOURCE_DIR} \
              --tools ${TOOLS_DIR} \
              --build ${BUILD_DIR}

          # Set dependency libraries environment variable
          export RabbitCommon_ROOT=${SOURCE_DIR}/RabbitCommon
          export tigervnc_DIR=${INSTALL_DIR}/lib/cmake/tigervnc
          export PcapPlusPlus_DIR=${INSTALL_DIR}/lib/cmake/pcapplusplus

          # Compile deb package
          ./Script/build_debpackage.sh

          # Compile AppImage
          ./Script/build_appimage.sh

- [deploy.sh](../../deploy.sh): Deploy. Only used by administarotr
