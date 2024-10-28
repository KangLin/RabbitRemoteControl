## Change Log

### v0.0.28
- Client parameters
  - Added the ability to automatically use the system username as the username
  - Add zoom viewer
- App: remove border when full screen
- Client:
  - Add wake on lan
  - Add CConnectDesktop
  - Add CClient::Details()
  - Add CParameterUI
  - Add CConnecter::QMenu* GetMenu(QWidget* parent = nullptr)
  - Rename CConnecterDesktop to CConnecterConnect
  - Rename CConnecterDesktopThread to CConnecterThread
-  FreeRDP: prepare default settings    
  - Fix ripple [#27](https://github.com/KangLin/RabbitRemoteControl/issues/27) [#21](https://github.com/KangLin/RabbitRemoteControl/issues/27)  
    Because of the default is not set FreeRDP_SupportGraphicsPipeline feature
- Plugin
  - Add player. need Qt >= 6.6.0 , record need Qt >= 6.8.0 .
  - Add capture screen or window. need Qt >= 6.6.0 , record need Qt >= 6.8.0 .
- Add snap package

### v0.0.27
- Client:
  - add OnInitReturnValue in CConnect
  - Modify debian package
  - Use [RabbitCommon v2.2.6](https://github.com/KangLin/RabbitCommon/releases/tag/v2.2.6)
  - Refactor parameter
    - Add CParameterBase
    - Add CParameterNet
    - Add CParameterUser
    - Add CParameterSSH
    - Add CParameterProxy
  - TigerVNC: Update tigervnc to v1.13.90
  - RabbitVNC: Update tigervnc to v1.13.90
  - FreeRDP: The synchronization keyboard and mouse event is sent in the background thread.
    Fix: the 10. in [ISSUES #20](https://github.com/KangLin/RabbitRemoteControl/issues/20)
  - LibVNCServer: The synchronization keyboard and mouse event is sent in the background thread.
  - Channel:
    - add CChannelSSHTunnel
    - add CSSHTunnelThread and CChannelSSHTunnelForward
    - Add SSH tunnel
      - TigerVnc: Modify it to add background threads to handle SSH tunnels.
      - LibVNCServer: Use local socket forwarding using SSH tunneling.
      - FreeRDP: Use local socket forwarding using SSH tunneling.

### v0.0.26
- Use RabbitCommon v2
- FreeRDP:
  - Redirection
    - Audio input and output
    - Drive(Files)
    - Printer
  - Fix press the left and right mouse buttons at the same time BUG
- App:
  - Add open the dialog of list connect settings 
- Rename tag.sh to deploy.sh
- Fix:
  - Fixed a bug where the correct dependency library could not be loaded
  when there were different versions of the dependency library under Linux.
  - Fixed tab key event
- Replace RabbitCommon::CDir::GetOpenFileName with QFileDialog::getOpenFileName etc
- UI
  - Add set parameter in client
    - Add prefix in name
    - Add only show IP:PORT in name
  - Add set parameter in main windows
    - Add icon in tab view
    - Add tooltip in tab view
  - Optimize the user interface
  - Optimize logon and setting dialog
    - RabbitVNC
    - TigerVNC
    - LibVNCServer
  - Client:
    - Add sigShowMessage
- Client interface
  - Add CConnecter::sigDisconnect()
  - Add CConnect::sigDisconnect()
  - Add CConnect::sigUpdateRect(const QImage& image)

### v0.0.25
- Modify CI for automation update
- Modify translations
- Add icon theme
- FreeRDP:
  - Clipboard
    - Copy file
      - Client to server
      - Server to client(Copy directory is core)
- Fix pop menu bug in favorite

### v0.0.24
- Install FreeRDP dll

### v0.0.23
- Rename
  - Rename the directory viewer to client
  - Rename the class CPluginViewer to CPluginClient
  - Rename the class CPluginViewerThread to CPluginClientThread
  - Rename the class CParameterViewer to CParameterClient
  - Rename the class CManagePlugin to CClient
  - Rename the functioin CConnecter::SetParameterViewer to CConnecter::SetParameterClient
- FreeRDP:
  - Completed clipboard functions
    - Text
    - HTML
    - Image
  - Add play bell sound

### v0.0.22
- Add keyboard hook
  - Windows
- Refactor parameter

### v0.0.21
- Add core dump in windows
- FIX install MSVC system libraries in windows. See RabbitCommon
- FIX: divide by 0 crash. Affects all previous releases.
  See RabbitCommon: 7ed300a206d8a36c30003b6c19ad181682d0ff5f
- Viewer: add QCheckBox in show dialog
- Viewer: optimize CConnecter::Name() and CConnecter::ServerName()

### v0.0.20
- Add version suffix to Windows dynamic library. see RabbitCommon
- RabbitCommon adds openssl version judgment.

### v0.0.19
- Add OpenGL form
- Add open log
- Add ICE
  - RabbitVNC
  - TigerVNC

### v0.0.18
- Organize documents
- Service
  - Add service configure application
  - Add FreeRDP service
  - Modify Desktop and grab cursor
    - linux

### v0.0.17
- FIX the bug disconnect from server
- Modify FreeRDP clipboard
  - Text
  - Image

### v0.0.16
- CConnecter add block dialog
- Add enable set system tray icon
- Add RabbitVNC plugin
- Fix same bug
  - Toolbar show bug
- Support Qt6
  
### v0.0.15
- Refactor CConneter parameters interface
- Refactor CConneterDesktop interface
  - add CConneterDesktopThread
  - add PluginViewerThread
- Modify documents
- Add input password when is connecting
- Add show message dialog in connect
- Add resume the connections when it was last closed at startup
- Fix same bug
  - Fix CConneter::Name CConneter::ServerName bug
  - Fix favorite bug

### v0.0.14
- Add system tray icon
- Fix same bug

### v0.0.13
- Modify UI
  - Add clone connect
  - Add favorite function
- Update manage password
  - Add enable view password
  
### v0.0.12
- Add settings
- Optimize saving password 
- Optimize main windows、toolbar
- FIX: shot screen window

### v0.0.11
- FIX main window focus bug
- Add other translates
- FIX freerdp compile error
- Add input password dialog when is connecting
  - TigerVNC
  - LibNVCServer
  - FreeRDP

### v0.0.10
- Add zoom view
- FIX: libvncserver initial bug
- Reface viewer parameters
- Add screenslot

### v0.0.9
- Add server
  - RFB
    - TigerVnc
    - LIBVNC
- Add Peer to peer
- Refactor frame
- Add github actions
- App: Add viewer automation adjust scroll bar by mouse move
- Fix some bug

### v0.0.8
- FIX tab view show bug when full screen
- Add proxy
  - tigervnc proxy
- Update tigervnc library
- Add recent open menu
- Modify documents
  - Use Doxygen to generate development documentation
  - Add doxygen to generate develop documentation

### v0.0.7
- Use update RabbitCommon::CStyle to change style

### v0.0.6
- UI: Use QUIWidget to modify the interface,
  it can customize the style of the window title bar

### v0.0.5
- FIX: LibVnc cursor bug
- Support Qt6
- FIX: Port range bug

### v0.0.4
- Modify ci
- Add send ctl-alt-del

### v0.0.3
- Fix some bugs

### v0.0.2
- Modify plugins interface
- Modify ci
- Add terminal
- Add telnet
- Add SSH
- RDP

### v0.0.1
- Gui
  - Screen zoom
- Supported protocols
  - Vnc base functions
- Display
  - Screen
  - Cursor
- Inputs events
  - Key board
  - Mouse
- Clipboard(Text)
  - Rdp
- Display
  - Screen
  - Curson
- Inputs event
  - mouse
  - Key board
- Clipboard
