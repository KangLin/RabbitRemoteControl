cmake_minimum_required(VERSION 2.8.11)

project(RabbitRemoteControl LANGUAGES CXX)

set(INSTALL_HEADER_FILES
    FrmViewer.h
    Connecter.h
    ManageConnecter.h
    PluginFactory.h
    )

set(HEADER_FILES
    ${INSTALL_HEADER_FILES}
    Connect.h
    )

set(SOURCE_FILES
    FrmViewer.cpp
    Connecter.cpp
    Connect.cpp
    ManageConnecter.cpp
    PluginFactory.cpp
    )

set(SOURCE_UI_FILES
    FrmViewer.ui
    )

SET(RCC_FILES )

SET(SRC_LIBS RabbitCommon ${QT_LIBRARIES})
find_package(log4cplus OPTIONAL_COMPONENTS log4cplus)
message("log4cplus:${log4cplus_FOUND}")
if(log4cplus_FOUND)
    #message("Found log4cplugs componts:${log4cplus_FIND_COMPONENTS}")
    LIST(APPEND SRC_LIBS log4cplus::log4cplus)
else()
    message(FATAL_ERROR "Need log4cplus. download from https://github.com/log4cplus/log4cplus")
endif()

find_package(RabbitVNC)
if(RabbitVNC_FOUND)
    LIST(APPEND SRC_LIBS ${RabbitVNC_LIBRARIES})
    INSTALL_TARGETS(TARGETS ${RabbitVNC_LIBRARIES})
    LIST(APPEND SOURCE_FILES ConnectVnc.cpp)
    LIST(APPEND HEADER_FILES ConnectVnc.h)
endif()

#翻译
GENERATED_QT_TRANSLATIONS(
    SOURCES ${SOURCE_FILES} ${HEADER_FILES} ${SOURCE_UI_FILES}
    OUT_QRC TRANSLATIONS_QRC_FILES
    )
string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
if(BUILD_TYPE STREQUAL "debug")
    LIST(APPEND PRIVATE_DEFINITIONS DEBUG)
    LIST(APPEND RCC_FILES
        ${TRANSLATIONS_QRC_FILES}
        )
endif()

string(TOLOWER ${PROJECT_NAME} LOWER_PROJECT_NAME)
set(INSTALL_HEADER_FILES ${INSTALL_HEADER_FILES}
    ${CMAKE_CURRENT_BINARY_DIR}/${LOWER_PROJECT_NAME}_export.h)

ADD_TARGET(NAME ${PROJECT_NAME}
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${SOURCE_UI_FILES} ${RCC_FILES}
    INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    INSTALL_HEADER_FILES ${INSTALL_HEADER_FILES}
    LIBS ${SRC_LIBS})

GENERATE_EXPORT_HEADER(${PROJECT_NAME})
file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${LOWER_PROJECT_NAME}_export.h
    DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(Plugins)
