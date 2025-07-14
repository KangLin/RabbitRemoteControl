# author: Kang Lin <kl222@126.com>

# setup RPATH to where binary is at.
if(APPLE)
    if(PARA_ISEXE)
        SET(INSTALL_RPATH
            "@executable_path:@executable_path/../${CMAKE_INSTALL_LIBDIR}")
    else()
        SET(INSTALL_RPATH
            "@loader_path:@loader_path/../${CMAKE_INSTALL_LIBDIR}")
    endif()
else()
    SET(INSTALL_RPATH
        "$ORIGIN:$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
endif()

SET(PLUGIN_PATH plugins)

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

option(BUILD_FILE_TRANSFER "Build file transfaer" ON)
if(BUILD_FILE_TRANSFER)
    add_subdirectory(FileTransfer)
endif()

## Terminal
find_package(qtermwidget${QT_VERSION_MAJOR})
if(qtermwidget${QT_VERSION_MAJOR}_FOUND)
    option(BUILD_TERMINAL "Build Terminal" ON)
    if(BUILD_TERMINAL)
        add_subdirectory(Terminal)
    endif()
    option(BUILD_TERMINAL_TELENT "Build Telnet" ON)
    if(BUILD_TERMINAL_TELENT)
        add_subdirectory(Telnet)
    endif()
    option(BUILD_TERMINAL_SSH "Build libssh" ON)
    if(BUILD_TERMINAL_SSH)
        add_subdirectory(SSH)
    endif()
endif()

message(STATUS "BUILD_RABBITVNC:${BUILD_RABBITVNC}")
message(STATUS "BUILD_TigerVNC:${BUILD_TigerVNC}")
message(STATUS "BUILD_LibVNCServer:${BUILD_LibVNCServer}")
message(STATUS "BUILD_FREERDP:${BUILD_FREERDP}")
message(STATUS "BUILD_WakeOnLan:${BUILD_WakeOnLan}")
message(STATUS "BUILD_PLAYER:${BUILD_PLAYER}")
message(STATUS "BUILD_SCREEN_CAPTURE:${BUILD_SCREEN_CAPTURE}")
message(STATUS "BUILD_TERMINAL:${BUILD_TERMINAL}")
message(STATUS "BUILD_TERMINAL_TELENT:${BUILD_TERMINAL_TELENT}")
message(STATUS "BUILD_TERMINAL_SSH:${BUILD_TERMINAL_SSH}")
message(STATUS "BUILD_FILE_TRANSFER:${BUILD_FILE_TRANSFER}")
