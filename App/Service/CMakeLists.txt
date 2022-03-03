# Author: Kang Lin <kl222@126.com>

cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "verbose")

project(RabbitRemoteControlService LANGUAGES CXX)

set(HEADER_FILES
    )

set(SOURCE_FILES
    main.cpp
    )

set(SOURCE_UI_FILES

    )

if(WIN32)
    set(RCC_FILES Resource/App.rc)
endif()

SET(SRC_LIBS RabbitCommon ${QT_LIBRARIES})

find_package(QtService)
if(NOT QtService_FOUND)
    return()
endif()
message("QtServeice is found")

list(APPEND SRC_LIBS Service)

ADD_TARGET(NAME ${PROJECT_NAME}
    ISEXE
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${SOURCE_UI_FILES} ${RCC_FILES}
    PRIVATE_DEFINITIONS BUILD_VERSION="${BUILD_VERSION}"
    INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # See: http://www.it1352.com/478094.html
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE_LIBS ${SRC_LIBS}
    VERSION ${BUILD_VERSION})
