# Author: Kang Lin <kl222@126.com>

cmake_minimum_required(VERSION 3.21)
MESSAGE(STATUS "Found CMake ${CMAKE_VERSION}")

project(RabbitRemoteControl
    DESCRIPTION "Rabbit Remote Control"
    HOMEPAGE_URL "https://github.com/KangLin/RabbitRemoteControl"
    )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "verbose")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

if(POLICY CMP0083)
    cmake_policy(SET CMP0083 NEW)
endif()

if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

if(POLICY CMP0021)
    cmake_policy(SET CMP0021 NEW)
endif()

IF(MSVC)
    # This option is to enable the /MP switch for Visual Studio 2005 and above compilers
    OPTION(WIN32_USE_MP "Set to ON to build with the /MP option (Visual Studio 2005 and above)." ON)
    MARK_AS_ADVANCED(WIN32_USE_MP)
    IF(WIN32_USE_MP)
        #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
        add_compile_options(/MP)
    ENDIF(WIN32_USE_MP)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
ENDIF(MSVC)

SET(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libs")
if (BUILD_SHARED_LIBS)
    add_definitions(-DBUILD_SHARED_LIBS)
    if (CMAKE_COMPILER_IS_GNUCXX AND NOT MINGW)
       # Just setting CMAKE_POSITION_INDEPENDENT_CODE should be enough to set
       # -fPIC for GCC but sometimes it still doesn't get set, so make sure it
       # does.
       add_definitions("-fPIC")
    endif()
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
else(BUILD_SHARED_LIBS)
    add_definitions(-DQT_STATICPLUGIN)
endif(BUILD_SHARED_LIBS)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(GenerateExportHeader)
include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckFunctionExists)
include(CMakeDependentOption)

#CHECK_INCLUDE_FILE_CXX("string" HAVE_STRING_H)
#check_include_file("math.h" HAVE_MATH_H)

#check_function_exists("fabs" HAVE_FABS)

