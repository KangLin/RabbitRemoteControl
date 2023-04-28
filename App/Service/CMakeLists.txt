# Author: Kang Lin <kl222@126.com>

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "verbose")

project(RabbitRemoteControlService LANGUAGES CXX)

set(SERVICE_HEADER_FILES
    )

set(SERVICE_SOURCE_FILES
    main.cpp
    )

set(SERVICE_UI_FILES

    )

if(WIN32)
    set(RCC_FILES Resource/App.rc)
endif()

if(WITH_GUI)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)
else()
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
endif()
SET(SERVICE_LIBS RabbitCommon)

find_package(QtService)
if(NOT QtService_FOUND)
    return()
endif()
message("QtServeice is found")

list(APPEND SERVICE_LIBS Service)

ADD_TARGET(NAME ${PROJECT_NAME}
    ISEXE
    SOURCE_FILES ${SERVICE_SOURCE_FILES} ${SERVICE_HEADER_FILES} ${SERVICE_UI_FILES} ${RCC_FILES}
    PRIVATE_DEFINITIONS RabbitRemoteControl_VERSION="${RabbitRemoteControl_VERSION}"
        RabbitRemoteControl_REVISION="${RabbitRemoteControl_REVISION}"
    INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # See: http://www.it1352.com/478094.html
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE_LIBS ${SERVICE_LIBS}
    VERSION ${RabbitRemoteControl_VERSION})

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
