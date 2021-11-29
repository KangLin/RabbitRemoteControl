cmake_minimum_required(VERSION 3.5)

project(Display VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT TARGET RabbitCommon)
    if(NOT RabbitCommon_DIR)
        set(RabbitCommon_DIR $ENV{RabbitCommon_DIR})
        if(NOT RabbitCommon_DIR)
            set(RabbitCommon_DIR ${CMAKE_SOURCE_DIR}/../RabbitCommon)
            if(NOT EXISTS ${RabbitCommon_DIR}/Src)
                set(RabbitCommon_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../RabbitCommon)
            endif()
        endif()
    endif()
    if(DEFINED RabbitCommon_DIR AND EXISTS ${RabbitCommon_DIR}/Src)
        add_subdirectory(${RabbitCommon_DIR}/Src ${CMAKE_BINARY_DIR}/RabbitCommon)
        #list(APPEND CMAKE_MODULE_PATH ${RabbitCommon_DIR}/cmake)
        include(${RabbitCommon_DIR}/cmake/Translations.cmake)
        include(${RabbitCommon_DIR}/cmake/RabbitCommonUtils.cmake)
    else()
        message("1. Please download RabbitCommon source code from https://github.com/KangLin/RabbitCommon")
        message("   ag:")
        message("       git clone https://github.com/KangLin/RabbitCommon.git")
        message("2. Then set cmake value or environment variable RabbitCommon_DIR to download root dirctory.")
        message("   ag:")
        message(FATAL_ERROR "       cmake -DRabbitCommon_DIR= ")
    endif()
endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

set(TS_FILES Display_zh_CN.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        #${TS_FILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Service/DisplayXLib.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Service/DesktopXLib.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Display
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Display APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

#    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(Display SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Display
            ${PROJECT_SOURCES}
        )
    endif()

#    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(Display PRIVATE Service Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(${PROJECT_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Service/>)
set_target_properties(Display PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Display)
endif()
