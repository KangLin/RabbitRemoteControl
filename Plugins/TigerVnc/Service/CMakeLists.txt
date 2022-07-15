# Author: Kang Lin <kl222@126.com>

project(TigerVNC)
set(CMAKE_CXX_STANDARD 17)

if(NOT TARGET Service)
    return()
endif()
SET(SRC_LIBS tigervnc Service)
SET(SOURCE_FILES
    ParameterServiceTigerVNC.cpp
    PluginServiceTigerVNC.cpp
    ServiceTigerVNC.cpp
    Connection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../RabbitVNC/InStreamChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../RabbitVNC/OutStreamChannel.cpp
    )
SET(HEADER_FILES
    ParameterServiceTigerVNC.h
    PluginServiceTigerVNC.h
    ServiceTigerVNC.h
    Connection.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../RabbitVNC/InStreamChannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../RabbitVNC/OutStreamChannel.h
    )
SET(UI_FILES
    )

if(WITH_GUI)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets)
    list(APPEND SOURCE_FILES FrmParameterTigerVNC.cpp FrmParameterTigerVNC.ui)
    list(APPEND HEADER_FILES FrmParameterTigerVNC.h)
endif()

ADD_PLUGIN_TARGET(NAME PluginService${PROJECT_NAME}
    ISPLUGIN
    VERSION ${RabbitRemoteControl_VERSION}
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
    PRIVATE_LIBS ${SRC_LIBS}
    PRIVATE_INCLUDE_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    PRIVATE_DEFINITIONS ${TIGERVNC_DEFINITIONS}
    INSTALL_DIR plugins/Service
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Service
    )
