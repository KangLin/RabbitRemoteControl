project(FreeRdp)

find_package(WinPR)
message("WinPR_FOUND:${WinPR_FOUND}")
if(NOT WinPR_FOUND)
    return()
endif()

find_package(FreeRDP)
message("FreeRDP_FOUND:${FreeRDP_FOUND}")
if(NOT FreeRDP_FOUND)
    return()
endif()

SET(SRC_LIBS winpr freerdp RabbitRemoteControl)
SET(SOURCE_FILES
    ManageConnectFreeRdp.cpp
    ConnectFreeRdp.cpp
    ConnecterFreeRdp.cpp
    ConnectThread.cpp
    )
SET(HEADER_FILES
    ManageConnectFreeRdp.h
    ConnectFreeRdp.h
    ConnecterFreeRdp.h
    ConnectThread.h
    )
SET(UI_FILES
    )

#翻译
GENERATED_QT_TRANSLATIONS(
    SOURCES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES}
    OUT_QRC TRANSLATIONS_QRC_FILES
    )
string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
if(BUILD_TYPE STREQUAL "debug")
    LIST(APPEND PRIVATE_DEFINITIONS DEBUG)
    LIST(APPEND RCC_FILES
        ${TRANSLATIONS_QRC_FILES}
        )
endif()

ADD_PLUGIN_TARGET(NAME ${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
    PRIVATE_LIBS ${SRC_LIBS}
    PRIVATE_INCLUDE_DIRS ${FreeRDP_INCLUDE_DIR} ${WinPR_INCLUDE_DIR}
    )
