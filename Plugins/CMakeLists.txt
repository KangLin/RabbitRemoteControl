# author: Kang Lin <kl222@126.com>

# Desktop
add_subdirectory(TigerVnc)

option(BUILD_FREERDP "Build freerdp plugin" OFF)
if(BUILD_FREERDP)
    add_subdirectory(FreeRdp)
endif()

add_subdirectory(LibVNCServer)

find_package(qtermwidget5)
if(qtermwidget5_FOUND)
    add_subdirectory(Terminal)
    add_subdirectory(Telnet)
    add_subdirectory(LibSSH)
endif()

