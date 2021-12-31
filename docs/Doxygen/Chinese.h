/**

\~chinese


\defgroup LIBAPI 玉兔远程控制库
\brief 玉兔远程控制库


\defgroup LIBAPI_SERVICE 服务端库
\brief 服务端库
\details 类关系：
\image html docs/Image/PluginServiceAPI.svg
\ingroup LIBAPI


\defgroup LIBAPI_VIEWER 控制端库
\brief 控制端库
\details 
+ 控制端插件接口类关系：
  \image html docs/Image/PluginViewerAPI.svg
\ingroup LIBAPI


\defgroup VIEWER_API 控制端应用程序接口
\brief 控制端应用程序接口
\ingroup LIBAPI_VIEWER


\defgroup VIEWER_PLUGIN_API 控制端插件接口
\ingroup LIBAPI_VIEWER
\brief 控制端插件接口.
\details
+ 类关系：
  \image html docs/Image/PluginViewerAPI.svg
+ 写一个插件：
  - 生成插件目标名称格式为： PluginViewer${PROJECT_NAME}
    注意：工程名必须与翻译资源文件名(.ts)相同
    \include Plugins/TigerVnc/Viewer/CMakeLists.txt
  - 实现 CPlugin 。例如：\ref CPluginTigerVnc 
    - 在类声明中实现Qt接口:
    \snippet Plugins/TigerVnc/Viewer/PluginTigerVnc.h Qt plugin interface
    - 在构造函数中初始化操作。例如：初始化资源等
    \snippet Plugins/TigerVnc/Viewer/PluginTigerVnc.cpp Initialize resorce
    - 在析构函数中释放资源。
    \snippet Plugins/TigerVnc/Viewer/PluginTigerVnc.cpp Clean resource
    - 实现属性、函数
      - 插件名：必须与翻译资源文件名(.ts)相同 \ref CPluginTigerVnc::Name() 
      \include Plugins/TigerVnc/Viewer/PluginTigerVnc.cpp
  - 实现 \ref CConnecter 。例如： \ref CConnecterTigerVnc
    - 实现远程桌面，可以从 \ref CConnecterDesktopThread 派生
    - 实现远程控制台，可以从 \ref CConnecterTerminal 派生
    - 如果上面两个不能满足你的需要，你可以直接从 \ref CConnecter 派生
  - 实现具体的连接，从 \ref CConnect 派生 。例如：\ref CConnectTigerVnc

\defgroup LIBAPI_THREAD 线程模型
\ingroup LIBAPI_VIEWER
\brief 线程模型

*/
