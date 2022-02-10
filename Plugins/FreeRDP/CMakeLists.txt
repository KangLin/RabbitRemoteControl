# Author: Kang Lin <kl222@126.com>

find_package(WinPR)
message("WinPR_FOUND:${WinPR_FOUND}")

find_package(FreeRDP)
message("FreeRDP_FOUND:${FreeRDP_FOUND}")

find_package(FreeRDP-Client)
message("FreeRDP-Client_FOUND:${FreeRDP-Client_FOUND}")

if(NOT (FreeRDP_FOUND AND WinPR_FOUND AND FreeRDP-Client_FOUND))
    return()
endif()

if(BUILD_VIEWER)
    add_subdirectory(Viewer)
endif()
if(BUILD_SERVICE)
    add_subdirectory(Service)
endif(BUILD_SERVICE)
