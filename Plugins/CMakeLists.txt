# author: Kang Lin <kl222@126.com>

# setup RPATH to where binary is at.
if(APPLE)
    if(PARA_ISEXE)
        SET(INSTALL_RPATH
            "@executable_path:@executable_path/../../${CMAKE_INSTALL_LIBDIR}")
    else()
        SET(INSTALL_RPATH
            "@loader_path:@loader_path/../../${CMAKE_INSTALL_LIBDIR}")
    endif()
else()
    SET(INSTALL_RPATH
        "$ORIGIN:$ORIGIN/../../${CMAKE_INSTALL_LIBDIR}")
endif()

SET(PLUGIN_PATH plugins/RabbitRemoteControl)

## Desktop

option(BUILD_RABBITVNC "Build RabbitVNC plugin" OFF)
if(BUILD_RABBITVNC)
    add_subdirectory(RabbitVNC)
endif()

option(BUILD_TigerVNC "Build TigerVnc plugin" ON)
if(BUILD_TigerVNC)
    add_subdirectory(TigerVnc)
endif()

option(BUILD_LibVNCServer "Build LibVNCServer plugin" ON)
if(BUILD_LibVNCServer)
    add_subdirectory(LibVNCServer)
endif()

option(BUILD_FREERDP "Build freerdp plugin" ON)
if(BUILD_FREERDP)
    add_subdirectory(FreeRDP)
endif()

message(STATUS "BUILD_RABBITVNC:${BUILD_RABBITVNC}")
message(STATUS "BUILD_TigerVNC:${BUILD_TigerVNC}")
message(STATUS "BUILD_LibVNCServer:${BUILD_LibVNCServer}")
message(STATUS "BUILD_FREERDP:${BUILD_FREERDP}")

## Tools
option(BUILD_WakeOnLan "Build wake on lan plugin" ON)
if(BUILD_WakeOnLan)
    add_subdirectory(WakeOnLan)
endif()
if(QT_VERSION GREATER_EQUAL "6.8.0")
    option(BUILD_PLAYER "Build player plugin" ON)
    if(BUILD_PLAYER)
        add_subdirectory(Player)
    endif()
endif()
if(QT_VERSION GREATER_EQUAL "6.8.0")
    option(BUILD_SCREEN_CAPTURE "Build screen capture" ON)
    if(BUILD_SCREEN_CAPTURE)
        add_subdirectory(ScreenCapture)
    endif()
endif()

## Terminal
find_package(qtermwidget6)
if(qtermwidget6_FOUND)
    option(BUILD_TERMINAL "Build Terminal" ON)
    if(BUILD_TERMINAL)
        add_subdirectory(Terminal)
    endif()
    option(BUILD_TELNET "Build Telnet" OFF)
    if(BUILD_TELNET)
        add_subdirectory(Telnet)
    endif()
    option(BUILD_LIBSSH "Build libssh" OFF)
    if(BUILD_LIBSSH)
        add_subdirectory(LibSSH)
    endif()
endif()

message(STATUS "BUILD_TERMINAL:${BUILD_TERMINAL}")
message(STATUS "BUILD_TELNET:${BUILD_TELNET}")
message(STATUS "BUILD_LIBSSH:${BUILD_LIBSSH}")
