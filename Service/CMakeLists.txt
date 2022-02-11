find_package(QtService)
if(NOT QtService_FOUND)
    return()
endif()

project(Service LANGUAGES CXX)

set(SRC_LIBS QtService RabbitCommon)
set(SRC_PRIVATE_LIBS)

set(INSTALL_HEADER_FILES
    PluginService.h
    ServiceManager.h
    InputDevice.h
    Screen.h
    Display.h
    Desktop.h
    )

set(HEADER_FILES
    ${INSTALL_HEADER_FILES}
    Service.h
    ServiceThread.h
    ParameterService.h
    )

set(SOURCE_FILES
    PluginService.cpp
    ServiceManager.cpp
    Service.cpp
    ServiceThread.cpp
    ParameterService.cpp
    )

if(WIN32)
    list(APPEND HEADER_FILES
        DesktopWindows.h
        ScreenWindows.h
        InputDeviceWindows.h
        )
    list(APPEND SOURCE_FILES
        DesktopWindows.cpp
        InputDeviceWindows.cpp
        ScreenWindows.cpp
        )
elseif(UNIX AND NOT APPLE AND NOT ANDROID)
    find_package(X11)
    if(X11_FOUND AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
        list(APPEND SRC_LIBS X11::X11)
        list(APPEND HEADER_FILES
            ScreenXLib.h
            InputDeviceXLib.h
            DisplayXLib.h
            DesktopXLib.h
            )
        list(APPEND SOURCE_FILES
            ScreenXLib.cpp
            DisplayXLib.cpp
            InputDeviceXLib.cpp
            DesktopXLib.cpp
            )
    endif()
    if(X11_XTest_FOUND)
        LIST(APPEND PRIVATE_DEFINITIONS HAVE_XTEST)
        list(APPEND SRC_PRIVATE_LIBS ${X11_XTest_LIB})
    else()
        message("Please install xorg-dev:\n"
            "Ubuntu:\n"
            "$ sudo apt-get install xorg-dev\n"
            
            "CentOS:\n"
            "$ sudo yum install xorg-x11*\n")
    endif()
    if(X11_Xfixes_FOUND)
        LIST(APPEND PRIVATE_DEFINITIONS HAVE_XFIXES)
        list(APPEND SRC_PRIVATE_LIBS ${X11_Xfixes_LIB})
    else()
        message("Please install xorg-dev:\n"
            "Ubuntu:\n"
            "$ sudo apt-get install xorg-dev\n"
            
            "CentOS:\n"
            "$ sudo yum install xorg-x11*\n")
    endif()
endif()

set(SOURCE_UI_FILES
    )

SET(RCC_FILES )

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network)
if(Qt${QT_VERSION_MAJOR}_FOUND)
    list(APPEND QT_LIBRARIES Qt${QT_VERSION_MAJOR}::Network)
endif()
list(APPEND SRC_LIBS ${QT_LIBRARIES})
INSTALL_TARGETS(TARGETS QtService)

ADD_TARGET(NAME ${PROJECT_NAME}
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${SOURCE_UI_FILES} ${RCC_FILES}
    INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # See: http://www.it1352.com/478094.html
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    INSTALL_HEADER_FILES ${INSTALL_HEADER_FILES}
    LIBS ${SRC_LIBS}
    PRIVATE_LIBS ${SRC_PRIVATE_LIBS}
    VERSION ${BUILD_VERSION}
    PRIVATE_DEFINITIONS ${PRIVATE_DEFINITIONS}
    INSTALL_EXPORT_NAME ${PROJECT_NAME}Targets
    INSTALL_NAMESPACE RabbitRemoteControl
    )
