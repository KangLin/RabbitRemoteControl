# Author: Kang Lin <kl222@126.com>

project(RabbitRemoteControlApp
    LANGUAGES CXX
    DESCRIPTION "Rabbit remote control client application"
    )

set(SOURCE_FILES
    main.cpp
    mainwindow.cpp
    FrmFullScreenToolBar.cpp
    View.cpp
    ViewTable.cpp
    ViewSplitter.cpp
    ViewSplitterContainer.cpp
    ParameterApp.cpp
    ParameterDlgSettings.cpp
    FavoriteView.cpp
    FavoriteMimeData.cpp
    Recent/FrmRecent.cpp
    Recent/RecentModel.cpp
    Recent/RecentDatabase.cpp
    FrmActive.cpp
    )

set(HEADER_FILES
    mainwindow.h
    FrmFullScreenToolBar.h
    View.h
    ViewTable.h
    ViewSplitter.h
    ViewSplitterContainer.h
    ParameterApp.h
    ParameterDlgSettings.h
    FavoriteView.h
    FavoriteMimeData.h
    Recent/FrmRecent.h
    Recent/RecentModel.h
    Recent/RecentDatabase.h
    FrmActive.h
    )

set(SOURCE_UI_FILES
    mainwindow.ui
    FrmFullScreenToolBar.ui
    ParameterDlgSettings.ui
    )

if(WIN32)
    set(RCC_FILES Resource/App.rc)
endif()

if(LINUX)
    LIST(APPEND RCC_FILES Resource/Resource.qrc)
endif()

list(APPEND APP_INCLUDE_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Recent>)

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

set(APP_QT_COMPONENTS Core Gui Widgets Network Svg Xml Multimedia MultimediaWidgets Sql)
if(QT_VERSION_MAJOR VERSION_GREATER_EQUAL 6)
    list(APPEND APP_QT_COMPONENTS StateMachine)
endif()
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${APP_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${APP_QT_COMPONENTS})
        list(APPEND QT_LIBRARIES Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
else()
    message(FATAL_ERROR "Qt${QT_VERSION_MAJOR} don't found")
endif()

option(WITH_MACOSX_BUNDLE "Create BUNDLE directory" OFF)
if(WITH_MACOSX_BUNDLE)
    set(IS_MACOSX_BUNDLE "IS_MACOSX_BUNDLE")
endif()
set(APP_ID io.github.KangLin.RabbitRemoteControl)
ADD_TARGET(NAME ${PROJECT_NAME}
    ISEXE
    ISWINDOWS
    ${IS_MACOSX_BUNDLE}
    VERSION ${RabbitRemoteControl_VERSION}
    INCLUDE_DIRS ${APP_INCLUDE_DIRS}
    PRIVATE_DEFINITIONS RabbitRemoteControl_VERSION="${RabbitRemoteControl_VERSION}"
        RabbitRemoteControl_REVISION="${RabbitRemoteControl_REVISION}"
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${SOURCE_UI_FILES} ${RCC_FILES} ${AppImage}
    PRIVATE_LIBS Plugin ${QT_LIBRARIES}
    )

# Install other files
set(OTHER_FILES
    ${CMAKE_SOURCE_DIR}/License.md
    ${CMAKE_SOURCE_DIR}/Authors.md
    ${CMAKE_SOURCE_DIR}/ChangeLog.md
    ${CMAKE_SOURCE_DIR}/Authors_zh_CN.md
    ${CMAKE_SOURCE_DIR}/ChangeLog_zh_CN.md)
INSTALL_FILE(SOURCES ${OTHER_FILES}
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
        COMPONENT Application)

if(ANDROID)
    if(NOT EXISTS ${CMAKE_BINARY_DIR}/plugins)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/plugins)
    endif()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        QT_ANDROID_EXTRA_PLUGINS
            ${CMAKE_BINARY_DIR}/plugins
        )
endif()

if(NOT EXISTS ${CMAKE_BINARY_DIR}/etc/RabbitRemoteControl_logqt.ini)
    configure_file(${CMAKE_SOURCE_DIR}/etc/RabbitRemoteControl_logqt_debug.ini
        ${CMAKE_BINARY_DIR}/etc/RabbitRemoteControl_logqt.ini
        COPYONLY)
    INSTALL_FILE(SOURCES ${CMAKE_SOURCE_DIR}/etc/RabbitRemoteControl_logqt.ini
        DESTINATION etc
        COMPONENT Application)
