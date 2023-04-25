# Author: Kang Lin <kl222@126.com>

set(FreeRDPVERSION 2.2.0)
find_package(WinPR)
message("WinPR_FOUND:${WinPR_VERSION}")

find_package(FreeRDP)
message("FreeRDP_FOUND:${FreeRDP_VERSION}")
find_package(FreeRDP-Client)
message("FreeRDP-Client_FOUND:${FreeRDP-Client_VERSION}")

if(NOT (FreeRDP_FOUND AND WinPR_FOUND AND FreeRDP-Client_FOUND))
    message("Don't find FreeRDP ${FreeRDPVERSION}. Please set BUILD_FREERDP=OFF "
        "or install FreeRDP ${FreeRDPVERSION} from https://github.com/FreeRDP/FreeRDP\n"
        "or set:\n"
        "    -DFreeRDP_DIR:PATH=INSTALL_ROOT/lib/cmake/FreeRDP3\n"
        "    -DFreeRDP-Client_DIR:PATH=INSTALL_ROOT/lib/cmake/FreeRDP-Client3\n"
        "    -DWinPR_DIR:PATH=INSTALL_ROOT/lib/cmake/WinPR3"
    )
    return()
endif()
if(FreeRDP_VERSION VERSION_LESS FreeRDPVERSION)
    message("Find FreeRDP ${FreeRDP_VERSION} less request FreeRDP ${FreeRDPVERSION}."
        " Please set BUILD_FREERDP=OFF "
        "or install FreeRDP ${FreeRDPVERSION} from https://github.com/FreeRDP/FreeRDP")
    return()
endif()
set(FreeRDP_MAJOR ${FreeRDP_VERSION_MAJOR})
set(FreeRDP_MINOR ${FreeRDP_VERSION_MINOR})
if(BUILD_CLIENT)
    add_subdirectory(Client)
endif()

find_package(FreeRDP-Shadow ${FreeRDP_VERSION})
message("FreeRDP-Shadow_FOUND:${FreeRDP-Shadow_VERSION}")

find_package(FreeRDP-Server ${FreeRDP_VERSION})
message("FreeRDP-Server_FOUND:${FreeRDP-Server_VERSION}")

if(NOT (FreeRDP-Shadow_FOUND AND FreeRDP-Server_FOUND))
    message("Don't find FreeRDP server\n"
            "Set:\n"
            "    -DFreeRDP-Server_DIR=\n"
            "    -DFreeRDP-Shadow_DIR="
    )
    return()
endif()

if(BUILD_SERVICE)
    add_subdirectory(Service)
endif(BUILD_SERVICE)
