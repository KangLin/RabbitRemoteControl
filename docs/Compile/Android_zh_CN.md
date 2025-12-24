## 为 Android 编译

作者：康林 <kl222@126.com>

### 环境
#### 操作系统

- Windows 10 版本 20H2 (19045.2604)
- Ubuntu 22.04.4 LTS

#### Qt Creator

当前版本：v18.0.0 。建议使用 v5.0.2 及以后版本，以前版本对 CMake 支持不够。

### 工具

- 编译器
  - [Android SDK](https://developer.android.google.cn/studio?hl=zh-cn)
    - 当前版本: 9.0
  - [Android NDK](https://developer.android.google.cn/ndk/downloads?hl=en)
    - 当前版本：20.0.5594570
  - [Java](https://www.java.com/zh-CN/)
- Qt
  - Qt 官方发行版本：https://download.qt.io/official_releases/qt/  
    当前使用版本：Qt 6.9.3
    详见： https://doc.qt.io/qt-6/android.html
  - IDE：Qt Creator。建议使用 v5.0.2 及以后版本，以前版本对 CMake 支持不够。
    当前使用版本：18.0.0
- Git: [https://www.git-scm.com/](https://www.git-scm.com/)  
  [Git 设置](http://blog.csdn.net/kl222/article/details/32903495)
- CMake: [https://www.cmake.org/](https://cmake.org/)
  版本: 3.27.0
- Doxygen: [http://www.doxygen.nl/](http://www.doxygen.nl/)
- [ninja](https://ninja-build.org/)
- vcpkg: [https://github.com/microsoft/vcpkg](https://github.com/microsoft/vcpkg)

### 编译

参见：[编译集成](../../.github/workflows/android.yml)

#### 依赖库

- [必选] 玉兔公共库: [https://github.com/KangLin/RabbitCommon](https://github.com/KangLin/RabbitCommon)
- [必选] [Openssl for Qt:](https://github.com/KDAB/android_openssl) 运行 Qt 需要
- [可选] RFB
  - [可选] RabbitVNC: [https://github.com/KangLin/RabbitVNC](https://github.com/KangLin/RabbitVNC)
  - [可选] LibVNCServer: [https://github.com/LibVNC/libvncserver](https://github.com/LibVNC/libvncserver)
  - [可选] TigerVNC: [https://github.com/KangLin/tigervnc](https://github.com/KangLin/tigervnc)
- [可选] FreeRDP: [https://github.com/FreeRDP/FreeRDP](https://github.com/FreeRDP/FreeRDP)
- [可选] SSH
  - libssh: libssh: [https://www.libssh.org](https://www.libssh.org)
  - libssh2:
    - [https://www.libssh2.org](libssh2: https://www.libssh2.org/)
    - [https://github.com/libssh2/libssh2](https://github.com/libssh2/libssh2/)
- [可选] QTermWidget: [https://github.com/lxqt/qtermwidget](https://github.com/lxqt/qtermwidget)
- [可选] libtelnet: [https://github.com/seanmiddleditch/libtelnet](https://github.com/seanmiddleditch/libtelnet)
- [可选] libdatachannel: [https://github.com/paullouisageneau/libdatachannel](https://github.com/paullouisageneau/libdatachannel)
- [可选] QXmpp: [https://github.com/qxmpp-project/qxmpp](https://github.com/qxmpp-project/qxmpp)
- [可选] QtService: [https://github.com/KangLin/qt-solutions](https://github.com/KangLin/qt-solutions)
- [可选] PcapPlusPlus: [https://github.com/seladb/PcapPlusPlus](https://github.com/seladb/PcapPlusPlus)
- [可选] [FFMPEG:](https://ffmpeg.org/) 多媒体功能需要
- [可选] qtkeychain: [https://github.com/KangLin/qtkeychain](https://github.com/KangLin/qtkeychain)
- [可选] libcurl: [https://curl.se](https://curl.se)
- [可选] QFtpServer: https://github.com/KangLin/QFtpServer

##### 设置全局变量

- windows

      set INSTALL_DIR=`pwd`/install
      set VCPKG_ROOT=/data/vcpkg
      set VCPKG_TARGET_TRIPLET=x64-android
      set ANDROID_ABI=x86_64
      set ANDROID_PLATFORM=android-23
      set ANDROID_NATIVE_API_LEVEL=23
      set ANDROID_NDK_HOME=
      set BUILD_TYPE=Release

  引用变量使用 %% 。例如： %BUILD_TYPE%
  
- ubuntu

      export INSTALL_DIR=`pwd`/install
      export VCPKG_ROOT=/data/vcpkg
      export VCPKG_TARGET_TRIPLET=x64-android
      export ANDROID_ABI=x86_64
      export ANDROID_PLATFORM=android-23
      export ANDROID_NATIVE_API_LEVEL=23
      export ANDROID_NDK_HOME=
      export BUILD_TYPE=Release
      
引用变量使用 ${} 。例如： ${BUILD_TYPE}

- CMAKE android 参数说明：https://developer.android.google.cn/ndk/guides/cmake
  + ANDROID_ABI: 可取下列值：
    目标 ABI。如果未指定目标 ABI，则 CMake 默认使用 armeabi-v7a。  
    有效的目标名称为：
    - armeabi：带软件浮点运算并基于 ARMv5TE 的 CPU。
    - armeabi-v7a：带硬件 FPU 指令 (VFPv3_D16) 并基于 ARMv7 的设备。
    - armeabi-v7a with NEON：与 armeabi-v7a 相同，但启用 NEON 浮点指令。这相当于设置 -DANDROID_ABI=armeabi-v7a 和 -DANDROID_ARM_NEON=ON。
    - arm64-v8a：ARMv8 AArch64 指令集。
    - x86：IA-32 指令集。
    - x86_64 - 用于 x86-64 架构的指令集。
  + ANDROID_NDK <path> 主机上安装的 NDK 根目录的绝对路径
  + ANDROID_PLATFORM: 如需平台名称和对应 Android 系统映像的完整列表，请参阅 [Android NDK 原生 API](https://developer.android.google.cn/ndk/guides/stable_apis.html)
  + ANDROID_ARM_MODE
  + ANDROID_ARM_NEON
  + ANDROID_STL: 指定 CMake 应使用的 STL
    - c++_shared: 使用 libc++ 动态库
    - c++_static: 使用 libc++ 静态库
    - none: 没有 C++ 库支持
    - system: 用系统的 STL

##### 编译 TigerVNC

- 下载源码： `git clone https://github.com/KangLin/tigervnc`
- cmake 配置
    
      cmake .. \
         -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
         -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
         -DBUILD_SHARED_LIBS=OFF \
         -DCMAKE_VERBOSE_MAKEFILE=ON \
         -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
         -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=${ANDROID_NDK_HOME}/build/cmake/android.toolchain.cmake \
         -DVCPKG_VERBOSE=ON \
         -DVCPKG_TRACE_FIND_PACKAGE=ON \
         -DVCPKG_TARGET_TRIPLET=${VCPKG_TARGET_TRIPLET} \
         -DX_VCPKG_APPLOCAL_DEPS_INSTALL=ON \
         -DVCPKG_INSTALLED_DIR=${INSTALL_DIR}/tigervnc/vcpkg_installed \
         -DANDROID_ABI=${ANDROID_ABI} \
         -DANDROID_PLATFORM=${ANDROID_PLATFORM} \
         -DBUILD_TESTS=OFF \
         -DBUILD_VIEWER=OFF

- 编译

      cmake --build .
      
- 安装

      cmake --install .

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

#### 编译本项目

- 项目位置：[https://github.com/KangLin/RabbitRemoteControl](https://github.com/KangLin/RabbitRemoteControl)
- 下载源码

      git clone https://github.com/KangLin/RabbitRemoteControl.git

- CMake 参数
  - QT_ROOT: Qt 安装位置
  - Qt6_DIR: 与 QT_ROOT 相同
  - RabbitCommon_ROOT：RabbitCommon 源码位置
  - BUILD_CLIENT: 编译客户端。默认为 ON
  - BUILD_SERVICE: 编译服务器端。默认依赖是否有 QtService
  - BUILD_PLUGINS: 编译插件。默认为 ON
  - BUILD_APP: 编译应用程序。默认为 ON
  - BUILD_DOCS: 编译文档。默认为 OFF
  - BUILD_SHARED_LIBS: 编译动态库。默认为 ON
  - BUILD_FREERDP：是否编译 FreeRDP。 默认为 OFF
  - WinPR_DIR: [FreeRDP 安装目录]/lib/cmake/WinPR3
  - FreeRDP_DIR: [FreeRDP 安装目录]/lib/cmake/FreeRDP3
  - FreeRDP-Client_DIR: [FreeRDP 安装目录]/lib/cmake/FreeRDP-Client3
  - BUILD_RABBITVNC: 编译 RabbitVNC 插件。默认为 ON
  - RabbitVNC_DIR: [RabbitVNC 安装目录]/lib/cmake/RabbitVNC
  - BUILD_TigerVNC: 编译 TigerVNC。默认为 ON
  - tigervnc_DIR: [TigerVNC 安装目录]/lib/cmake/tigervnc
  - BUILD_LibVNCServer: 编译 LibVNCServer。默认为 ON
  - LibVNCServer_DIR: [libvncserver 安装目录]/lib/cmake/LibVNCServer
  - libdatachannel_DIR: [libdatachannel 安装目录]/lib/cmake/LibDataChannel
  - QXmpp_DIR=[QXmpp 安装目录]/lib/cmake/qxmpp
  - qtermwidget5_DIR: [QTermWidget 安装目录]/lib/cmake/qtermwidget5
  - libssh_DIR: [libssh 安装目录]/lib/cmake/libssh
  - QtService_DIR: [QtService 安装目录]/lib/cmake/QtService
- 如果使用 vcpkg，增加下面参数
  - 因为使用了 vcpkg 清单模式，所以依赖库在 `vcpkg.json` 中。
  - CMAKE_TOOLCHAIN_FILE: [vcpkg 安装目录]/scripts/buildsystems/vcpkg.cmake
  - X_VCPKG_APPLOCAL_DEPS_INSTALL: ON  #安装时，把把依赖库的复制到安装目录中
  - VCPKG_MANIFEST_FEATURES: vcpkg 中的清单功能。
        如果它要放到CMakeLists.txt文件中，则必须入在最顶端 project 之前才能生效。
        所以本项目中从环境变量或 CMake 参数传入。
        参见： https://learn.microsoft.com/vcpkg/users/buildsystems/cmake-integration#settings-reference
- 编译
  - 命令行编译
    - 设置环境变量
      - ubuntu

            export JAVA_HOME=
            export QT_ROOT=
            export Qt6_DIR=
            export ANDROID_SDK_ROOT=
            export ANDROID_NDK_ROOT=
            export ANDROID_NDK_HOME=%ANDROID_NDK_ROOT%
            export CMAKE_ROOT=
            export NINJA_ROOT=
            export PATH=%CMAKE_ROOT%/bin;%NINJA_ROOT%;%JAVA_HOME%/bin;%PATH%
            export VCPKG_ROOT=
            export VCPKG_TARGET_TRIPLET=x64-android
            export BUILD_TYPE=Release

      - windows

            set JAVA_HOME=
            set QT_ROOT=
            set Qt6_DIR=
            set ANDROID_SDK_ROOT=
            set ANDROID_NDK_ROOT=
            set ANDROID_NDK_HOME=%ANDROID_NDK_ROOT%
            set CMAKE_ROOT=
            set NINJA_ROOT=
            set PATH=%CMAKE_ROOT%/bin;%NINJA_ROOT%;%JAVA_HOME%/bin;%PATH%
            set VCPKG_ROOT=
            set VCPKG_TARGET_TRIPLET=x64-android
            set BUILD_TYPE=Release

    - 编译

          mkdir build
          ${Qt6_DIR}/bin/qt-cmake .. \
              -DCMARK_SHARED=OFF \
              -DCMARK_TESTS=OFF \
              -DCMARK_STATIC=ON \
              -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
              -DCMAKE_AUTOGEN_VERBOSE=ON \
              -DQT_CHAINLOAD_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
              -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=${ANDROID_NDK_HOME}/build/cmake/android.toolchain.cmake \
              -DVCPKG_TARGET_TRIPLET=${VCPKG_TARGET_TRIPLET} \
              -DX_VCPKG_APPLOCAL_DEPS_INSTALL=ON \
              -DVCPKG_TRACE_FIND_PACKAGE=ON \
              -DVCPKG_VERBOSE=ON \
              -DRABBIT_ENABLE_INSTALL_DEPENDENT=ON \
              -DRABBIT_ENABLE_INSTALL_QT=ON \
              -DRABBIT_ENABLE_INSTALL_TO_BUILD_PATH=ON \
              -DQT_HOST_PATH=${Qt6_DIR}/../gcc_64 \
              -DQT_ANDROID_SIGN_APK=ON \
              -DQT_ENABLE_VERBOSE_DEPLOYMENT=ON \
              -DVCPKG_TRACE_FIND_PACKAGE=ON \
              -DQt6LinguistTools_DIR=${Qt6_DIR}/../gcc_64/lib/cmake/Qt6LinguistTools \
              -DCMAKE_INSTALL_PREFIX=`pwd`/install \
              [Depend libraries ......]
          cmake --build . --verbose --config Release --target all

    - 参见：
      - [编译集成 android.yml](../../.github/workflows/android.yml)
  - 使用 CMakePreset
    - 设置环境变量
      - windows
      
            set JAVA_HOME=
            set ANDROID_SDK_ROOT=
            set ANDROID_NDK_ROOT=
            set ANDROID_NDK_HOME=%ANDROID_NDK_ROOT%
            set CMAKE_ROOT=
            set NINJA_ROOT=
            set PATH=%CMAKE_ROOT%/bin;%NINJA_ROOT%;%JAVA_HOME%/bin;%PATH%
            set VCPKG_ROOT=
            set QT_ROOT=
            set Qt6_DIR=
          
      - ubuntu

            export JAVA_HOME=
            export ANDROID_SDK_ROOT=
            export ANDROID_NDK_ROOT=
            export ANDROID_NDK_HOME=%ANDROID_NDK_ROOT%
            export CMAKE_ROOT=
            export NINJA_ROOT=
            export PATH=%CMAKE_ROOT%/bin;%NINJA_ROOT%;%JAVA_HOME%/bin;%PATH%
            export VCPKG_ROOT=
            export QT_ROOT=
            export Qt6_DIR=

    - 编译

          cd RabbitRemoteControl
          cmake --list-presets                             ;查看 presets
          cmake --preset "android-x86_64-qt6" \
              -DQt6LinguistTools_DIR=${Qt6_DIR}/../gcc_64/lib/cmake/Qt6LinguistTools \
              -DQT_HOST_PATH=${Qt6_DIR}/../gcc_64 \
              [Depend libraries ......]
          cmake --build --preset "android-x86_64-qt6"

    - 参见：
      - [CMakePreset.json](../../CMakePreset.json)
  - 使用 QtCreator 编译
  
    因为 QtCreator 14 中对 VCPKG ，在 Android 上支持不完善，
    所以需要通过设置  QT_CREATOR_SKIP_VCPKG_SETUP=ON，来关闭 VCPKG。
    当然，也可以关闭 CMake 包管理器：
    Qt Creator’s Options -> Build & Run -> CMake and then
    uncheck “Package manager auto-setup”.
    - 参见：[Qt Creator: CMake package-manager auto-setup](https://www.qt.io/blog/qt-creator-cmake-package-manager-auto-setup)
