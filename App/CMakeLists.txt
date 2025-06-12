# Author: Kang Lin <kl222@126.com>

add_subdirectory(Client)
if(BUILD_SERVICE)
    find_package(QtService)
    if(QtService_FOUND)
        add_subdirectory(Service)
        if(WITH_GUI)
            add_subdirectory(ServiceConfigure)
        endif(WITH_GUI)
    endif(QtService_FOUND)
endif(BUILD_SERVICE)
