find_package(QtService)
if(NOT QtService_FOUND)
    return()
endif()

project(Service LANGUAGES CXX)

set(SRC_LIBS RabbitCommon QtService)

set(INSTALL_HEADER_FILES
    PluginService.h
    ServiceManager.h
    InputDevice.h
    Screen.h
    Disply.h
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
        ScreenWindows.h
        InputDeviceWindows.h)
    list(APPEND SOURCE_FILES
        InputDeviceWindows.cpp
        ScreenWindows.cpp)
elseif(UNIX AND NOT APPLE AND NOT ANDROID)
    find_package(X11)
    if(X11_FOUND AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
        list(APPEND SRC_LIBS X11::X11)
        list(APPEND HEADER_FILES
            ScreenXLib.h
            InputDeviceX11.h
            )
        list(APPEND SOURCE_FILES
            ScreenXLib.cpp
            InputDeviceX11.cpp)
    endif()
    if(X11_XTest_FOUND)
        LIST(APPEND PRIVATE_DEFINITIONS HAVE_XTEST)
        list(APPEND SRC_LIBS ${X11_XTest_LIB})
    else()
        message("Please install xorg-dev:
            Ubuntu:
            $ sudo apt-get install xorg-dev
            
            CentOS:
            $ sudo yum install xorg-x11*")
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
    INSTALL_HEADER_FILES ${INSTALL_HEADER_FILES}
    LIBS ${SRC_LIBS}
    VERSION ${BUILD_VERSION}
    PRIVATE_DEFINITIONS ${PRIVATE_DEFINITIONS}
    )
