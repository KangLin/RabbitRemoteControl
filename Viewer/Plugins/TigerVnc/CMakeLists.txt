
project(TigerVnc)

find_package(tigervnc)
message("tigervnc_FOUND:${tigervnc_FOUND}")
if(NOT tigervnc_FOUND)
    return()
endif()

SET(SRC_LIBS tigervnc RabbitRemoteControl)
SET(SOURCE_FILES
    ConnectTigerVnc.cpp
    FramePixelBuffer.cpp
    ConnecterTigerVnc.cpp
    ConnectThread.cpp
    ManageConnectTigerVnc.cpp)
SET(HEADER_FILES
    ConnectTigerVnc.h
    FramePixelBuffer.h
    ConnecterTigerVnc.h
    ConnectThread.h
    ManageConnectTigerVnc.h)

ADD_PLUGIN_TARGET(NAME ${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES}
    PRIVATE_LIBS ${SRC_LIBS}
    )
