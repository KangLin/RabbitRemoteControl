# Author: Kang Lin <kl222@126.com>

cmake_minimum_required(VERSION 3.5)

project(RabbitRemoteControlApp LANGUAGES CXX)

set(SOURCE_FILES
    main.cpp
    mainwindow.cpp
    FrmFullScreenToolBar.cpp
    ViewTable.cpp
    View.cpp
    FrmViewScroll.cpp
    ParameterApp.cpp
    DlgSettings.cpp
    DockWdgFavorite.cpp
    )

set(HEADER_FILES
    mainwindow.h
    FrmFullScreenToolBar.h
    ViewTable.h
    View.h
    FrmViewScroll.h
    ParameterApp.h
    DlgSettings.h
    DockWdgFavorite.h
    )

set(SOURCE_UI_FILES
    mainwindow.ui
    FrmFullScreenToolBar.ui
    DlgSettings.ui
    DockWdgFavorite.ui
    )

if(WIN32)
    set(RCC_FILES Resource/App.rc)
endif()
LIST(APPEND RCC_FILES Resource/Resource.qrc)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

ADD_TARGET(NAME ${PROJECT_NAME}
    ISEXE
    ISWINDOWS
    VERSION ${BUILD_VERSION}
    PRIVATE_DEFINITIONS BUILD_VERSION="${BUILD_VERSION}"
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${SOURCE_UI_FILES} ${RCC_FILES}
    PRIVATE_LIBS Viewer)

if(WIN32)
    install(FILES Resource/Image/App.ico
        DESTINATION .
            COMPONENT Runtime)
endif()
if(UNIX AND NOT ANDROID)
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/debian/RabbitRemoteControl.desktop
        DESTINATION share/applications
            COMPONENT Runtime)
    INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Resource/Image/App.png
        DESTINATION share/pixmaps
            COMPONENT Runtime
        RENAME RabbitRemoteControl.png)
endif()
