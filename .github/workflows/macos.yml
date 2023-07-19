name: macos

on:
  workflow_call:
    outputs:
      name:
        description: "The artifact name"
        value: ${{ jobs.build_macos.outputs.name }}

env:
  GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}

jobs:
  build_macos:
    strategy:
      matrix:
        # Customize the CMake build type here (Release, Debug, RelWithDebInfo, etc.)
        BUILD_TYPE: [Release, Debug]
        qt_version: [6.6.0, 6.5.1, 5.15.2, 5.12.12]
        include:
          - qt_version: 6.6.0
            qt_modules: qtscxml qtmultimedia qtserialport qtimageformats qtwebchannel qtwebsockets qtwebview
          
          - qt_version: 6.5.1
            qt_modules: qtscxml qtmultimedia qtserialport qtimageformats qtwebchannel qtwebsockets qtwebview

          - qt_version: 5.15.2

          - qt_version: 5.12.12

    runs-on: macos-latest

    env:
      BUILD_TYPE:  ${{ matrix.BUILD_TYPE }}
      SOURCE_DIR:  ${{github.workspace}}/.cache/source
      TOOSL_DIR:   ${{github.workspace}}/.cache/tools
      INSTALL_DIR: ${{github.workspace}}/.cache/install_macos_qt${{matrix.qt_version}}_${{matrix.BUILD_TYPE}}
      qt_modules: qtwebengine ${{matrix.qt_modules}}
      RabbitRemoteControl_VERSION: v1.0.9
      VCPKGGITCOMMITID: 980ec0f49d81f794778386f7bd518b2cc9c092f0
      artifact_name: build_macos

    # Map the job outputs to step outputs
    outputs:
      name: ${{ env.artifact_name }}

    steps:
      - name: Checkout Repository
        uses: actions/checkout@v3
        with:
          submodules: true
    
      - name: Make directories
        run: |
          cmake -E make_directory ${{env.SOURCE_DIR}}
          cmake -E make_directory ${{env.TOOSL_DIR}}
          cmake -E make_directory ${{env.INSTALL_DIR}}
          cmake -E make_directory ${{github.workspace}}/build

      - name: Cache installed
        uses: actions/cache@v3
        id: cache-installed
        with:
          path: |
            ${{env.INSTALL_DIR}}
          key: Cache-installed-macos

      - name: run-vcpkg
        uses: lukka/run-vcpkg@v7.4
        with:
          vcpkgGitCommitId: ${{env.VCPKGGITCOMMITID}}
          vcpkgDirectory: ${{runner.workspace}}/vcpkg/
          # Since the cache must be invalidated when content of the vcpkg.json file changes, let's
          # compute its hash and append this to the computed cache's key.
          appendedCacheKey: cache-vcpkg-${{env.VCPKGGITCOMMITID}}
          vcpkgArguments: 'openssl libssh libpng pixman libjpeg-turbo libdatachannel freerdp'
          
