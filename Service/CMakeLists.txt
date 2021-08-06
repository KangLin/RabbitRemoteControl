find_package(QtService)
if(NOT QtService_FOUND)
    return()
endif()

project(Service LANGUAGES CXX)

set(INSTALL_HEADER_FILES
    PluginService.h
    ServiceManager.h
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

set(SOURCE_UI_FILES
    )

SET(RCC_FILES )

SET(SRC_LIBS RabbitCommon QtService ${QT_LIBRARIES})

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
    VERSION ${BUILD_VERSION})
