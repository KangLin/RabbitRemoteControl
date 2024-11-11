# Author: Kang Lin <kl222@126.com>

project(WakeOnLan
    DESCRIPTION "Wake on lan plugin"
    )

find_package(PcapPlusPlus)
if(PcapPlusPlus_FOUND)
    if(PcapPlusPlus_VERSION VERSION_LESS 24.09)
        message("Found PcapPlusPlus ${PcapPlusPlus_VERSION} less 24.09. Please version >=24.09 . "
        "Please build and install from https://github.com/seladb/PcapPlusPlus.git . ")
    endif()
    message(STATUS "Using Pcap++ ${PcapPlusPlus_VERSION}")
    message(STATUS "Include dir: ${PcapPlusPlus_INCLUDE_DIR}")
    list(APPEND WakeOnLan_DEFINITIONS HAVE_PCAPPLUSPLUS)
    list(APPEND WakeOnLan_LIBS PcapPlusPlus::Pcap++)
else()
    message("There is no PcapPlusPlus. "
        "Please build and install from "
        "'git clone https://github.com/seladb/PcapPlusPlus.git ${CMAKE_SOURCE_DIR}/../PcapPlusPlus' "
        "And set PcapPlusPlus_ROOT to install root directory")
endif()

list(APPEND WakeOnLan_LIBS Client)

SET(WakeOnLan_SOURCE_FILES
    PluginWakeOnLan.cpp
    ConnecterWakeOnLan.cpp
    ParameterWakeOnLan.cpp
    ParameterWakeOnLanUI.cpp
    WakeOnLan.cpp
    WakeOnLanQt.cpp
    Arp.cpp
    FrmWakeOnLan.cpp
    WakeOnLanModel.cpp
    )

SET(WakeOnLan_HEADER_FILES
    PluginWakeOnLan.h
    ConnecterWakeOnLan.h
    FrmWakeOnLan.h
    WakeOnLanModel.h
    WakeOnLan.h
    WakeOnLanQt.h
    ParameterWakeOnLan.h
    ParameterWakeOnLanUI.h
    Arp.h
    )

SET(WakeOnLan_UI_FILES
    FrmWakeOnLan.ui
    ParameterWakeOnLanUI.ui
    )

set(WakeOnLan_QT_COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${WakeOnLan_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${WakeOnLan_QT_COMPONENTS})
        list(APPEND WakeOnLan_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()
ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${WakeOnLan_SOURCE_FILES} ${WakeOnLan_HEADER_FILES} ${WakeOnLan_UI_FILES}
    PRIVATE_LIBS ${WakeOnLan_LIBS}
    PRIVATE_INCLUDE_DIRS ${WakeOnLan_INCLUDE_DIR} ${WinPR_INCLUDE_DIR}
    PRIVATE_DEFINITIONS PluginWakeOnLan_VERSION="${RabbitRemoteControl_VERSION}"
    DEFINITIONS ${WakeOnLan_DEFINITIONS}
    PRIVATE_OPTIONS ${WakeOnLan_OPTIONS}
    INSTALL_DIR plugins/Client
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Client
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
    )
