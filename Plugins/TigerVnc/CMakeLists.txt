# Author: Kang Lin <kl222@126.com>

find_package(tigervnc)
message("tigervnc_FOUND:${tigervnc_FOUND}")
if(tigervnc_FOUND)
    if(BUILD_VIEWER)
        add_subdirectory(Viewer)
    endif()
    if(BUILD_SERVICE)
        add_subdirectory(Service)
    endif(BUILD_SERVICE)
endif()
