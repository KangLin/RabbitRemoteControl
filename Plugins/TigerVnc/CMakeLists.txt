# Author: Kang Lin <kl222@126.com>

find_package(tigervnc)
message("tigervnc_FOUND:${tigervnc_FOUND}")
if(tigervnc_FOUND)
    add_subdirectory(Viewer)
    add_subdirectory(Service)
endif()
