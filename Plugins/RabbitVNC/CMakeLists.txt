# Author: Kang Lin <kl222@126.com>

find_package(RabbitVNC)
message("RabbitVNC_FOUND:${RabbitVNC_FOUND}")
if(RabbitVNC_FOUND)
    if(BUILD_CLIENT)
        add_subdirectory(Client)
    endif()
    if(BUILD_SERVICE)
        add_subdirectory(Service)
    endif(BUILD_SERVICE)
else()
    message("Don't find RabbitVNC. Please set BUILD_RABBITVNC=OFF "
        "or install RabbitVNC from https://github.com/KangLin/RabbitVNC")
endif()
