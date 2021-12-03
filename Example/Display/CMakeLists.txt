cmake_minimum_required(VERSION 3.5)

project(Display)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    )

ADD_TARGET(NAME ${PROJECT_NAME}
    ISEXE
    ISWINDOWS
    NO_TRANSLATION
    VERSION ${BUILD_VERSION}
    PRIVATE_INCLUDE_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Service/>
    SOURCE_FILES ${PROJECT_SOURCES}
    PRIVATE_LIBS Service ${QT_LIBRARIES}
    )
