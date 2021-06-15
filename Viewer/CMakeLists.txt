# Author: Kang Lin(kl222@126.com)

cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "verbose")

project(RabbitRemoteControl LANGUAGES CXX)

set(INSTALL_HEADER_FILES
    FrmViewer.h
    Connecter.h
    ManageConnecter.h
    PluginFactory.h
    ConnecterPlugins.h
    Parameter.h 
    )

set(HEADER_FILES
    ${INSTALL_HEADER_FILES}
    Connect.h
    ConnectThread.h
    )

set(SOURCE_FILES
    FrmViewer.cpp
    Connecter.cpp
    Connect.cpp
    ManageConnecter.cpp
    PluginFactory.cpp
    ConnecterPlugins.cpp
    ConnectThread.cpp
    Parameter.cpp  
    )

set(SOURCE_UI_FILES
    FrmViewer.ui
    )

SET(RCC_FILES )

SET(SRC_LIBS RabbitCommon ${QT_LIBRARIES})

find_package(RabbitVNC)
if(RabbitVNC_FOUND)
    LIST(APPEND SRC_LIBS ${RabbitVNC_LIBRARIES})
    INSTALL_TARGETS(TARGETS ${RabbitVNC_LIBRARIES})
    LIST(APPEND SOURCE_FILES ConnectVnc.cpp)
    LIST(APPEND HEADER_FILES ConnectVnc.h)
endif()

set(_LibDataChannel_DIR ${LibDataChannel_DIR})
find_package(LibDataChannel)
if(LibDataChannel_FOUND)
    list(APPEND SRC_LIBS LibDatachannel::datachannel)
else()
    find_path(_LibDataChannel_INCLUDE_DIRS
        NAMES "rtc/rtc.hpp"
        HINTS ${_LibDataChannel_DIR}
        PATHS ${_LibDataChannel_DIR}
        PATH_SUFFIXES include
        )
    find_library(_LibDataChannel_LIB
        NAMES datachannel
        HINTS ${_LibDataChannel_DIR}
        PATHS ${_LibDataChannel_DIR}
        PATH_SUFFIXES lib
        )
    if(_LibDataChannel_LIB)
        list(APPEND SRC_LIBS ${_LibDataChannel_LIB})
    endif()
    if(_LibDataChannel_INCLUDE_DIRS AND _LibDataChannel_LIB)
        set(LibDataChannel_FOUND TRUE)
        message("libdatachannel: ${_LibDataChannel_LIB}")
    endif()
endif()
if(LibDataChannel_FOUND)
    LIST(APPEND SOURCE_FILES
        Ice/DataChannelIce.cpp
        #Ice/DataChannelIceChannel.cpp
        Ice/IceSignal.cpp
        Ice/IceSignalWebSocket.cpp)
    LIST(APPEND HEADER_FILES
        Ice/DataChannelIce.h
        #Ice/DataChannelIceChannel.h
        Ice/IceSignal.h
        Ice/IceSignalWebSocket.h)
endif()

# Translations
GENERATED_QT_TRANSLATIONS(
    SOURCES ${SOURCE_FILES} ${HEADER_FILES} ${SOURCE_UI_FILES}
    OUT_QRC TRANSLATIONS_QRC_FILES
    )
if(CMAKE_BUILD_TYPE)
    string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
endif()
if(BUILD_TYPE STREQUAL "debug")
    LIST(APPEND PRIVATE_DEFINITIONS DEBUG)
    LIST(APPEND RCC_FILES
        ${TRANSLATIONS_QRC_FILES}
        )
endif()

ADD_TARGET(NAME ${PROJECT_NAME}
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${SOURCE_UI_FILES} ${RCC_FILES}
    INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # See: http://www.it1352.com/478094.html
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        ${_LibDataChannel_INCLUDE_DIRS}
    INSTALL_HEADER_FILES ${INSTALL_HEADER_FILES}
    LIBS ${SRC_LIBS}
    VERSION ${BUILD_VERSION})

add_subdirectory(Plugins)
