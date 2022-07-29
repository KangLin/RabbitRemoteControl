# Author: Kang Lin <kl222@126.com>

find_package(tigervnc)
message("tigervnc_FOUND:${tigervnc_FOUND}")
if(tigervnc_FOUND)
    if(BUILD_CLIENT)
        add_subdirectory(Client)
    endif()
    if(BUILD_SERVICE)
        add_subdirectory(Service)
    endif(BUILD_SERVICE)
else()
    message("Don't find TigerVNC. Please set BUILD_TigerVNC=OFF "
        "or install TigerVNC from https://github.com/KangLin/TigerVNC")
endif()
