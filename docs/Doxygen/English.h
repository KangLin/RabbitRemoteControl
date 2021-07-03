/**
\~english

\defgroup LIBAPI Rabbit remote control library
\brief Rabbit remote control library


\defgroup LIBAPI_VIEWER Viewer library
\brief Viewer library
\details Class relationship
\image html docs/Image/ViewerPlugAPI.svg
\ingroup LIBAPI

\defgroup VIEWER_API Viewer application interface
\brief Viewer application interface
\ingroup LIBAPI_VIEWER

\defgroup VIEWER_PLUGIN_API Viewer plugin interfaces
\ingroup LIBAPI_VIEWER
\brief Viewer plugin interfaces.
\details
  Write a plugin:
- Implement CPluginFactory. For example: \ref CPluginFactoryTigerVnc
  - Implement the Qt interface in the class declaration:
    \snippet Viewer/Plugins/TigerVnc/PluginFactoryTigerVnc.h Qt plugin interface
  - Initialize the operation in the constructor. For example: initializing resources, loading translation resources, etc.
    \snippet Viewer/Plugins/TigerVnc/PluginFactoryTigerVnc.cpp Initialize resorce
  - Release resources in the destructor.
     \snippet Viewer/Plugins/TigerVnc/PluginFactoryTigerVnc.cpp Clean resource
  - Implement properties and functions
     \include Viewer/Plugins/TigerVnc/PluginFactoryTigerVnc.cpp
- Implement \ref CConnecter. For example: \ref CConnecterTigerVnc
  - Implement remote desktop, which can be derived from \ref CConnecterPlugins
  - Implement remote console, which can be derived from \ref CConnecterPluginsTerminal
  - If the above two cannot meet your needs, you  can be derived from \ref CConnecter
-Implement a specific connection, derived from \ref CConnect. For example: \ref CConnectTigerVnc 

*/
