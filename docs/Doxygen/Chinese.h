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


\defgroup LIBAPI_PLUGIN 插件库
\ingroup LIBAPI
\brief 插件库
\details 
+ 插件接口类关系：
  \image html docs/Image/PluginAPI.svg
+ 序列图：
  \image html docs/Image/PluginSequenceDiagram.svg


\defgroup APP_API 应用程序接口
\ingroup LIBAPI_PLUGIN
\brief 应用程序接口
\details 
+ 类关系：
  \image html docs/Image/PluginAPI.svg
+ 序列图：
  \image html docs/Image/SequenceDiagram.svg
+ 使用：
  - 实例化类： CManager m_Manager;
    + 处理 CManager::Handle::onProcess 得到注册的插件
  - 使用下面一种方式建立对象：
    + CManager::CreateOperate
    + CManager::LoadOperate
  - 关联信号：
    + 成功信号： COperate::sigRunning
    + 停止连接信号： COperate::sigStop
    + 停止成功信号： COperate::sigFinished
  - 开始： COperate::Start()
  - 接收到 COperate::sigRunning 信号后，做相关初始化工作
  - 接收到 COperate::sigStop() 信号后，
    调用 COperate::Stop() 停止。
  - 停止： COperate::Stop()
  - 接收到 COperate::sigFinished 信号后，做相关的清理工作，
   完成后并删除此对象 (必须使用 CManager::DeleteOperate(COperate* p) )
 

\defgroup PLUGIN_API 插件接口
\ingroup LIBAPI_PLUGIN
\brief 插件接口.
\details

+ 线程模型：
  - 阻塞：大多数控制协议实现库连接都是阻塞的。所以需要一个线程处理一个连接。
    \see CPluginClient COperateThread
  - 非阻塞：例如Qt事件。一个线程可以处理多个连接。
    - 插件没有后台线程，所有连接使用主线程
    - 插件有一个后台线程，所有连接使用同一个后台线程
    \see CPluginClientThread COperateDesktop
+ 类关系：
  \image html docs/Image/PluginAPI.svg
+ 序列图：
  \image html docs/Image/PluginSequenceDiagram.svg
+ 写一个插件：
  - 生成插件目标名称格式为： PluginClient${PROJECT_NAME}
    \include Plugins/FreeRDP/Client/CMakeLists.txt
  - 实现插件接口 CPluginClient
    + 如果没有后台线程，或者是阻塞线程模型（一个后台线程处理一个连接。连接是阻塞的）。例如：FreeRDP
      - 从 CPluginClient 派生插件。例如： \ref CPluginFreeRDP
        + 在类声明中实现Qt接口:
          \snippet Plugins/FreeRDP/Client/PluginFreeRDP.h Qt plugin interface
        + 在构造函数中初始化操作。例如：初始化资源等
        + 在析构函数中释放资源。
        + 实现属性、函数
          - 插件名：必须与工程名（翻译资源文件名[.ts]）相同。 例如： \ref CPluginFreeRDP::Name() 
            \include Plugins/FreeRDP/Client/PluginFreeRDP.cpp
    + 如果非阻塞线程模型。一个后台线程处理多个连接。连接是非阻塞的。(暂时未用)
      - 从  CPluginClientThread 派生插件。
        + 在类声明中实现Qt接口:
          \snippet Plugins/RabbitVNC/Client/PluginRabbitVNC.h Qt plugin interface
        + 在构造函数中初始化操作。例如：初始化资源等
          \snippet Plugins/RabbitVNC/Client/PluginRabbitVNC.cpp Initialize resource
        + 在析构函数中释放资源。
        + 实现属性、函数
          - 插件名：必须与工程名（翻译资源文件名[.ts]）相同。
            \include Plugins/RabbitVNC/Client/PluginRabbitVNC.cpp
  - 实现连接者接口 \ref COperate 。
    + 实现远程桌面
      - 如果是阻塞线程模型。实现一个后台线程处理一个远程连接，连接是阻塞的。
        可以从 \ref COperateThread 派生。例如：\ref COperateFreeRDP
        \image html docs/Image/PluginClientBlockSequenceDiagram.svg
      - 如果没有后台线程，或者是非阻塞线程模型(实现一个后台线程处理多个远程连接，连接是非阻塞的)。
        可以从 \ref COperateDesktop 派生。
        \image html docs/Image/PluginClientNoBlockSequenDiagram.svg
    + 实现远程控制台，可以从 \ref COperateTerminal 派生
    + 如果上面两个不能满足你的需要，你可以直接从 \ref COperate 派生
  - 实现具体的连接，从 \ref CConnect 派生 。例如：\ref CConnectFreeRDP

\defgroup LIBAPI_THREAD 线程模型
\ingroup LIBAPI_PLUGIN
\brief 线程模型
\details
+ 线程分类
  - 主线程（UI线程）。以下类在主线程中：
    - COperate
    - CFrmView
    - CParameter
  - 工作线程。以下类在工作线程中：
    - CConnect

+ 工作线程模型
  - 阻塞：大多数控制协议实现库连接都是阻塞的。所以需要一个线程处理一个连接。
         每个连接者启动一个后台线程。
    \see CPluginClient COperateThread
  - 非阻塞：例如Qt事件。一个线程可以处理多个连接。
    插件启动一个线程，连接者重用此线程，它不再启动线程。
    \see CPluginClientThread COperateDesktop

+ 类关系：
  \image html docs/Image/PluginAPI.svg

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
