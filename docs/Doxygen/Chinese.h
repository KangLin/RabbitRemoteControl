/**

\~chinese

\defgroup APP 程序

\defgroup LIBAPI 库

\defgroup LIBAPI_SERVICE 服务端库
\ingroup LIBAPI
\brief 服务端库
\details 

+ 线程模型：一个插件启动一个线程处理一个服务。具体实现见： CPluginServiceThread 和 CService
+ 类关系：
  \image html docs/Image/PluginServiceAPI.svg
+ 写一个插件：
  - 实现插件接口:
    + CPluginService 服务插件接口
    + CPluginServiceThread: 默认为每个插件启动一个线程进行处理。例如： CPluginServiceRabbitVNC
      - 插件名：必须与工程名（翻译资源文件名[.ts]）相同。 例如： \ref CPluginServiceRabbitVNC::Name() 
  - 实现服务接口 CService 。支持Qt事件或非Qt事件循环。例如： CServiceRabbitVNC


\defgroup LIBAPI_CLIENT 客户端库
\ingroup LIBAPI
\brief 客户端库
\details 
+ 客户端插件接口类关系：
  \image html docs/Image/PluginClientAPI.svg
+ 序列图：
  \image html docs/Image/PluginClientSequenceDiagram.svg


\defgroup CLIENT_API 客户端应用程序接口
\ingroup LIBAPI_CLIENT
\brief 客户端应用程序接口
\details 
+ 类关系：
  \image html docs/Image/PluginClientAPI.svg
+ 使用：
  - 实例化类： \ref CClient m_Client;
    + 处理 CClient::Handle::onProcess 得到注册的连接插件
  - 使用下面一种方式得到连接者对象：
    + \ref CClient::CreateConnecter
    + \ref CClient::LoadConnecter
  - 关联信号：
    + 连接成功信号： \ref CConnecter::sigConnected
    + 断连信号： \ref CConnecter::sigDisconnected
  - 打开连接： CConnecter::Connect()
  - 接收到 \ref CConnecter::sigConnected 信号后，做连接相关初始化工作
  - 关闭连接： CConnecter::DisConnect()
  - 接收到 \ref CConnecter::sigDisconnected 信号后，做连接相关的清理工作，完成后并删除此连接者对象 (必须使用 CConnecter::deleteLater )
    

\defgroup CLIENT_PLUGIN_API 客户端插件接口
\ingroup LIBAPI_CLIENT
\brief 客户端插件接口.
\details

+ 线程模型：
  - 阻塞：大多数控制协议实现库连接都是阻塞的。所以需要一个线程处理一个连接。
    \see CPluginClient CConnecterDesktopThread
  - 非阻塞：例如Qt事件。一个线程可以处理多个连接。
    \see CPluginClientThread CConnecterDesktop
+ 类关系：
  \image html docs/Image/PluginClientAPI.svg
+ 序列图：
  \image html docs/Image/PluginClientSequenceDiagram.svg
+ 写一个插件：
  - 生成插件目标名称格式为： PluginClient${PROJECT_NAME}
    \include Plugins/FreeRDP/Client/CMakeLists.txt
  - 实现插件接口 CPluginClient
    + 如果是阻塞线程模型。一个后台线程处理一个连接。连接是阻塞的。例如：FreeRDP
      - 从 CPluginClient 派生插件。例如： \ref CPluginFreeRDP
        + 在类声明中实现Qt接口:
          \snippet Plugins/FreeRDP/Client/PluginFreeRDP.h Qt plugin interface
        + 在构造函数中初始化操作。例如：初始化资源等
        + 在析构函数中释放资源。
        + 实现属性、函数
          - 插件名：必须与工程名（翻译资源文件名[.ts]）相同。 例如： \ref CPluginFreeRDP::Name() 
            \include Plugins/FreeRDP/Client/PluginFreeRDP.cpp
    + 如果非阻塞线程模型。一个后台线程处理多个连接。连接是非阻塞的。 例如：RabbitVNC
      - 从  CPluginClientThread 派生插件。例如： \ref CPluginRabbitVNC
        + 在类声明中实现Qt接口:
          \snippet Plugins/RabbitVNC/Client/PluginRabbitVNC.h Qt plugin interface
        + 在构造函数中初始化操作。例如：初始化资源等
          \snippet Plugins/RabbitVNC/Client/PluginRabbitVNC.cpp Initialize resource
        + 在析构函数中释放资源。
        + 实现属性、函数
          - 插件名：必须与工程名（翻译资源文件名[.ts]）相同。 例如： \ref CPluginRabbitVNC::Name() 
            \include Plugins/RabbitVNC/Client/PluginRabbitVNC.cpp
  - 实现连接者接口 \ref CConnecter 。
    + 实现远程桌面
      - 如果是阻塞线程模型。实现一个后台线程处理一个远程连接，连接是阻塞的。
        可以从 \ref CConnecterDesktopThread 派生。例如：\ref CConnecterFreeRDP
        \image html docs/Image/PluginClientBlockSequenceDiagram.svg
      - 如果非阻塞线程模型。实现一个后台线程处理多个远程连接，连接是非阻塞的。
        可以从 \ref CConnecterDesktop 派生。例如：\ref CConnecterRabbitVNC
        \image html docs/Image/PluginClientNoBlockSequenDiagram.svg
    + 实现远程控制台，可以从 \ref CConnecterTerminal 派生
    + 如果上面两个不能满足你的需要，你可以直接从 \ref CConnecter 派生
  - 实现具体的连接，从 \ref CConnect 派生 。例如：\ref CConnectFreeRDP

\defgroup LIBAPI_THREAD 线程模型
\ingroup LIBAPI_CLIENT
\brief 线程模型
\details
+ 线程模型
  - 阻塞：大多数控制协议实现库连接都是阻塞的。所以需要一个线程处理一个连接。
    \see CPluginClient CConnecterDesktopThread
  - 非阻塞：例如Qt事件。一个线程可以处理多个连接。
    \see CPluginClientThread CConnecterDesktop
+ 类关系：
  \image html docs/Image/PluginClientAPI.svg
  
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