#      - name: build FreeRDP
#        working-directory: ${{env.SOURCE_DIR}}
#        run: |
#          if [ ! -d ${{env.INSTALL_DIR}}/lib/cmake/FreeRDP3 ]; then
#            git clone https://github.com/FreeRDP/FreeRDP.git
#            # git clone https://github.com/KangLin/FreeRDP.git
#            cd FreeRDP
#            git checkout -b 04fa5b3033722df4fba45c02056be8e7cb45d7c6 04fa5b3033722df4fba45c02056be8e7cb45d7c6
#            git submodule update --init --recursive
#            cmake -E make_directory build
#            cd build
#            cmake .. \
#                -DCMAKE_BUILD_TYPE=${{matrix.BUILD_TYPE}} \
#                -DCMAKE_INSTALL_PREFIX="${{env.INSTALL_DIR}}" \
#                -DWITH_SERVER=ON \
#                -DWITH_CLIENT_SDL=OFF \
#                -DWITH_KRB5=OFF \
#                -DWITH_MANPAGES=OFF \
#                -DCMAKE_TOOLCHAIN_FILE="${{env.VCPKG_ROOT}}/scripts/buildsystems/vcpkg.cmake"
#            cmake --build . --config ${{matrix.BUILD_TYPE}}
#            cmake --build . --config ${{matrix.BUILD_TYPE}} --target install/strip
#          fi

      - name: build RabbitVNC
        working-directory: ${{env.SOURCE_DIR}}
        run: |
          if [ ! -d ${{env.INSTALL_DIR}}/lib/cmake/RabbitVNC ]; then
            git clone https://github.com/KangLin/RabbitVNC.git 
            cd ${{env.SOURCE_DIR}}/RabbitVNC
            cmake -E make_directory build
            cd build
            cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.BUILD_TYPE }} \
                -DCMAKE_INSTALL_PREFIX=${{ env.INSTALL_DIR }} \
                -DBUILD_TESTS=OFF -DBUILD_VIEWER=OFF \
                -DCMAKE_TOOLCHAIN_FILE="${{env.VCPKG_ROOT}}/scripts/buildsystems/vcpkg.cmake"
            cmake --build . --config ${{ matrix.BUILD_TYPE }}
            cmake --build . --config ${{ matrix.BUILD_TYPE }} --target install/strip
          fi

      - name: build tigervnc
        working-directory: ${{env.SOURCE_DIR}}
        run: |
          if [ ! -d ${{env.INSTALL_DIR}}/lib/cmake/tigervnc ]; then
            git clone https://github.com/KangLin/tigervnc.git 
            cd ${{env.SOURCE_DIR}}/tigervnc
            cmake -E make_directory build
            cd build
            cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.BUILD_TYPE }} \
                -DCMAKE_INSTALL_PREFIX=${{ env.INSTALL_DIR }} \
                -DBUILD_TESTS=OFF -DBUILD_VIEWER=OFF \
                -DCMAKE_TOOLCHAIN_FILE="${{env.VCPKG_ROOT}}/scripts/buildsystems/vcpkg.cmake"
            cmake --build . --config ${{ matrix.BUILD_TYPE }}
            cmake --build . --config ${{ matrix.BUILD_TYPE }} --target install/strip
          fi

