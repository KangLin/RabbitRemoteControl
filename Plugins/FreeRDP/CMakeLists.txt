# Author: Kang Lin <kl222@126.com>

set(FreeRDPVERSION 2.10.0)
find_package(WinPR)
message("WinPR FOUND:${WinPR_VERSION}")

find_package(FreeRDP)
message("FreeRDP FOUND:${FreeRDP_VERSION}")
find_package(FreeRDP-Client)
message("FreeRDP-Client FOUND:${FreeRDP-Client_VERSION}")
if(NOT (FreeRDP_FOUND AND WinPR_FOUND AND FreeRDP-Client_FOUND))
    message(WARNING "Don't find FreeRDP ${FreeRDPVERSION}. Please set BUILD_FREERDP=OFF "
        "or install FreeRDP ${FreeRDPVERSION} from https://github.com/FreeRDP/FreeRDP\n"
        "or set:\n"
        "    -DFreeRDP_DIR:PATH=INSTALL_ROOT/lib/cmake/FreeRDP3\n"
        "    -DFreeRDP-Client_DIR:PATH=INSTALL_ROOT/lib/cmake/FreeRDP-Client3\n"
        "    -DWinPR_DIR:PATH=INSTALL_ROOT/lib/cmake/WinPR3"
    )
    set(BUILD_FREERDP OFF PARENT_SCOPE)
    return()
endif()
if(FreeRDP_VERSION VERSION_LESS FreeRDPVERSION)
    message(WARNING "Find FreeRDP ${FreeRDP_VERSION} less request FreeRDP ${FreeRDPVERSION}."
        " Please set BUILD_FREERDP=OFF "
        "or install FreeRDP ${FreeRDPVERSION} from https://github.com/FreeRDP/FreeRDP")
    set(BUILD_FREERDP OFF PARENT_SCOPE)
    return()
endif()
if(BUILD_CLIENT)
    add_subdirectory(Client)
endif()

if(FreeRDP_VERSION GREATER_EQUAL "3.0.0")
    find_package(WinPR-tools)
    message("WinPR-tools Found: ${WinPR-tools_VERSION}")
    find_package(FreeRDP-Proxy)
    message("FreeRDP-Proxy FOUND: ${FreeRDP-Proxy_VERSION}")
endif()

find_package(FreeRDP-Shadow)
message("FreeRDP-Shadow FOUND:${FreeRDP-Shadow_VERSION}")
find_package(FreeRDP-Server)
message("FreeRDP-Server FOUND:${FreeRDP-Server_VERSION}")

if(NOT (FreeRDP-Server_FOUND AND FreeRDP-Shadow_FOUND))
    message("Don't find FreeRDP server: FreeRDP-Shadow, FreeRDP-Server\n"
        "Set:\n"
        "    -DFreeRDP-Server_DIR=\n"
        "    -DFreeRDP-Shadow_DIR="
    )
    return()
endif()
add_subdirectory(Server)
