## 为 Android 编译

作者：康林 <kl222@126.com>

### 环境
#### 操作系统

- Windows 10 版本 20H2 (19045.2604)
- Ubuntu 22.04.4 LTS

#### Qt Creator

当前版本：v13.0.0 。建议使用 v5.0.2 及以后版本，以前版本对 CMake 支持不够。

### 工具

- 编译器
  - [Android SDK](https://developer.android.google.cn/studio?hl=zh-cn)
    - 当前版本: 9.0
  - [Android NDK](https://developer.android.google.cn/ndk/downloads?hl=en)
    - 当前版本：20.0.5594570
  - [Java](https://www.java.com/zh-CN/)
- Qt
  - Qt 官方发行版本：https://download.qt.io/official_releases/qt/  
    当前使用版本：Qt 6.7.0
  - IDE：Qt Creator。建议使用 v5.0.2 及以后版本，以前版本对 CMake 支持不够。
    当前使用版本：13.0.0
- Git: [https://www.git-scm.com/](https://www.git-scm.com/)  
  [Git 设置](http://blog.csdn.net/kl222/article/details/32903495)
- CMake: [https://www.cmake.org/](https://cmake.org/)
  版本: 3.27.0
- Doxygen: [http://www.doxygen.nl/](http://www.doxygen.nl/)
- [ninja](https://ninja-build.org/)

### 编译

参见：[编译集成](../../.github/workflows/android.yml)

### 依赖库

- [必选] 玉兔公共库: [https://github.com/KangLin/RabbitCommon](https://github.com/KangLin/RabbitCommon)
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

#### 玉兔公共库
此库默认放在与本项目同级目录下，如果没有在同级目录下，则必须指定 CMake 参数:
-DRabbitCommon_DIR=[RabbitCommon 安装目录]

    git clone https://github.com/KangLin/RabbitCommon.git

### 编译本项目
- 项目位置：[https://github.com/KangLin/RabbitRemoteControl](https://github.com/KangLin/RabbitRemoteControl)
- 下载源码

      git clone https://github.com/KangLin/RabbitRemoteControl.git

- CMake 参数
  - QT_ROOT: Qt 安装位置
  - Qt6_ROOT: 与 QT_ROOT 相同
  - RabbitCommon_DIR：RabbitCommon 源码位置
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
  - CMAKE_TOOLCHAIN_FILE: [vcpkg 安装目录]/scripts/buildsystems/vcpkg.cmake
  - X_VCPKG_APPLOCAL_DEPS_INSTALL: ON  #安装时，把把依赖库的复制到安装目录中
- 编译
  - 命令行编译
    - 设置环境变量

          set JAVA_HOME=
          set ANDROID_SDK_ROOT=
          set ANDROID_NDK_ROOT=
          set ANDROID_NDK_HOME=%ANDROID_NDK_ROOT%
          set VCPKG_ROOT=
          set CMAKE_ROOT=
          set NINJA_ROOT=
          set PATH=%CMAKE_ROOT%/bin;%NINJA_ROOT%;%JAVA_HOME%/bin;%PATH%

    - 编译

          mkdir build
          ${Qt6_DIR}/bin/qt-cmake .. \
              -DCMARK_SHARED=OFF \
              -DCMARK_TESTS=OFF \
              -DCMARK_STATIC=ON \
              -DCMAKE_BUILD_TYPE=${{matrix.BUILD_TYPE}} \
              -DCMAKE_AUTOGEN_VERBOSE=ON \
              -DQT_CHAINLOAD_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
              -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=${ANDROID_NDK_HOME}/build/cmake/android.toolchain.cmake \
              -DVCPKG_TARGET_TRIPLET=${VCPKG_TARGET_TRIPLET} \
              -DX_VCPKG_APPLOCAL_DEPS_INSTALL=ON \
              -DVCPKG_TRACE_FIND_PACKAGE=ON \
              -DVCPKG_VERBOSE=ON \
              -DRABBIT_ENABLE_INSTALL_QT=ON \
              -DQT_HOST_PATH=${Qt6_DIR}/../gcc_64 \
              -DQT_ANDROID_SIGN_APK=ON \
              -DQT_ENABLE_VERBOSE_DEPLOYMENT=ON \
              -DQt6_DIR=  \
              -DQt6LinguistTools_DIR=${Qt6_DIR}/../gcc_64/lib/cmake/Qt6LinguistTools \
              -DCMAKE_INSTALL_PREFIX=`pwd`/install \
              [Depend libraries ......]
          cmake --build . --verbose --config Release --target all

  - 使用 CMakePreset
    - 设置环境变量

          set JAVA_HOME=
          set ANDROID_SDK_ROOT=
          set ANDROID_NDK_ROOT=
          set ANDROID_NDK_HOME=%ANDROID_NDK_ROOT%
          set VCPKG_ROOT=
          set CMAKE_ROOT=
          set NINJA_ROOT=
          set PATH=%CMAKE_ROOT%/bin;%NINJA_ROOT%;%JAVA_HOME%/bin;%PATH%
          set QT_ROOT=

    - 编译

          cd RabbitRemoteControl
          cmake --list-presets                             ;查看 presets
          cmake --preset "android-x86_64-qt6"
          cmake --build --preset "android-x86_64-qt6"

- 参见：
  - [CMakePreset.json](../../CMakePreset.json)
  - [编译集成 android.yml](../../.github/workflows/android.yml)
