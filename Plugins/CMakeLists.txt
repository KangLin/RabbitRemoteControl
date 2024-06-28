# author: Kang Lin <kl222@126.com>

# setup RPATH to where binary is at.
if(APPLE)
    if(PARA_ISEXE)
        SET(INSTALL_RPATH
            "@executable_path:@executable_path/../../lib:@executable_path/../../lib/${CMAKE_LIBRARY_ARCHITECTURE}")
    else()
        SET(INSTALL_RPATH
            "@loader_path:@loader_path/../../lib:@loader_path/../../lib/${CMAKE_LIBRARY_ARCHITECTURE}")
    endif()
else()
    SET(INSTALL_RPATH
        "$ORIGIN:$ORIGIN/../../lib:$ORIGIN/../../lib/${CMAKE_LIBRARY_ARCHITECTURE}")
endif()

## Desktop

option(BUILD_RABBITVNC "Build RabbitVNC plugin" ON)
if(BUILD_RABBITVNC)
    add_subdirectory(RabbitVNC)
endif()

option(BUILD_TigerVNC "Build TigerVnc plugin" $ENV{BUILD_TigerVNC})
if(BUILD_TigerVNC)
    add_subdirectory(TigerVnc)
endif()

option(BUILD_LibVNCServer "Build LibVNCServer plugin" $ENV{BUILD_LibVNCServer})
if(BUILD_LibVNCServer)
    add_subdirectory(LibVNCServer)
endif()

option(BUILD_FREERDP "Build freerdp plugin" OFF)
if(BUILD_FREERDP)
    add_subdirectory(FreeRDP)
endif()

message(STATUS "BUILD_RABBITVNC:${BUILD_RABBITVNC}")
message(STATUS "BUILD_TigerVNC:${BUILD_TigerVNC}")
message(STATUS "BUILD_LibVNCServer:${BUILD_LibVNCServer}")
message(STATUS "BUILD_FREERDP:${BUILD_FREERDP}")

## Terminal

if(BUILD_CLIENT_TERMINAL)
    find_package(qtermwidget5)
    if(qtermwidget5_FOUND)
        option(BUILD_TERMINAL "Build Terminal" ON)
        if(BUILD_TERMINAL)
            add_subdirectory(Terminal)
        endif()
        option(BUILD_TELNET "Build Telnet" ON)
        if(BUILD_TELNET)
            add_subdirectory(Telnet)
        endif()
        option(BUILD_LIBSSH "Build libssh" ON)
        if(BUILD_LIBSSH)
            add_subdirectory(LibSSH)
        endif()
    endif()
endif()

message(STATUS "BUILD_TERMINAL:${BUILD_TERMINAL}")
message(STATUS "BUILD_TELNET:${BUILD_TELNET}")
message(STATUS "BUILD_LIBSSH:${BUILD_LIBSSH}")