#      - name: build libdatachannel
#        working-directory: ${{env.SOURCE_DIR}}
#        run: |
#          if [ ! -d ${{env.INSTALL_DIR}}/lib/cmake/LibDataChannel ]; then
#            git clone -b v0.17.8 https://github.com/paullouisageneau/libdatachannel.git
#            cd libdatachannel
#            git submodule update --init --recursive
#            cmake -E make_directory build
#            cd build
#            cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.BUILD_TYPE }} \
#                -DCMAKE_INSTALL_PREFIX=${{ env.INSTALL_DIR }} \
#                -DCMAKE_TOOLCHAIN_FILE="${{env.VCPKG_ROOT}}/scripts/buildsystems/vcpkg.cmake"
#            cmake --build . --config ${{ matrix.BUILD_TYPE }}
#            cmake --build . --config ${{ matrix.BUILD_TYPE }} --target install/strip
#          fi

      - name: Cache Qt
        id: cache-qt
        uses: actions/cache@v1  # not v2!
        with:
          path: ${{env.TOOSL_DIR}}/qt
          key: qt${{matrix.qt_version}}-${{matrix.qt_arch}}
        
      - name: Install Qt
        uses: jurplel/install-qt-action@v3
        with:
          dir: '${{env.TOOSL_DIR}}/qt' # optional
          version: '${{matrix.qt_version}}' # optional, default is 5.15.2
          arch: '${{matrix.qt_arch}}' # optional
          modules: '${{env.qt_modules}}' # optional
          cached: '${{steps.cache-qt.outputs.cache-hit}}' # optional, default is false
 
      - name: build qxmpp
        working-directory: ${{env.SOURCE_DIR}}
        run: |
          if [ ! -d ${{env.INSTALL_DIR}}/lib/cmake/qxmpp ]; then
            git clone -b v1.4.0 "https://github.com/qxmpp-project/qxmpp.git"
            cd qxmpp
            cmake -E make_directory build
            cd build
            cmake .. \
                -DCMAKE_BUILD_TYPE=${{matrix.BUILD_TYPE}} \
                -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=OFF \
                -DCMAKE_INSTALL_PREFIX=${{env.INSTALL_DIR}} \
                -DCMAKE_TOOLCHAIN_FILE=${{env.VCPKG_ROOT}}/scripts/buildsystems/vcpkg.cmake
            cmake --build . --config ${{matrix.BUILD_TYPE}}
            cmake --build . --config ${{matrix.BUILD_TYPE}} --target install
          fi

      - name: build QtService
        working-directory: ${{env.SOURCE_DIR}}
        env:
          QT_ROOT: /usr/lib/`uname -m`-linux-gnu/qt5
        run: |
          if [ ! -d ${{env.INSTALL_DIR}}/lib/cmake/QtService ]; then
            git clone https://github.com/KangLin/qt-solutions.git
            cd qt-solutions/qtservice
            git submodule update --init --recursive
            cmake -E make_directory build
            cd build
            cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.BUILD_TYPE }} \
                -DCMAKE_INSTALL_PREFIX=${{ env.INSTALL_DIR }} \
                -DQT_DIR=${{env.QT_ROOT}}/lib/cmake/Qt5 \
                -DQt5_DIR=${{env.QT_ROOT}}/lib/cmake/Qt5 \
                -DCMAKE_TOOLCHAIN_FILE="${{env.VCPKG_ROOT}}/scripts/buildsystems/vcpkg.cmake"
            cmake --build . --config ${{ matrix.BUILD_TYPE }}
            cmake --build . --config ${{ matrix.BUILD_TYPE }} --target install/strip
          fi

      - name: git clone RabbitCommon
        working-directory: ${{env.SOURCE_DIR}}
        run: git clone https://github.com/KangLin/RabbitCommon.git

      - name: Build RabbitRemoteControl
        working-directory: ${{github.workspace}}/build
        env:
          RabbitCommon_DIR: ${{env.SOURCE_DIR}}/RabbitCommon
        run: |
          cmake ${{github.workspace}} \
              -DCMARK_SHARED=OFF \
              -DCMARK_TESTS=OFF \
              -DCMARK_STATIC=ON \
              -DCMAKE_BUILD_TYPE=${{ matrix.BUILD_TYPE }} \
              -DCMAKE_INSTALL_PREFIX=${{ github.workspace }}/build/install \
              -DCMAKE_PREFIX_PATH=${{env.INSTALL_DIR}}/lib/cmake \
              -DCMAKE_TOOLCHAIN_FILE="${{env.VCPKG_ROOT}}/scripts/buildsystems/vcpkg.cmake" \
              -DBUILD_LibVNCServer=OFF \
              -DBUILD_FREERDP=ON \
              -DLibVNCServer_DIR="${{env.INSTALL_DIR}}/lib/cmake/LibVNCServer" \
              -DRabbitVNC_DIR=${{env.INSTALL_DIR}}/lib/cmake/RabbitVNC \
              -Dtigervnc_DIR=${{env.INSTALL_DIR}}/lib/cmake/tigervnc \
              -DQXmpp_DIR=${{env.INSTALL_DIR}}/lib/cmake/qxmpp \
              -DQtService_DIR=${{env.INSTALL_DIR}}/lib/cmake/QtService \
              -DCMAKE_TOOLCHAIN_FILE=${{env.VCPKG_ROOT}}/scripts/buildsystems/vcpkg.cmake \
              -DFreeRDP_DIR=${{env.INSTALL_DIR}}/lib/cmake/FreeRDP2 \
              -DFreeRDP-Client_DIR=${{env.INSTALL_DIR}}/lib/cmake/FreeRDP-Client2 \
              -DWinPR_DIR=${{env.INSTALL_DIR}}/lib/cmake/WinPR2 \
              -DFreeRDP-Shadow_DIR=${{env.INSTALL_DIR}}/lib/cmake/FreeRDP-Shadow2 \
              -DFreeRDP-Server_DIR=${{env.INSTALL_DIR}}/lib/cmake/FreeRDP-Server2
          cmake --build . --config ${{ matrix.BUILD_TYPE }} --target install
          cmake --build . --config ${{ matrix.BUILD_TYPE }} --target package
          7z a RabbitRemoteControl_${{env.RabbitRemoteControl_VERSION}}_macos.zip ./install/*

      - name: Update artifact
        if: ${{ matrix.BUILD_TYPE == 'Release' }} 
        uses: actions/upload-artifact@v3
        with:
          name: ${{ env.artifact_name }}
          path: |
            ${{github.workspace}}/build/RabbitRemoteControl_${{env.RabbitRemoteControl_VERSION}}_macos.zip
