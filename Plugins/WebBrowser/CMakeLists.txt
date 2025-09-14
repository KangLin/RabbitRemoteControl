cmake_minimum_required(VERSION 3.21)

project(WebBrowser LANGUAGES CXX
   DESCRIPTION "Web browser plugin")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(WebBrowser_QT_COMPONENTS Core Gui Widgets WebEngineWidgets WebEngineCore)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${WebBrowser_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${WebBrowser_QT_COMPONENTS})
        list(APPEND WebBrowser_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
else()
    message("Don't found qt libraries")
    return()
endif()
message("Qt components: ${WebBrowser_LIBS}")
list(APPEND WebBrowser_LIBS Plugin)
set(WebBrowser_SOURCE_FILES
    PluginWebBrowser.cpp
    OperateWebBrowser.cpp
    FrmWebBrowser.cpp
    FrmWebView.cpp
    FrmDownload.cpp
    FrmDownloadManager.cpp
    DlgSettings.cpp
    ParameterWebBrowser.cpp
    )
set(WebBrowser_HEADER_FILES
    PluginWebBrowser.h
    OperateWebBrowser.h
    FrmWebBrowser.h
    FrmWebView.h
    FrmDownload.h
    FrmDownloadManager.h
    DlgSettings.h
    ParameterWebBrowser.h
    )
set(WebBrowser_UI_FILES
    FrmDownload.ui
    DlgSettings.ui
    )
if(QT_VERSION GREATER_EQUAL "6.0.0")
    list(APPEND WebBrowser_SOURCE_FILES
        )
    list(APPEND WebBrowser_HEADER_FILES
        )
endif()
ADD_PLUGIN_TARGET(NAME Plugin${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${WebBrowser_SOURCE_FILES} ${WebBrowser_HEADER_FILES} ${WebBrowser_UI_FILES}
    PRIVATE_LIBS ${WebBrowser_LIBS}
    PRIVATE_INCLUDE_DIRS ${WebBrowser_INCLUDE_DIR} ${WinPR_INCLUDE_DIR}
    PRIVATE_DEFINITIONS PluginWebBrowser_VERSION="${RabbitRemoteControl_VERSION}"
    DEFINITIONS ${WebBrowser_DEFINITIONS}
    PRIVATE_OPTIONS ${WebBrowser_OPTIONS}
    INSTALL_DIR ${PLUGIN_PATH}
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
    )
