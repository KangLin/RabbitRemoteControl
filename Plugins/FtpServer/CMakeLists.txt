# Author: Kang Lin <kl222@126.com>

project(FtpServer
    DESCRIPTION "Ftp service plugin"
    )

find_package(QFtpServerLib)
if(NOT QFtpServerLib_FOUND)
    # Fetch QFtpServer from GitHub
    include(FetchContent)
    FetchContent_Declare(
        QFtpServerLib
        GIT_REPOSITORY https://github.com/KangLin/QFtpServer.git
        GIT_TAG master  # or specify a specific version/tag
    )
    
    # Configure QFtpServer build options before making it available
    set(WITH_APP OFF CACHE BOOL "" FORCE)
    set(WITH_TEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(QFtpServerLib)
endif()

option(INSTALL_QFtpServer "Install QFtpServer libraries" OFF)
if(INSTALL_QFtpServer)
    INSTALL_TARGETS(TARGETS QFtpServerLib)
endif()

SET(FtpServer_PRIVATE_LIBS Plugin QFtpServerLib)
SET(SOURCE_FILES
    PluginFtpServer.cpp
    OperateFtpServer.cpp
    BackendFtpServer.cpp
    DlgSettings.cpp
    ParameterFtpServer.cpp
    FrmMain.cpp
    )

SET(HEADER_FILES
    PluginFtpServer.h
    OperateFtpServer.h
    BackendFtpServer.h
    DlgSettings.h
    ParameterFtpServer.h
    FrmMain.h
    )

SET(RCC_FILES
    DlgSettings.ui
    FrmMain.ui)
if(WITH_GUI)
    
endif()

set(FtpServer_QT_COMPONENTS Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${FtpServer_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${FtpServer_QT_COMPONENTS})
        list(APPEND FtpServer_PRIVATE_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()

ADD_PLUGIN_TARGET(NAME PluginService${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${RCC_FILES}
    PRIVATE_LIBS ${FtpServer_PRIVATE_LIBS}
    PRIVATE_INCLUDE_DIRS ${FtpServer_INCLUDE_DIR} ${WinPR_INCLUDE_DIR}
    PRIVATE_DEFINITIONS FtpServer_VERSION_MAJOR=${FtpServer_MAJOR}
    PRIVATE_OPTIONS ${FtpServer_OPTIONS}
    INSTALL_DIR ${PLUGIN_PATH}
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
    )
