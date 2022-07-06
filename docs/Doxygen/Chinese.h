/**

\~chinese

\defgroup APP 程序

\defgroup LIBAPI 库

\defgroup LIBAPI_SERVICE 服务端库
\ingroup LIBAPI
\brief 服务端库
\details 
+ 类关系：
  \image html docs/Image/PluginServiceAPI.svg
+ 写一个插件：
  - 实现插件接口:
    + CPluginService 在主线程中处理
    + CPluginServiceThread: 默认为每个插件启动一个线程进行处理。例如： CPluginServiceRabbitVNC
      - 插件名：必须与工程名（翻译资源文件名[.ts]）相同。 例如： \ref CPluginServiceRabbitVNC::Name() 
  - 实现服务接口 CService 。支持Qt事件或非Qt事件循环。例如： CServiceRabbitVNC


\defgroup LIBAPI_VIEWER 控制端库
\ingroup LIBAPI
\brief 控制端库
\details 
+ 控制端插件接口类关系：
  \image html docs/Image/PluginViewerAPI.svg
+ 序列图：
  \image html docs/Image/PluginViewerSequenceDiagram.svg


\defgroup VIEWER_API 控制端应用程序接口
\ingroup LIBAPI_VIEWER
\brief 控制端应用程序接口
\details 
+ 类关系：
  \image html docs/Image/PluginViewerAPI.svg
+ 使用：
  - 实例化类： \ref CManagePlugin m_ManageConnecter;
    + 处理 CManagePlugin::Handle::onProcess 得到注册的连接插件
  - 使用下面一种方式得到连接者对象：
    + \ref CManagePlugin::CreateConnecter
    + \ref CManagePlugin::LoadConnecter
  - 关联信号：
    + 连接成功信号： \ref CConnecter::sigConnected
    + 断连信号： \ref CConnecter::sigDisconnected
  - 打开连接： CConnecter::Connect()
  - 接收到 \ref CConnecter::sigConnected 信号后，做连接相关初始化工作
  - 关闭连接： CConnecter::DisConnect()
  - 接收到 \ref CConnecter::sigDisconnected 信号后，做连接相关的清理工作，完成后并删除此连接者对象( CConnecter::deleteLater )
    

\defgroup VIEWER_PLUGIN_API 控制端插件接口
\ingroup LIBAPI_VIEWER
\brief 控制端插件接口.
\details
+ 类关系：
  \image html docs/Image/PluginViewerAPI.svg
+ 序列图：
  \image html docs/Image/PluginViewerSequenceDiagram.svg
+ 写一个插件：
  - 生成插件目标名称格式为： PluginViewer${PROJECT_NAME}
    \include Plugins/FreeRDP/Viewer/CMakeLists.txt
  - 实现插件接口 CPluginViewer
    + 一个后台线程处理一个连接。连接可能是阻塞的。例如：FreeRDP
      - 从 CPluginViewer 派生插件。例如： \ref CPluginFreeRDP
        + 在类声明中实现Qt接口:
          \snippet Plugins/FreeRDP/Viewer/PluginFreeRDP.h Qt plugin interface
    + 一个后台线程处理多个连接。连接是非阻塞的。 例如 RabbitVNC
      - 从  CPluginViewerThread 派生插件。例如： \ref CPluginRabbitVNC
        + 在类声明中实现Qt接口:
          \snippet Plugins/RabbitVNC/Viewer/PluginRabbitVNC.h Qt plugin interface
        + 在构造函数中初始化操作。例如：初始化资源等
          \snippet Plugins/RabbitVNC/Viewer/PluginRabbitVNC.cpp Initialize resorce
        + 在析构函数中释放资源。
        + 实现属性、函数
          - 插件名：必须与工程名（翻译资源文件名[.ts]）相同。 例如： \ref CPluginFreeRDP::Name() 
            \include Plugins/FreeRDP/Viewer/PluginFreeRDP.cpp
  - 实现连接者接口 \ref CConnecter 。
    + 实现远程桌面
      - 实现一个后台线程处理一个远程连接，连接是阻塞的。可以从 \ref CConnecterDesktopThread 派生。例如：\ref CConnecterFreeRDP
        \image html docs/Image/PluginViewerThreadSequenceDiagram.svg
      - 实现一个后台线程处理多个远程连接，连接是非阻塞的。可以从 \ref CConnecterDesktop 派生。例如：\ref CConnecterRabbitVNC
        \image html docs/Image/PluginViewerManageConnectSequenDiagram.svg
    + 实现远程控制台，可以从 \ref CConnecterTerminal 派生
    + 如果上面两个不能满足你的需要，你可以直接从 \ref CConnecter 派生
  - 实现具体的连接，从 \ref CConnect 派生 。例如：\ref CConnectFreeRDP

\defgroup LIBAPI_THREAD 线程模型
\ingroup LIBAPI_VIEWER
\brief 线程模型

\defgroup LIBAPI_CHANNEL 通道
\ingroup LIBAPI
\brief 流式数据通道
\details
+ 类关系图
  \image html docs/Image/Channel.svg
  
\defgroup LIBAPI_ICE ICE
\ingroup LIBAPI_CHANNEL
\details
+ 类关系图
  \image html docs/Image/Channel.svg

*/
