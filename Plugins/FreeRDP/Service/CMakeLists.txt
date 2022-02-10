# Author: Kang Lin <kl222@126.com>

project(FreeRDP)

SET(SRC_LIBS freerdp-client freerdp Service)
if(NOT WIN32)
    LIST(APPEND SRC_LIBS winpr)
endif()

if(NOT TARGET Service)
    return()
endif()

SET(SOURCE_FILES
    ServiceFreeRDP.cpp
    PluginServiceFreeRDP.cpp
    )

SET(HEADER_FILES
    ServiceFreeRDP.h
    PluginServiceFreeRDP.h
    )

if(MINGW)
    set(FreeRDP_OPTIONS -fpermissive)
endif()

ADD_PLUGIN_TARGET(NAME PluginService${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${RCC_FILES}
    PRIVATE_LIBS ${SRC_LIBS}
    PRIVATE_INCLUDE_DIRS ${FreeRDP_INCLUDE_DIR} ${WinPR_INCLUDE_DIR}
    PRIVATE_DEFINITIONS ${RDP_DEFINED}
    PRIVATE_OPTIONS ${FreeRDP_OPTIONS}
    INSTALL_DIR plugins/Service
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Service
    )