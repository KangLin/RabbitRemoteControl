# Author: Kang Lin <kl222@126.com>

find_package(RabbitVNC)
message("RabbitVNC_FOUND:${RabbitVNC_FOUND}")
if(RabbitVNC_FOUND)
    if(BUILD_VIEWER)
        add_subdirectory(Viewer)
    endif()
    if(BUILD_SERVICE)
        add_subdirectory(Service)
    endif(BUILD_SERVICE)
endif()