set(BUILD_PLATFORM "${CMAKE_SYSTEM_NAME}")
# ----------------------------------------------------------------------------
# Detect compiler and target platform architecture
# ----------------------------------------------------------------------------
if(NOT ANDROID)
    if(X86_64 OR CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(BUILD_ARCH x86_64)
    elseif(X86 OR CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(BUILD_ARCH x86)
    endif()
else()
    set(BUILD_ARCH ${CMAKE_SYSTEM_PROCESSOR})
endif()

# Open qt build tool
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)

# Need qt components
if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()
SET(QT_COMPONENTS Core)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_COMPONENTS})
message("QT_VERSION:${Qt${QT_VERSION_MAJOR}_VERSION}")
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${QT_COMPONENTS})
        list(APPEND QT_LIBRARIES Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()
get_filename_component(QT_INSTALL_DIR "${Qt${QT_VERSION_MAJOR}_DIR}/../../.." ABSOLUTE)
message("QT_ROOT:${QT_ROOT}")
message("QT_DIR:${QT_DIR}")
message("Qt${QT_VERSION_MAJOR}_ROOT:${Qt${QT_VERSION_MAJOR}_ROOT}")
message("Qt${QT_VERSION_MAJOR}_DIR:${Qt${QT_VERSION_MAJOR}_DIR}")
message("QT_INSTALL_DIR:${QT_INSTALL_DIR}")
message("${PROJECT_NAME} QT_LIBRARIES:${QT_LIBRARIES}")

if(NOT RabbitCommon_ROOT)
    set(RabbitCommon_ROOT $ENV{RabbitCommon_ROOT})
    if(NOT RabbitCommon_ROOT)
        set(RabbitCommon_ROOT ${CMAKE_SOURCE_DIR}/../RabbitCommon)
    endif()
endif()
if(RabbitCommon_ROOT AND EXISTS ${RabbitCommon_ROOT}/Src)
    message("Use RabbitCommon source code")
    add_subdirectory(${RabbitCommon_ROOT}/Src ${CMAKE_BINARY_DIR}/RabbitCommon)
else()
    find_package(RabbitCommon)
    if(NOT RabbitCommon_FOUND)
        message("RabbitCommon_ROOT is not found. Please use one of the following ways to set it:")
        message("1. Set RabbitCommon_ROOT to the install prefix of RabbitCommon.")
        message("2. Set RabbitCommon_ROOT to source code root of RabbitCommon.")
        message("2.1 Please download the source code of RabbitCommon from https://github.com/KangLin/RabbitCommon")
        message("   ag:")
        message("       git clone https://github.com/KangLin/RabbitCommon.git ${CMAKE_SOURCE_DIR}/..")
        message("2.2 Then set cmake variable or environment variable RabbitCommon_ROOT to download root directory.")
        message("   ag:")
        message("       cmake -DRabbitCommon_ROOT=${CMAKE_SOURCE_DIR}/.. ")
        message(FATAL_ERROR "RabbitCommon_ROOT isn't set.")
    endif()
endif()

GET_VERSION(OUT_VERSION RabbitRemoteControl_VERSION
    OUT_REVISION RabbitRemoteControl_REVISION
    OUT_TAG RabbitRemoteControlTag)
IF(NOT RabbitRemoteControl_VERSION)
    SET(RabbitRemoteControl_VERSION "0.0.33")
ENDIF()
IF(NOT RabbitRemoteControlTag)
    SET(RabbitRemoteControlTag "0.0.33")
ENDIF()
message("RabbitRemoteControl_VERSION:${RabbitRemoteControl_VERSION};Revision:${RabbitRemoteControl_REVISION};Tag:${RabbitRemoteControlTag}")
set(VERSION ${RabbitRemoteControl_VERSION})

add_subdirectory(Channel)
cmake_dependent_option(BUILD_CLIENT "Build Client" ON WITH_GUI OFF)
if(BUILD_CLIENT)
    add_subdirectory(Client)
endif(BUILD_CLIENT)
find_package(QtService)
cmake_dependent_option(BUILD_SERVICE "Build service" OFF QtService_FOUND OFF)
if(BUILD_SERVICE)
    add_subdirectory(Service)
endif(BUILD_SERVICE)
option(BUILD_PLUGINS "Build plugins" ON)
if(BUILD_PLUGINS)
    add_subdirectory(Plugins ${CMAKE_BINARY_DIR}/build_plugins)
endif(BUILD_PLUGINS)
option(BUILD_APP "Build applaction" ON)
if(BUILD_APP)
    add_subdirectory(App)
endif(BUILD_APP)
option(BUILD_EXAMPLE "Build example" OFF)
if(BUILD_EXAMPLE)
    add_subdirectory(Example)
endif()

iF(WIN32)
    # 替换 Windows.nsi 中的 CMAKE_INSTALL_PREFIX 等
    configure_file(${CMAKE_SOURCE_DIR}/Package/Windows.nsi
        ${CMAKE_BINARY_DIR}/Windows.nsi @ONLY)
endif()

option(BUILD_DOCS "Generating API documentation" OFF)
if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Doxygen)
        file(COPY ${CMAKE_SOURCE_DIR}/docs/Doxygen/index.html
            DESTINATION ${CMAKE_BINARY_DIR}/Doxygen)
        foreach(OUTPUT_LANGUAGE English Chinese)
            set(MAIN_README "${CMAKE_SOURCE_DIR}/docs/Doxygen/Develop.md")
            set(PROJECT_NAME "Rabbit Remote Control")
            if(OUTPUT_LANGUAGE STREQUAL "Chinese")
                set(PROJECT_NAME "玉兔远程控制")
                set(MAIN_README "${CMAKE_SOURCE_DIR}/docs/Doxygen/Develop_zh_CN.md")
            endif()
            configure_file(${CMAKE_SOURCE_DIR}/docs/Doxygen/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile_${OUTPUT_LANGUAGE} @ONLY)
            add_custom_target(doc_${OUTPUT_LANGUAGE} ALL
                ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile_${OUTPUT_LANGUAGE} # &> doxygen.log
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen" VERBATIM
                )
        endforeach()
        install(DIRECTORY ${CMAKE_BINARY_DIR}/Doxygen/
            DESTINATION ${CMAKE_INSTALL_DOCDIR}/${OUTPUT_LANGUAGE}
                COMPONENT Runtime)
    endif(DOXYGEN_FOUND)
endif(BUILD_DOCS)
