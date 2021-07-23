/**

\~chinese


\defgroup LIBAPI 玉兔远程控制库
\brief 玉兔远程控制库


\defgroup LIBAPI_VIEWER 控制端库
\brief 控制端库
\details 类关系：
\image html docs/Image/ViewerPlugAPI.svg
\ingroup LIBAPI


\defgroup VIEWER_API 控制端应用程序接口
\brief 控制端应用程序接口
\ingroup LIBAPI_VIEWER


\~chinese
\defgroup VIEWER_PLUGIN_API 控制端插件接口
\ingroup LIBAPI_VIEWER
\brief 控制端插件接口.
\details
写一个插件：
- 实现 CPlugin 。例如：\ref CPluginTigerVnc 
  - 在类声明中实现Qt接口:
    \snippet Viewer/Plugins/TigerVnc/PluginFactoryTigerVnc.h Qt plugin interface
  - 在构造函数中初始化操作。例如：初始化资源，加载翻译资源等
    \snippet Viewer/Plugins/TigerVnc/PluginFactoryTigerVnc.cpp Initialize resorce
  - 在析构函数中释放资源。
    \snippet Viewer/Plugins/TigerVnc/PluginFactoryTigerVnc.cpp Clean resource
  - 实现属性、函数
    \include Viewer/Plugins/TigerVnc/PluginFactoryTigerVnc.cpp
- 实现 \ref CConnecter 。例如： \ref CConnecterTigerVnc
  - 实现远程桌面，可以从 \ref CConnecterPlugins 派生
  - 实现远程控制台，可以从 \ref CConnecterPluginsTerminal 派生
  - 如果上面两个不能满足你的需要，你可以直接从 \ref CConnecter 派生
- 实现具体的连接，从 \ref CConnect 派生 。例如：\ref CConnectTigerVnc


\class CManageConnecter
\brief 管理插件和连接者

\fn CConnecter* CManageConnecter::CreateConnecter(const QString& id)
\brief 新建 CConnecter 指针，所有者是调用者。
          <b>当不在使用时，调用者必须释放指针</b>。

\fn CConnecter* CManageConnecter::LoadConnecter(const QString& szFile)
\brief 从文件中新建 CConnecter 指针，所有者是调用者。
 
\fn int CManageConnecter::SaveConnecter(const QString &szFile, CConnecter *pConnecter)
保存参数到文件


\class CPlugin
\brief 控制者插件接口。建立 CConnecter 实例，它由协议插件实现。

\fn CPlugin::CPlugin(QObject *parent)
\brief 初始化操作。例如：初始化资源，加载翻译资源等，例如：
\snippet Viewer/Plugins/TigerVnc/PluginFactoryTigerVnc.cpp Initialize resorce
\note  派生类必须实现它.

\fn CPlugin::~CPlugin()
\brief 派生类实现它，用于释放资源。例如：

\fn CConnecter* CPlugin::CreateConnecter(const QString& szProtol)
\brief 新建 CConnecter 实例。仅由 CManageConnecter 调用
\return 返回 CConnecter 指针, 它的所有者是调用者。
\note 此函数新建一个堆栈对象指针，
      <b>调用者必须负责在用完后释放指针</b>。
\param szProtol 连接协议


\class CConnecter
\brief 描述连接应用接口。
\note 此类是用户接使用接口，由插件实现
\details 
已经提供以下类型的基本实现：
1. 桌面类连接： \ref CConnecterDesktop
2. 控制台类连接：\ref CConnecterPluginsTerminal

\fn CConnecter::CConnecter(CPlugin *parent) 
\param parent: 此指针必须是相应的 CPlugin 派生类的实例指针

\fn QString CConnecter::ServerName()
* \brief 当前连接名。例如：服务名或 IP:端口
* \return 返回服务名

\fn void CConnecter::sigUpdateName(const QString& szName)
\note 在插件中使用时，请使用 slotSetServerName

\fn QWidget* CConnecter::GetViewer()
\brief 得到显示视图
\return CFrmViewer*: 视图指针。它的所有者是本类的实例

\fn int CConnecter::OpenDialogSettings(QWidget* parent = nullptr)
\brief 打开设置对话框
\param parent
\return DialogCode
  \li QDialog::Accepted: 接收
  \li QDialog::Rejected: 拒绝
  \li -1: 错误

\fn int CConnecter::Load(QDataStream& d)
\brief 从文件中加载参数

\fn int CConnecter::Save(QDataStream& d)
\brief 保存参数到文件中

\fn int CConnecter::Connect()
\brief 开始连接
\note 由用户调用，插件不能直接调用此函数。
      插件连接好后，触发信号 sigConnected()

\fn int CConnecter::DisConnect()
\brief 关闭连接
\note 由用户调用，插件不能直接调用此函数。
      插件断开连接后，触发信号 sigDisconnected()

      
\class CConnecterDesktop
\brief 它实现一个远程桌面后台线程处理连接
\details
  1. 实现 \ref InstanceConnect() ，生成连接对象
  2. 实现 \ref GetDialogSettings() ，得到参数对话框
\note 此接口仅由插件实现
\see CConnectThread

\fn CConnect* CConnecterDesktop::InstanceConnect()
\brief 新建 CConnect 对象。它的所有者是调用者，如果调用者不再使用它，必须释放它。

\fn QDialog* CConnecterDesktop::GetDialogSettings(QWidget* parent = nullptr)
\brief 得到设置对话框
\param parent: 返回窗口的父窗口
\return QDialog*: 插件实现时，此对话框必须设置属性 Qt::WA_DeleteOnClose，
                  它的所有者是调用者


\class CFrmViewer
\~chinese
\brief 
 用于显示从 CConnect 输出的图像，和向 CConnect 发送键盘、鼠标事件。
 当新的输出图像从 CConnect 输出时，它调用 slotUpdateRect() 更新显示。
 
\enum CFrmViewer::ADAPT_WINDOWS
窗口适配枚举常量
\var CFrmViewer::ADAPT_WINDOWS CFrmViewer::Auto
自动适配
\var CFrmViewer::ADAPT_WINDOWS CFrmViewer::Disable
禁用适配窗口
\var CFrmViewer::ADAPT_WINDOWS CFrmViewer::Original
原始桌面大小，桌面的左上点与窗口的左上点对齐
\var CFrmViewer::ADAPT_WINDOWS CFrmViewer::OriginalCenter
原始桌面大小，桌面中心点与窗口中心点对齐
\var CFrmViewer::ADAPT_WINDOWS CFrmViewer::Zoom
桌面缩放到窗口大小
\var CFrmViewer::ADAPT_WINDOWS CFrmViewer::AspectRation
保持长宽比缩放到窗口大小

*/
