# author: Kang Lin <kl222@126.com>

# Desktop
option(BUILD_TigerVNC "Build TigerVnc plugin" ON)
if(BUILD_TigerVNC)
    add_subdirectory(TigerVnc)
endif()

option(BUILD_FREERDP "Build freerdp plugin" OFF)
if(BUILD_FREERDP)
    add_subdirectory(FreeRdp)
endif()

option(BUILD_LibVNCServer "Build LibVNCServer plugin" ON)
if(BUILD_LibVNCServer)
    add_subdirectory(LibVNCServer)
endif()

find_package(qtermwidget5)
if(qtermwidget5_FOUND)
    add_subdirectory(Terminal)
    add_subdirectory(Telnet)
    add_subdirectory(LibSSH)
endif()

