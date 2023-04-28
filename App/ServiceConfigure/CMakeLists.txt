# Author: Kang Lin <kl222@126.com>

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "verbose")

project(RabbitRemoteControlServiceConfigure LANGUAGES CXX)

if(WIN32)
    set(RabbitRemoteControlServiceConfigure_RCC_FILES Resource/App.rc)
endif()

find_package(QtService)
if(NOT QtService_FOUND)
    return()
endif()
message("QtServeice is found")

SET(RabbitRemoteControlServiceConfigure_LIBS RabbitCommon)
list(APPEND RabbitRemoteControlServiceConfigure_LIBS Service)

set(HEADER_FILES 
    MainWindow.h
    )
set(SOURCE_FILES
    MainWindow.cpp
    main.cpp
    )
set(SOURCE_UI_FILES MainWindow.ui)
LIST(APPEND RabbitRemoteControlServiceConfigure_RCC_FILES ../Client/Resource/Resource.qrc)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets)
message("RabbitRemoteControlServiceConfigure_LIBS:${RabbitRemoteControlServiceConfigure_LIBS}")
ADD_TARGET(NAME ${PROJECT_NAME}
    ISEXE
    ISWINDOWS
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${SOURCE_UI_FILES} ${RabbitRemoteControlServiceConfigure_RCC_FILES}
    PRIVATE_DEFINITIONS RabbitRemoteControl_VERSION="${RabbitRemoteControl_VERSION}"
        RabbitRemoteControl_REVISION="${RabbitRemoteControl_REVISION}"
    INCLUDE_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # See: http://www.it1352.com/478094.html
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE_LIBS ${RabbitRemoteControlServiceConfigure_LIBS}
    VERSION ${RabbitRemoteControl_VERSION})

if(UNIX AND NOT ANDROID)
    # Install .desktop file
    set(APP_ICON_NAME org.Rabbit.RemoteControl.Service.Configure)
    configure_file(${CMAKE_SOURCE_DIR}/share/applications/org.Rabbit.RemoteControl.Service.Configure.desktop.in
        ${CMAKE_BINARY_DIR}/org.Rabbit.RemoteControl.Service.Configure.desktop)
    INSTALL(FILES ${CMAKE_BINARY_DIR}/org.Rabbit.RemoteControl.Service.Configure.desktop
        DESTINATION share/applications
            COMPONENT Runtime)
    # Install icon of .desktop
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/App/Client/Resource/Image/system-settings.svg
        DESTINATION share/pixmaps
            COMPONENT Runtime
        RENAME ${APP_ICON_NAME}.svg)
endif()

if(Log4Qt_FOUND)
    if(NOT EXISTS ${CMAKE_BINARY_DIR}/etc/RabbitRemoteControl_log4qt.conf)
        configure_file(${CMAKE_SOURCE_DIR}/etc/RabbitRemoteControl_log4qt.debug.conf
            ${CMAKE_BINARY_DIR}/etc/RabbitRemoteControl_log4qt.conf
            COPYONLY)
    endif()
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/etc/RabbitRemoteControl_log4qt.conf
        DESTINATION etc
        COMPONENT Runtime)
else()
    if(NOT EXISTS ${CMAKE_BINARY_DIR}/etc/RabbitRemoteControl_logqt.ini)
        configure_file(${CMAKE_SOURCE_DIR}/etc/RabbitRemoteControl_logqt.ini
            ${CMAKE_BINARY_DIR}/etc/RabbitRemoteControl_logqt.ini
            COPYONLY)
    endif()
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/etc/RabbitRemoteControl_logqt.ini
        DESTINATION etc
        COMPONENT Runtime)
endif()
