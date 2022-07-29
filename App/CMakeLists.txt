# Author: Kang Lin <kl222@126.com>

if(BUILD_CLIENT)
    add_subdirectory(Client)
endif(BUILD_CLIENT)
if(BUILD_SERVICE)
    find_package(QtService)
    if(QtService_FOUND)
        add_subdirectory(Service)
        if(WITH_GUI)
            add_subdirectory(ServiceConfigure)
        endif(WITH_GUI)
    endif(QtService_FOUND)
endif(BUILD_SERVICE)