endif()

if(WIN32)
    install(FILES Resource/icons/rabbit-green/ico/app.ico
        DESTINATION .
            COMPONENT Application)
elseif(APPLE)
    if(WITH_MACOSX_BUNDLE)
        # 设置 bundle 属性 (如果是 GUI 应用)
        # See: https://developer.apple.com/documentation/bundleresources/information-property-list
        set_target_properties(${PROJECT_NAME} PROPERTIES
            MACOSX_BUNDLE_GUI_IDENTIFIER "${APP_ID}"
            MACOSX_BUNDLE_ICON_FILE "${APP_ID}.icns"
            MACOSX_BUNDLE_BUNDLE_NAME "RabbitRemoteControl"
            MACOSX_BUNDLE_BUNDLE_VERSION "${RabbitRemoteControl_VERSION}"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "${RabbitRemoteControl_VERSION}"
            )
    else()
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Resource/App.Info.plist
            ${CMAKE_CURRENT_BINARY_DIR}/Info.plist
                     @ONLY)
        INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/Info.plist
            DESTINATION "." COMPONENT Application)
    endif()
    target_add_resources(${PROJECT_NAME} . "${CMAKE_CURRENT_SOURCE_DIR}/Resource/Image/${APP_ID}.icns")
elseif(UNIX AND NOT ANDROID)
    configure_file(${CMAKE_SOURCE_DIR}/share/applications/${APP_ID}.desktop.in
        ${CMAKE_BINARY_DIR}/${APP_ID}.desktop)
    INSTALL(FILES ${CMAKE_BINARY_DIR}/${APP_ID}.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
            COMPONENT Application)
    # INSTALL(PROGRAMS ${CMAKE_SOURCE_DIR}/share/applications/RabbitRemoteControl.sh
    #         DESTINATION ${CMAKE_INSTALL_BINDIR}
    #             COMPONENT Application)

    # Icon path: https://www.freedesktop.org/wiki/Specifications/icon-theme-spec/
    # The freedesktop.org standard specifies in which order and directories programs should look for icons:
    #
    #    $HOME/.icons (for backwards compatibility)
    #    $XDG_DATA_DIRS/icons
    #    /usr/share/pixmaps
    INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Resource/Image/${APP_ID}.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps
            COMPONENT Application)
    # Flatpak: https://docs.flatpak.org/en/latest/conventions.html#application-icons
    INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Resource/icons/rabbit-green/scalable/apps/${APP_ID}.svg
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps/
            COMPONENT Application
        RENAME ${APP_ID}.svg)

    string(TIMESTAMP BUILD_DATE "%Y-%m-%d")
    configure_file(${CMAKE_SOURCE_DIR}/share/metainfo/${APP_ID}.metainfo.xml.in
        ${CMAKE_BINARY_DIR}/${APP_ID}.metainfo.xml @ONLY)
    INSTALL(FILES ${CMAKE_BINARY_DIR}/${APP_ID}.metainfo.xml
        DESTINATION "${CMAKE_INSTALL_DATADIR}/metainfo"
            COMPONENT Application)
endif()

# Install QIcon theme
INSTALL_ICON_THEME(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Resource/icons)

if(ANDROID)
    find_package(QFtpServerLib)
    if(QFtpServerLib_FOUND)
        get_target_property(QFtpServerLib_Location QFtpServerLib IMPORTED_LOCATION)
        list(APPEND android_extra_libs ${QFtpServerLib_Location})
    endif()
    find_package(Qt${QT_VERSION_MAJOR}Keychain)
    if(Qt${QT_VERSION_MAJOR}Keychain_FOUND)
        get_target_property(QtKeychain_Location Qt${QT_VERSION_MAJOR}Keychain::Qt${QT_VERSION_MAJOR}Keychain IMPORTED_LOCATION)
        list(APPEND android_extra_libs ${QtKeychain_Location})
    endif()
    message("QT_ANDROID_EXTRA_LIBS: ${android_extra_libs}")
    if(android_extra_libs)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            QT_ANDROID_EXTRA_LIBS ${android_extra_libs})
    endif()
endif()
