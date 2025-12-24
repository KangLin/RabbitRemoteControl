# author: Kang Lin <kl222@126.com>

# setup RPATH to where binary is at.
if(APPLE)
    if(PARA_ISEXE)
        SET(INSTALL_RPATH
            "@executable_path:@executable_path/../${CMAKE_INSTALL_LIBDIR}")
    else()
        SET(INSTALL_RPATH
            "@loader_path:@loader_path/../${CMAKE_INSTALL_LIBDIR}")
    endif()
else()
    SET(INSTALL_RPATH
        "$ORIGIN:$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
endif()

SET(PLUGIN_PATH plugins)

## Desktop

option(BUILD_RABBITVNC "Build RabbitVNC plugin" ON)
if(BUILD_RABBITVNC)
    add_subdirectory(RabbitVNC)
endif()

option(BUILD_TigerVNC "Build TigerVnc plugin" ON)
if(BUILD_TigerVNC)
    add_subdirectory(TigerVnc)
endif()

option(BUILD_LibVNCServer "Build LibVNCServer plugin" ON)
if(BUILD_LibVNCServer)
    add_subdirectory(LibVNCServer)
endif()

option(BUILD_FREERDP "Build freerdp plugin" ON)
if(BUILD_FREERDP)
    add_subdirectory(FreeRDP)
endif()

## Tools
option(BUILD_WakeOnLan "Build wake on lan plugin" ON)
if(BUILD_WakeOnLan)
    add_subdirectory(WakeOnLan)
endif()
if(QT_VERSION GREATER_EQUAL "6.8.0")
    option(BUILD_PLAYER "Build player plugin" ON)
    if(BUILD_PLAYER)
        add_subdirectory(Player)
    endif()
    option(BUILD_SCREEN_CAPTURE "Build screen capture" ON)
    if(BUILD_SCREEN_CAPTURE)
        add_subdirectory(ScreenCapture)
    endif()
endif()

option(BUILD_WEB_BROWSER "Build web browser" ON)
if(BUILD_WEB_BROWSER)
    find_package(Qt${QT_VERSION_MAJOR}WebEngineWidgets)
    if(Qt${QT_VERSION_MAJOR}WebEngineWidgets_FOUND)
        add_subdirectory(WebBrowser)
    endif()
endif()

if(QT_VERSION GREATER_EQUAL "5.15.2")
    option(BUILD_FILE_TRANSFER "Build file transfaer" OFF)
    if(BUILD_FILE_TRANSFER)
        add_subdirectory(FileTransfer)
    endif()
endif()

## Terminal
if(WITH_QTERMWIDGET)
    find_package(qtermwidget${QT_VERSION_MAJOR})
    if(qtermwidget${QT_VERSION_MAJOR}_FOUND)
        option(BUILD_TERMINAL "Build Terminal" ON)
        if(BUILD_TERMINAL)
            add_subdirectory(Terminal)
        endif()
        option(BUILD_TERMINAL_TELNET "Build Telnet" ON)
        if(BUILD_TERMINAL_TELNET)
            add_subdirectory(Telnet)
        endif()
        option(BUILD_TERMINAL_SSH "Build libssh" ON)
        if(BUILD_TERMINAL_SSH)
            add_subdirectory(SSH)
        endif()
        option(BUILD_SERIAL_PORT "Build Serial port" ON)
        if(BUILD_SERIAL_PORT)
            add_subdirectory(SerialPort)
        endif()
        option(BUILD_RAW_STREAM "Build raw stream" ON)
        if(BUILD_RAW_STREAM)
            add_subdirectory(RawStream)
        endif()
    endif()
endif()

## Server
option(BUILD_FTP_SERVER "Build ftp server" ON)
if(BUILD_FTP_SERVER)
    add_subdirectory(FtpServer)
endif()

message(STATUS "BUILD_RABBITVNC: ${BUILD_RABBITVNC}")
message(STATUS "BUILD_TigerVNC: ${BUILD_TigerVNC}")
message(STATUS "BUILD_LibVNCServer: ${BUILD_LibVNCServer}")
message(STATUS "BUILD_FREERDP: ${BUILD_FREERDP}")
message(STATUS "BUILD_WakeOnLan: ${BUILD_WakeOnLan}")
message(STATUS "BUILD_PLAYER: ${BUILD_PLAYER}")
message(STATUS "BUILD_SCREEN_CAPTURE: ${BUILD_SCREEN_CAPTURE}")
message(STATUS "BUILD_TERMINAL: ${BUILD_TERMINAL}")
message(STATUS "BUILD_TERMINAL_TELNET: ${BUILD_TERMINAL_TELNET}")
message(STATUS "BUILD_TERMINAL_SSH: ${BUILD_TERMINAL_SSH}")
message(STATUS "BUILD_FILE_TRANSFER: ${BUILD_FILE_TRANSFER}")
message(STATUS "BUILD_WEB_BROWSER: ${BUILD_WEB_BROWSER}; Qt${QT_VERSION_MAJOR}WebEngineWidgets: ${Qt${QT_VERSION_MAJOR}WebEngineWidgets_FOUND}")

message(STATUS "BUILD_FTP_SERVER: ${BUILD_FTP_SERVER}")
