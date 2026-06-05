/**
\~english

\defgroup APP Applications

\defgroup LIBAPI Libraries

\defgroup LIBAPI_SERVICE Service library(deprecated)
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


\defgroup LIBAPI_PLUGIN Plugin library
\ingroup LIBAPI
\brief Plugin library
\details 
- Plugin interfaces class relationship
  \image html docs/Image/PluginAPI.svg
- Sequence diagram
  \image html docs/Image/PluginSequenceDiagram.svg


\defgroup APP_API Application interface
\ingroup LIBAPI_PLUGIN
\brief Application interface
\details
+ Class relationship
  \image html docs/Image/PluginAPI.svg
- Sequence diagram
  \image html docs/Image/SequenceDiagram.svg
+ usge:
  - Instance class: CManager m_Manager;
    + Handle CManager::Handle::onProcess to get the registered plugin
  - Use one of the following methods to create the object:
    + CManager::CreateConnecter
    + CManager::LoadConnecter
  - Connect signal:
    + Success Signal: COperate::sigRunning
    + Stop signal: COperate::sigStop
    + finish signal: COperate::sigFinished
  - Start: COperate::Start()
  - After receiving the COperate::sigRunning signal,
    do related initialization work
  - After receiving the COperate::sigStop signal,
    Call COperate::Stop() to stop.
  - Stop: COperate::Stop()
  - After receiving the COperate::sigFinished signal,
    do related cleanup work,
    and delete the object after completion
     (MUST use CManager::DeleteOperate(COperate* p) )


\defgroup PLUGIN_API Plugin interfaces
\ingroup LIBAPI_PLUGIN
\brief Plugin interfaces.
\details
+ \ref LIBAPI_THREAD
+ Class relationship
  \image html docs/Image/PluginAPI.svg
+ Sequence diagram
  \image html docs/Image/PluginSequenceDiagram.svg
+ Generate plugin from template using a script: Script/create_plugin.sh
  \code
  ./create_plugin.sh -h

    create_plugin.sh - Generate plugin from template

    Usage: ./Script/create_plugin.sh [OPTION] PluginName

    Options:
      -h, --help            Show this help message
      -v, --verbose[=LEVEL] Set verbose mode. [LEVEL: ON, OFF]

    Directory options:
      --install=DIR         Set installation directory

    Other options:
      --name=NAME           Plugin name
      --template=NAME       Template name. [NAME: Base(Default), Desktop, Server, QtEvent]

    Examples:
      ./Script/create_plugin.sh --name=Server

  \endcode
+ Write a plugin:
  - The format of the generated plug-in target name is: PluginClient${PROJECT_NAME}
    \include Plugins/FreeRDP/Client/CMakeLists.txt
  - Implement CPlugin.
    + No background thread or Blocked background thread
      (A background thread handles a connection. The connection is blocked.).
      E.g. FreeRDP
      - Derive from CPlugin. For example: CPluginFreeRDP
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
    + No-blocking background thread: One background thread handles multiple connections.(Not used yet)
      The connection is non-blocking.
      - Derive from CPluginClientThread.
        + Implement the Qt interface in the class declaration:
          \snippet Plugins/RabbitVNC/Client/PluginRabbitVNC.h Qt plugin interface
        + Initialize the operation in the constructor.
          For example: initializing resources, loading translation resources, etc.
          \snippet Plugins/RabbitVNC/Client/PluginRabbitVNC.cpp Initialize resource
        + Release resources in the destructor.
        + Implement properties and functions
          - Plugin name: This name must be the same as the project name (${PROJECT_NAME}).
            The translation file (${PROJECT_NAME}_*.ts)) name is associated with it. 
            \include Plugins/RabbitVNC/Client/PluginRabbitVNC.cpp
  - Implement \ref COperate.
    - Implement remote desktop
      - Blocked: Implements a remote desktop background thread to handle
        a remote desktop connection, which can be derived from COperateThread. Eg: COperateFreeRDP
        \image html docs/Image/PluginClientBlockSequenceDiagram.svg
      - No-blocking: Implements a background thread to handle multiple remote desktop connections,
        which can be derived from COperateDesktop.
        \image html docs/Image/PluginClientNoBlockSequenDiagram.svg
    - Implement remote console, which can be derived from COperateTerminal
    - Implement server, which can be derived from COperateServer
    - If the above two cannot meet your needs, you  can be derived from COperate
  - Implement a specific connection, derived from COperate. For example: COperateFreeRDP

\defgroup LIBAPI_THREAD Thread module
\ingroup LIBAPI_PLUGIN
\brief Thread module
\details
+ Thread classification
  - Main thread(UI thread)
    - COperate
    - CFrmViewer
    - CParameter
  - Work thread
    - CBackend
+ The module of work thread
  - Blocked: Most control protocol implementation library connections are blocking.
    So one thread handles one connection.
    \see CPlugin CBackendThread
  - No-blocking: eg: qt event. A thread can handle multiple connections.
    - The plugin has no background thread; all connections use the main thread
    - The plugin has a background thread; all connections use the same background thread
      \see CPluginBackendThread CPluginThread COperateTemplateQtEvent
+ Class relationship
  \image html docs/Image/PluginAPI.svg

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


\defgroup LIBAPI_DATABASE Database
\ingroup LIBAPI
\brief Database

*/
