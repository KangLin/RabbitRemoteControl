# Author: Kang Lin <kl222@126.com>

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "verbose")

find_package(QtService)
if(NOT QtService_FOUND)
    return()
endif()

project(Service LANGUAGES CXX)

set(Service_LIBS QtService Channel)

set(Service_INSTALL_HEADER_FILES
    ManagerPlugins.h
    PluginService.h
    PluginServiceThread.h
    ServiceManager.h
    InputDevice.h
    Screen.h
    Display.h
    Desktop.h
    )

set(Service_HEADER_FILES
    ${Service_INSTALL_HEADER_FILES}
    Service.h
    ServiceThread.h
    ParameterService.h
    )

set(Service_SOURCE_FILES
    PluginService.cpp
    PluginServiceThread.cpp
    ServiceManager.cpp
    Service.cpp
    ServiceThread.cpp
    ParameterService.cpp
    ManagerPlugins.cpp
    )

if(WIN32)
    list(APPEND Service_HEADER_FILES
        DesktopWindows.h
        ScreenWindows.h
        InputDeviceWindows.h
        )
    list(APPEND Service_SOURCE_FILES
        DesktopWindows.cpp
        InputDeviceWindows.cpp
        ScreenWindows.cpp
        )
elseif(UNIX AND NOT APPLE AND NOT ANDROID)
    find_package(X11)
    if(X11_FOUND AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
        list(APPEND Service_LIBS X11::X11)
        list(APPEND Service_HEADER_FILES
            ScreenXLib.h
            InputDeviceXLib.h
            DisplayXLib.h
            DesktopXLib.h
            )
        list(APPEND Service_SOURCE_FILES
            ScreenXLib.cpp
            DisplayXLib.cpp
            InputDeviceXLib.cpp
            DesktopXLib.cpp
            )
    endif()
    if(X11_Xtst_FOUND)
        LIST(APPEND PRIVATE_DEFINITIONS HAVE_XTEST)
        list(APPEND Service_PRIVATE_LIBS X11::Xtst)
    else()
        message("Please install xorg-dev:\n"
            "Ubuntu:\n"
            "$ sudo apt-get install xorg-dev\n"
            
            "CentOS:\n"
            "$ sudo yum install xorg-x11*\n")
    endif()
    if(X11_Xfixes_FOUND)
        LIST(APPEND PRIVATE_DEFINITIONS HAVE_Xfixes)
        list(APPEND Service_PRIVATE_LIBS X11::Xfixes)
    else()
        message("Please install xorg-dev:\n"
            "Ubuntu:\n"
            "$ sudo apt-get install xorg-dev\n"
            
            "CentOS:\n"
            "$ sudo yum install xorg-x11*\n")
    endif()
endif()

SET(Service_QT_COMPONENTS Core Gui Network)
if(WITH_GUI)
    list(APPEND Service_QT_COMPONENTS Widgets)
    set(Service_SERVICE_DEFINITIONS HAVE_GUI)
endif()
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${Service_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${Service_QT_COMPONENTS})
        list(APPEND Service_QT_LIBRARIES Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()
list(APPEND Service_LIBS ${Service_QT_LIBRARIES})
message("${PROJECT_NAME} LIBRARIES: ${Service_LIBS}")
INSTALL_TARGETS(TARGETS QtService)

ADD_TARGET(NAME Service
    SOURCE_FILES ${Service_SOURCE_FILES} ${Service_HEADER_FILES}
    INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # See: http://www.it1352.com/478094.html
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> 
    INSTALL_HEADER_FILES ${Service_INSTALL_HEADER_FILES}
    LIBS ${Service_LIBS}
    PRIVATE_LIBS ${Service_PRIVATE_LIBS}
    VERSION ${RabbitRemoteControl_VERSION}
    DEFINITIONS ${Service_SERVICE_DEFINITIONS}
    PRIVATE_DEFINITIONS ${PRIVATE_DEFINITIONS}
    INSTALL_EXPORT_NAME ServiceTargets
    INSTALL_NAMESPACE RabbitRemoteControl
    )
