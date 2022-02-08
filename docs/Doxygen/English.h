/**
\~english

\defgroup LIBAPI Rabbit remote control library
\brief Rabbit remote control library

\defgroup LIBAPI_SERVICE Service library
\brief Service library
\details Class relationship
\image html docs/Image/PluginServiceAPI.svg
\ingroup LIBAPI

\defgroup LIBAPI_VIEWER Viewer library
\brief Viewer library
\details 
- Viewer plugin interfaces Class relationship
  \image html docs/Image/PluginViewerAPI.svg
- Sequence diagram
  \image html docs/Image/PluginViewerSequenceDiagram.svg
\ingroup LIBAPI

\defgroup VIEWER_API Viewer application interface
\brief Viewer application interface
\details
+ Class relationship
  \image html docs/Image/PluginViewerAPI.svg
\ingroup LIBAPI_VIEWER

\defgroup VIEWER_PLUGIN_API Viewer plugin interfaces
\ingroup LIBAPI_VIEWER
\brief Viewer plugin interfaces.
\details
+ Class relationship
  \image html docs/Image/PluginViewerAPI.svg
+ Sequence diagram
  \image html docs/Image/PluginViewerSequenceDiagram.svg
+ Write a plugin:
  - The format of the generated plug-in target name is: PluginViewer${PROJECT_NAME}
    \include Plugins/FreeRDP/Viewer/CMakeLists.txt
  - Implement CPluginViewer. 
    + A background thread handles a connection. The connection may be blocked.
      E.g. FreeRDP
      - Derive from CPluginViewer. For example: CPluginFreeRDP 
        + Implement the Qt interface in the class declaration:
          \snippet Plugins/FreeRDP/Viewer/PluginFreeRDP.h Qt plugin interface
    + One background thread handles multiple connections.
      The connection is non-blocking. E.g. RabbitVNC 
      - Derive from CPluginViewerThread. For example: CPluginRabbitVNC
        + Implement the Qt interface in the class declaration:
          \snippet Plugins/RabbitVNC/Viewer/PluginRabbitVNC.h Qt plugin interface
        + Initialize the operation in the constructor.
          For example: initializing resources, loading translation resources, etc.
          \snippet Plugins/RabbitVNC/Viewer/PluginRabbitVNC.cpp Initialize resorce
        + Release resources in the destructor.
        + Implement properties and functions
          - Plugin name: This name must be the same as the project name (${PROJECT_NAME}).
            The translation file (${PROJECT_NAME}_*.ts)) name is associated with it. 
            E.g. CPluginFreeRDP::Name() 
             \include Plugins/FreeRDP/Viewer/PluginFreeRDP.cpp
  - Implement \ref CConnecter. 
    - Implement remote desktop
      - Implements a remote desktop background thread to handle
        a remote desktop connection, which can be derived from CConnecterDesktopThread. Eg: CConnecterFreeRDP
        \image html docs/Image/PluginViewerThreadSequenceDiagram.svg
      - Implements a background thread to handle multiple remote desktop connections,
        which can be derived from CConnecterDesktop. Eg: CConnecterRabbitVNC
        \image html docs/Image/PluginViewerManageConnectSequenDiagram.svg
    - Implement remote console, which can be derived from CConnecterTerminal
    - If the above two cannot meet your needs, you  can be derived from CConnecter
  - Implement a specific connection, derived from CConnect. For example: CConnectFreeRDP 

\defgroup LIBAPI_THREAD Thread module
\ingroup LIBAPI_VIEWER
\brief Thread module

*/
