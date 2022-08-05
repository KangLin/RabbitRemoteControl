# Author: Kang Lin <kl222@126.com>

set(FreeRDP_VERSION 2.8.0)
find_package(WinPR ${FreeRDP_VERSION})
message("WinPR_FOUND:${WinPR_FOUND}")

find_package(FreeRDP ${FreeRDP_VERSION})
message("FreeRDP_FOUND:${FreeRDP_FOUND}")

find_package(FreeRDP-Client ${FreeRDP_VERSION})
message("FreeRDP-Client_FOUND:${FreeRDP-Client_FOUND}")

if(NOT (FreeRDP_FOUND AND WinPR_FOUND AND FreeRDP-Client_FOUND))
    message("Don't find FreeRDP ${FreeRDP_VERSION}. Please set BUILD_FREERDP=OFF "
        "or install FreeRDP ${FreeRDP_VERSION} from https://github.com/FreeRDP/FreeRDP")
    return()
endif()

if(BUILD_CLIENT)
    add_subdirectory(Client)
endif()

find_package(FreeRDP-Shadow ${FreeRDP_VERSION})
message("FreeRDP-Shadow_FOUND:${FreeRDP-Shadow_FOUND}")

find_package(FreeRDP-Server ${FreeRDP_VERSION})
message("FreeRDP-Server_FOUND:${FreeRDP-Server_FOUND}")

if(NOT (FreeRDP-Shadow_FOUND AND FreeRDP-Server_FOUND))
    return()
endif()

if(BUILD_SERVICE)
    add_subdirectory(Service)
endif(BUILD_SERVICE)
