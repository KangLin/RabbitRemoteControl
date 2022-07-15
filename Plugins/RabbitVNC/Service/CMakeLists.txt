# Author: Kang Lin <kl222@126.com>

project(RabbitVNC)
set(CMAKE_CXX_STANDARD 17)

if(NOT TARGET Service)
    return()
endif()
SET(SRC_LIBS RabbitVNC Service)
SET(SOURCE_FILES
    ParameterServiceRabbitVNC.cpp
    PluginServiceRabbitVNC.cpp
    ServiceRabbitVNC.cpp
    Connection.cpp
    FrmParameterRabbitVNC.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../InStreamChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../OutStreamChannel.cpp
    )
SET(HEADER_FILES
    ParameterServiceRabbitVNC.h
    PluginServiceRabbitVNC.h
    ServiceRabbitVNC.h
    Connection.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../InStreamChannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../OutStreamChannel.h
    )

if(WITH_GUI)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets)
    list(APPEND SOURCE_FILES FrmParameterRabbitVNC.cpp FrmParameterRabbitVNC.ui)
    list(APPEND HEADER_FILES FrmParameterRabbitVNC.h)
endif()

ADD_PLUGIN_TARGET(NAME PluginService${PROJECT_NAME}
    ISPLUGIN
    VERSION ${RabbitRemoteControl_VERSION}
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${RCC_FILES}
    PRIVATE_LIBS ${SRC_LIBS}
    PRIVATE_INCLUDE_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    INSTALL_DIR plugins/Service
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Service
    )
