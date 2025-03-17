# Author: Kang Lin <kl222@126.com>

project(RabbitRemoteControlApp
    LANGUAGES CXX
    DESCRIPTION "Rabbit remote control client application"
    )

set(SOURCE_FILES
    main.cpp
    mainwindow.cpp
    FrmFullScreenToolBar.cpp
    ViewTable.cpp
    View.cpp
    ParameterApp.cpp
    ParameterDlgSettings.cpp
    FavoriteView.cpp
    FavoriteMimeData.cpp
    FrmListConnects.cpp
    )

set(HEADER_FILES
    mainwindow.h
    FrmFullScreenToolBar.h
    ViewTable.h
    View.h
    ParameterApp.h
    ParameterDlgSettings.h
    FavoriteView.h
    FavoriteMimeData.h
    FrmListConnects.h
    )

set(SOURCE_UI_FILES
    mainwindow.ui
    FrmFullScreenToolBar.ui
    ParameterDlgSettings.ui
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

set(APP_QT_COMPONENTS Core Gui Widgets Network Svg Xml Multimedia MultimediaWidgets)
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

ADD_TARGET(NAME ${PROJECT_NAME}
    ISEXE
    ISWINDOWS
    VERSION ${RabbitRemoteControl_VERSION}
    PRIVATE_DEFINITIONS RabbitRemoteControl_VERSION="${RabbitRemoteControl_VERSION}"
      RabbitRemoteControl_REVISION="${RabbitRemoteControl_REVISION}"
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${SOURCE_UI_FILES} ${RCC_FILES}
    PRIVATE_LIBS Client ${QT_LIBRARIES})

# Install other files
set(OTHER_FILES
    ${CMAKE_SOURCE_DIR}/License.md
    ${CMAKE_SOURCE_DIR}/Authors.md
    ${CMAKE_SOURCE_DIR}/ChangeLog.md
    ${CMAKE_SOURCE_DIR}/Authors_zh_CN.md
    ${CMAKE_SOURCE_DIR}/ChangeLog_zh_CN.md)
INSTALL_FILE(SOURCES  ${OTHER_FILES}
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
        COMPONENT Application)

if(ANDROID)
    if(NOT EXISTS ${CMAKE_BINARY_DIR}/plugins/Client)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/plugins/Client)
    endif()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        QT_ANDROID_EXTRA_PLUGINS
            ${CMAKE_BINARY_DIR}/plugins/Client
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
endif()
if(UNIX AND NOT ANDROID)
    set(APP_ID io.github.KangLin.RabbitRemoteControl)
    configure_file(${CMAKE_SOURCE_DIR}/share/applications/${APP_ID}.desktop.in
        ${CMAKE_BINARY_DIR}/${APP_ID}.desktop)
    INSTALL(FILES ${CMAKE_BINARY_DIR}/${APP_ID}.desktop
        DESTINATION share/applications
            COMPONENT Application)
    INSTALL(PROGRAMS ${CMAKE_SOURCE_DIR}/share/applications/RabbitRemoteControl.sh
            DESTINATION ${CMAKE_INSTALL_BINDIR}
                COMPONENT Application)
    INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Resource/icons/rabbit-green/svg/app.svg
        DESTINATION share/icons/hicolor/scalable/apps
            COMPONENT Application
        RENAME ${APP_ID}.svg)
endif()

# Install QIcon theme
INSTALL_ICON_THEME(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Resource/icons)
