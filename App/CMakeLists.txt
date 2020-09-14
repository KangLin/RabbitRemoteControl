cmake_minimum_required(VERSION 3.5)

project(RabbitRemoteControl LANGUAGES CXX)

set(SOURCE_FILES
    main.cpp
    mainwindow.cpp
    )

set(HEADER_FILES
    mainwindow.h
    )

set(SOURCE_UI_FILES
    mainwindow.ui
    )

if(WIN32)
    set(RCC_FILES Resource/App.rc)
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

ADD_TARGET(NAME ${PROJECT_NAME}
    ISEXE
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${SOURCE_UI_FILES} ${RCC_FILES}
    PRIVATE_LIBS RabbitCommon ${QT_LIBRARIES})

install(FILES Resource/Image/App.ico
    ${CMAKE_SOURCE_DIR}/Install/Install.nsi
    DESTINATION .
        COMPONENT Runtime)
