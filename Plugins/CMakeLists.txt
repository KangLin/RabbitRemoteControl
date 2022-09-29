# author: Kang Lin <kl222@126.com>

## Desktop

cmake_dependent_option(BUILD_RABBITVNC
    "Build RabbitVNC plugin" ON BUILD_PLUGINS OFF)
if(BUILD_RABBITVNC)
    add_subdirectory(RabbitVNC)
endif()

cmake_dependent_option(BUILD_TigerVNC
    "Build TigerVnc plugin" ON BUILD_PLUGINS OFF)
if(BUILD_TigerVNC)
    add_subdirectory(TigerVnc)
endif()

cmake_dependent_option(BUILD_LibVNCServer
    "Build LibVNCServer plugin" ON BUILD_PLUGINS OFF)
if(BUILD_LibVNCServer)
    add_subdirectory(LibVNCServer)
endif()

cmake_dependent_option(BUILD_FREERDP
    "Build freerdp plugin" OFF BUILD_PLUGINS OFF)
if(BUILD_FREERDP)
    add_subdirectory(FreeRDP)
endif()

## Terminal

if(BUILD_CLIENT_TERMINAL AND BUILD_PLUGINS)
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
