/**
\~english

\defgroup APP Applications

\defgroup LIBAPI Libraries

\defgroup LIBAPI_SERVICE Service library
\ingroup LIBAPI
\brief Service library
\details

+ Threading model: A plugin starts a thread to process a service.
    See the specific implementation: CPluginServiceThread and CService
+ Class relationship
  \image html docs/Image/PluginServiceAPI.svg
+ usge:
  - Implement plugin interface:
    + CPluginService: Plugin interface
    + CPluginServiceThread: The default start a new thread. eg. CPluginServiceRabbitVNC
      - Plugin name: This name must be the same as the project name (${PROJECT_NAME}).
        The translation file (${PROJECT_NAME}_*.ts)) name is associated with it. 
        E.g. CPluginServiceRabbitVNC::Name() 
  - Implement CService . Eg. CServiceRabbitVNC


\defgroup LIBAPI_CLIENT Client library
\ingroup LIBAPI
\brief Client library
\details 
- Client plugin interfaces Class relationship
  \image html docs/Image/PluginViewerAPI.svg
- Sequence diagram
  \image html docs/Image/PluginViewerSequenceDiagram.svg


\defgroup VIEWER_API Client application interface
\ingroup LIBAPI_CLIENT
\brief Client application interface
\details
+ Class relationship
  \image html docs/Image/PluginViewerAPI.svg
+ usge:
  - Instance class: CManagePlugin m_ManageConnecter;
    + Handle CManagePlugin::Handle::onProcess to get the registered connection plugin
  - Use one of the following methods to get the connecter object:
    + \ref CManagePlugin::CreateConnecter
    + \ref CManagePlugin::LoadConnecter
  - Connect signal:
    + Connect CConnecter::sigConnected
    + Connect CConnecter::sigDisconnected
  - Open connect: CConnecter::Connect()
  - After receiving the \ref CConnecter::sigConnected signal, do connection-related initialization work
  - Close connect: CConnecter::DisConnect()
  - After receiving the \ref CConnecter::sigDisconnected signal, do connection-related cleanup work, and delete the connector object after completion ( CConnecter::deleteLater )


\defgroup VIEWER_PLUGIN_API Client plugin interfaces
\ingroup LIBAPI_CLIENT
\brief Client plugin interfaces.
\details
+ Thread module
  - Blocked: Most control protocol implementation library connections are blocking.
    \see CPluginClient CConnecterDesktopThread
  - No-blocking: eg: qt event. A thread can handle multiple connections.
    \see CPluginClientThread CConnecterDesktop
+ Class relationship
  \image html docs/Image/PluginViewerAPI.svg
+ Sequence diagram
  \image html docs/Image/PluginViewerSequenceDiagram.svg
+ Write a plugin:
  - The format of the generated plug-in target name is: PluginClient${PROJECT_NAME}
    \include Plugins/FreeRDP/Client/CMakeLists.txt
  - Implement CPluginClient. 
    + Blocked: A background thread handles a connection. The connection is blocked.
      E.g. FreeRDP
      - Derive from CPluginClient. For example: CPluginFreeRDP 
        + Implement the Qt interface in the class declaration:
          \snippet Plugins/FreeRDP/Client/PluginFreeRDP.h Qt plugin interface
        + Initialize the operation in the constructor.
          For example: initializing resources, loading translation resources, etc.
        + Release resources in the destructor.
        + Implement properties and functions
          - Plugin name: This name must be the same as the project name (${PROJECT_NAME}).
            The translation file (${PROJECT_NAME}_*.ts)) name is associated with it. 
            E.g. CPluginFreeRDP::Name() 
             \include Plugins/FreeRDP/Client/PluginFreeRDP.cpp
    + No-blocking: One background thread handles multiple connections.
      The connection is non-blocking. E.g. RabbitVNC 
      - Derive from CPluginClientThread. For example: CPluginRabbitVNC
        + Implement the Qt interface in the class declaration:
          \snippet Plugins/RabbitVNC/Client/PluginRabbitVNC.h Qt plugin interface
        + Initialize the operation in the constructor.
          For example: initializing resources, loading translation resources, etc.
          \snippet Plugins/RabbitVNC/Client/PluginRabbitVNC.cpp Initialize resource
        + Release resources in the destructor.
        + Implement properties and functions
          - Plugin name: This name must be the same as the project name (${PROJECT_NAME}).
            The translation file (${PROJECT_NAME}_*.ts)) name is associated with it. 
            E.g. CPluginRabbitVNC::Name() 
             \include Plugins/RabbitVNC/Client/PluginRabbitVNC.cpp
  - Implement \ref CConnecter. 
    - Implement remote desktop
      - Blocked: Implements a remote desktop background thread to handle
        a remote desktop connection, which can be derived from CConnecterDesktopThread. Eg: CConnecterFreeRDP
        \image html docs/Image/PluginViewerThreadSequenceDiagram.svg
      - No-blocking: Implements a background thread to handle multiple remote desktop connections,
        which can be derived from CConnecterDesktop. Eg: CConnecterRabbitVNC
        \image html docs/Image/PluginViewerManageConnectSequenDiagram.svg
    - Implement remote console, which can be derived from CConnecterTerminal
    - If the above two cannot meet your needs, you  can be derived from CConnecter
  - Implement a specific connection, derived from CConnect. For example: CConnectFreeRDP 

\defgroup LIBAPI_THREAD Thread module
\ingroup LIBAPI_CLIENT
\brief Thread module

\defgroup LIBAPI_CHANNEL Channel
\brief Stream data channel
\ingroup LIBAPI
\details
+ Class relationship
  \image html docs/Image/Channel.svg
  
\defgroup LIBAPI_ICE ICE
\ingroup LIBAPI_CHANNEL
\details
+ Class relationship
  \image html docs/Image/Channel.svg
  
*/
