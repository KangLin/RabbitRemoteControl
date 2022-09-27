# Author: Kang Lin <kl222@126.com>

project(FreeRDP)

SET(SRC_LIBS freerdp-server freerdp-shadow
    freerdp-shadow-subsystem freerdp-client freerdp Service)
if(NOT WIN32)
    LIST(APPEND SRC_LIBS winpr)
    install_targets(TARGETS ${SRC_LIBS})
endif()

if(NOT TARGET Service)
    return()
endif()

SET(SOURCE_FILES
    ServiceFreeRDP.cpp
    PluginServiceFreeRDP.cpp
    ParameterServiceFreeRDP.cpp
    )

SET(HEADER_FILES
    ServiceFreeRDP.h
    PluginServiceFreeRDP.h
    ParameterServiceFreeRDP.h
    )

if(WITH_GUI)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets)
    
    list(APPEND SOURCE_FILES FrmParameterFreeRDP.cpp FrmParameterFreeRDP.ui)
    list(APPEND HEADER_FILES FrmParameterFreeRDP.h)
endif()

if(MINGW)
    set(FreeRDP_OPTIONS -fpermissive)
endif()

ADD_PLUGIN_TARGET(NAME PluginService${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${RCC_FILES}
    PRIVATE_LIBS ${SRC_LIBS}
    PRIVATE_INCLUDE_DIRS ${FreeRDP_INCLUDE_DIR} ${WinPR_INCLUDE_DIR}
    PRIVATE_DEFINITIONS FreeRDP_VERSION_MAJOR=${FreeRDP_MAJOR}
    PRIVATE_OPTIONS ${FreeRDP_OPTIONS}
    INSTALL_DIR plugins/Service
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Service
    VERSION ${RabbitRemoteControl_VERSION}
    )
