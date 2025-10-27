## 修改日志

### v0.1.0-dev
- [重构](https://github.com/KangLin/RabbitRemoteControl/issues/47)
  - App: 重命名界面提示

      | 老名称  |  新名称   |  功能    |
      |--------|----------|----------|
      |远程     | 操作     | 菜单      |
      |远程->连接|操作->开始|菜单或工具条|
      |远程->断开|操作->停止|菜单或工具条|

  - 重命名类名

      |老名称              |新名称             |
      |-------------------|------------------|
      |CConnecter         |COperate          |
      |Connect            |CBackend          |
      |ConnectThread      |CBackendThread    |
      |CConnecterThread   |COperateDesktop   |
      |CPluginClient      |CPlugin           |
      |CClient            |CManager          |
      |CParameterConnecter|CParameterOperate |
      |CParameterClient   |CParameterPlugin  |
      |FrmParameterClient |CParameterPluginUI|

  - 插件
    - 终端
    - SSH 终端
    - Telnet
    - 串口
    - 文件传输
      - SFTP
      - FTP
    - 播放器
      - 增加显示字幕
    - 增加网页浏览器

- 增加活动菜单
- [为 FreeRDP 增加输入法](https://github.com/KangLin/RabbitRemoteControl/issues/48)
- 应用程序
  - [应用程序：从最近列表和活动的列表添加到收藏](https://github.com/KangLin/RabbitRemoteControl/issues/49)
  - 增加视图布局
  - 增加菜单到工具条中
- 增加 macOS 编译脚本。生成　App 捆绑包
- 为 windows 安装包增加“不安装执行功能”。
- [增加存储密码到系统凭据中。](https://github.com/KangLin/RabbitRemoteControl/issues/48)

### v0.0.36
- Client:
  - 修复录制视频帧率错误
- FreeRDP:
  - 增加连接层接口: CConnectLayer
  - 增加 Http 和 Sockes V5 代理
- 修复:
  - LibVNCServer: 因为初始化未成功导制的程序崩溃

### v0.0.35
- 应用：
  - 修改参数
  - 拆分视图增加标签
- FreeRDP:
  - 增加安全参数设置
  - 增加连接类型设置
- Client:
  - 修改键盘事件
  - 修复 Alt、Tab 按键本地窗口接收错误

### v0.0.34
- 应用：增加拆分视图

### v0.0.33
- 移除捐赠比特币
- VNC: 修改鼠标，增加后退按钮
- 增加脚本
  - build_depend.sh
  - build_appimage.sh
  - build_docker.sh
- 自动化集成: 增加 arm64(aarch64) 架构发行版本
- Client: 修改接口
  - CPluginClient
    - 增加 CPluginClient::OnCreateConnecter
    - 修改 CPlginClient::CreateConnecter
  - CClient
    - 修改 CClient::CreateConnecter
    - 修改 CClient::DeleteConnecter
  - CConnecter
    - 增加 CConnecter::GetSettingsFile
    - 增加 CConnecter::SetSettingsFile
- WakeOnLan: 单个实例
- 应用: 修改接口
  - CView
    - 增加 CView::SetCurrentView
- 打包:
  - 增加 flatpak manifests
  - 重命名 Install.nsi 为 Windows.nsi
  - Debian: modify control
  - 增加 rpm 包
- 重命名 org.Rabbit.RemoteControl 为 io.github.KangLin.RabbitRemoteControl
  - 修改相应的 .desktop 文件、图标名、安装脚本
- 应用程序：重命名 CFrmListConnects 为 CFrmListRecnetConnects

### v0.0.32
- 修复播放器菜单错误
- 修复播放器名称错误
- 修复局域网唤醒错误

### v0.0.31
- 使用 [RabbitCommon v2.3.2](https://github.com/KangLin/RabbitCommon/releases/tag/v2.3.2)
- 修复使用 Qt6 时，最大化时，工具栏位置错误。
  现在很多 linux 用 wayland 作为桌面显示，这样会出现一个问题，
  由于没有坐标系统，导致无边框窗体无法拖动和定位（即 QWidge::move 失效）。
 （Qt6 开始强制默认优先用 wayland ，Qt5 默认有 xcb 则优先用 xcb），
  所以需要在 main 函数最前面加一行 `qputenv("QT_QPA_PLATFORM", "xcb")`;
- 修复 AppImage 加载图标失败错误

### v0.0.30
- 客户端: 修复 CFrmParameterClient 不保存参数错误
- 应用程序：修复当应用程序退出时程序崩溃的错误。
  因为退出时，后台线程未退出，导致资源没有正确释放。暂时用等待 1 秒来解决。

### v0.0.29
- FreeRDP
  - 修复服务器不支持剪切板格式返回错误 #31
  - 修复鼠标扩展键引起的程序崩溃

### v0.0.28
- 客户端参数
  - 增加允许自动使用系统用户名作为用户名
  - 增加视图缩放
- 当最大化时，移除窗口边框
- 客户端：
  - 增加 CConnectDesktop
  - 增加 CClient::Details()
  - 增加 CParameterUI
  - 增加 CConnecter::QMenu* GetMenu(QWidget* parent = nullptr)
  - 重命名 CConnecterDesktop 为 CConnecterConnect
  - 重命名 CConnecterDesktopThread 为 CConnecterThread
- FreeRDP: 预设默认参数
  - 修复图像有波纹 [#27](https://github.com/KangLin/RabbitRemoteControl/issues/27) [#21](https://github.com/KangLin/RabbitRemoteControl/issues/27)
    原因是没有启用 FreeRDP_SupportGraphicsPipeline 功能。
- 插件：
  - 增加播放器。需要 Qt >= 6.6.0 。录制需要 Qt >= 6.8.0
  - 增加捕获屏幕或窗口。需要 Qt >= 6.6.0 。录制需要 Qt >= 6.8.0
  - 增加局域网唤醒设备
- 打包
  - 增加 snap 
  - 增加 AppImage

### v0.0.27
- 客户端接口:
  - 在 CConnect 中增加 OnInitReturnValue
  - 修改 Debian 打包
  - 使用 [RabbitCommon v2.2.6](https://github.com/KangLin/RabbitCommon/releases/tag/v2.2.6)
  - 通道
    - 增加 CChannelSSHTunnel
    - 增加 CSSHTunnelThread and CChannelSSHTunnelForward
    - 增加 SSH 隧道
      - TigerVnc: 修改它增加后台线程处理 SSH 隧道。
      - LibVNCServer: 使用本地 SOCKET 转发使用 SSH 隧道。
      - FreeRDP: 使用本地 SOCKET 转发使用 SSH 隧道。
  - 重构参数
    - 增加 CParameterBase
    - 增加 CParameterNet
    - 增加 CParameterUser
    - 增加 CParameterSSH
    - 增加 CParameterProxy
  - TigerVNC: 更新 tigervnc 到 v1.13.90
    - 以后版本用：https://github.com/kanglin/tigervnc f4927c7abb937e41d7dec5ca33c3b1dc87833ef6 以后的
    - 以前版本用：https://github.com/kanglin/tigervnc 4d30e637976fa89f405e4299b7b3aad830e8c7b1 以前的
  - RabbitVNC: 更新 tigervnc 到 v1.13.90
    - 以后版本用：https://github.com/kanglin/rabbitvnc aa3baf4ab1460c548f1694500d45230119002879 以后的
    - 以前版本用：https://github.com/kanglin/rabbitvnc 81d5876d80db5b084e885304344630165840c8ee 以前的
  - FreeRDP: 修复因为键盘、鼠标事件不在后台线程中发送引起的连接中断问题。 [ISSUES #20](https://github.com/KangLin/RabbitRemoteControl/issues/20) 第 10 点
  - LibVNCServer: 修复因为键盘、鼠标事件不在后台线程中发送引起的连接中断问题。

### v0.0.26
- 使用 RabbitCommon v2
- FreeRDP:
  - 优化登录提示，设置窗口
  - 增加重定向
    - 音频输出、输入
    - 驱动器(文件)
    - 打印机
  - 修复同时按下左右鼠标导制的协议出错
- 应用程序增加打开连接配置列表窗口
- 重命名 tag.sh 为 deploy.sh
- 修复：
  - 修复在 LINUX 下有不同版本依赖库时,不能加载正确的依赖库
  - 修复　tab　键事件
- 用 QFileDialog::getOpenFileName 替换 RabbitCommon::CDir::GetOpenFileName 等
- 界面
  - 增加客户端参数
    - 在连接名前显示协议前缀
    - 在连接名中仅显示 IP:PORT
  - 增加主窗口参数
    - 标签显示图标
    - 标签显示提示
  - 优化界面
  - 优化登录提示，设置窗口
    - RabbitVNC
    - TigerVNC
    - LibVNCServer
  - 客户端
    - 增加 sigShowMessage 接口
- 客户端接口
  - 增加　CConnecter::sigDisconnect()
  - 增加　CConnect::sigDisconnect()
  - 增加  CConnect::sigUpdateRect(const QImage& image)

### v0.0.25
- 为自动更新，修改自动化编译分发
- 修改翻译
- 增加图标主题
- FreeRDP:
  - 完成的剪切板功能
    - 文件复制
      - 客户端到服务器端
      - 服务端到客户端（复制目录时会程序会崩溃）
- 修改收藏夹右键菜单错误

### v0.0.24
- 安装 FreeRDP 动态库

### v0.0.23
- 重命名
  - 重命名目录 viewer 到 client
  - 重命名类 CPluginViewer 到 CPluginClient
  - 重命名类 CPluginViewerThread 到 CPluginClientThread
  - 重命名类 CParameterViewer 到 CParameterClient
  - 重命名类 CManagePlugin 到 CClient
  - 重命名函数 CConnecter::SetParameterViewer 到 CConnecter::SetParameterClient
- FreeRDP:
  - 完成的剪切板功能
    - 文本
    - HTML
    - 图片
  - 增加喇叭声

### v0.0.22
- 增加键盘挂钩
  - Windows
- 重构参数

### v0.0.21
- 在 Windows 下，增加导出崩溃文件
- 在 Windows 下，安装 MSVC 系统库。详见: RabbitCommon
- 修复除 0 引起的崩溃。影响前面所有发行版本。此错误是 RabbitCommon 引起的。
  详见：RabbitCommon: 7ed300a206d8a36c30003b6c19ad181682d0ff5f
- Viewer: 在显示对话框中显示 QCheckBox
- Viewer: 优化 CConnecter::Name() 和 CConnecter::ServerName()

### v0.0.20
- 为 Windows 动态库增加版本后缀。详见: RabbitCommon
- RabbitCommon 增加 openssl 版本判断。

### v0.0.19
- 增加 OpenGL 显示窗口
- 增加打开日志功能
- 增加 ICE
  - RabbitVNC
  - TigerVNC
  
### v0.0.18
- 整理文档
- 服务
  - 增加服务配置程序
  - 增加 FreeRDP 服务
  - 增加获得光标
    - linux

### v0.0.17
- 修复从服务器端断连，没有释放线程错误
- 修改 FreeRDP 剪切板
  - 文本
  - 图像

### v0.0.16
- CConnecter 增加阻塞显示对话框
- 增加允许系统托盘图标设置
- 增加玉兔插件
- 修复一些错误
  - 工具栏显示错误
- 支持 Qt6
  
### v0.0.15
- 重构 CConnecter 参数接口
- 重构 CConneterDesktop 接口
  - 增加 CConneterDesktopThread
  - 增加 PluginViewerThread
- 修改文档
- 增加连接时输入密码
- 增加连接显示消息对话框
- 启动时增加恢复上次关闭的连接
- 修复一些错误
  - 修复得到名称、服务名错误
  - 修复收藏夹错误

### v0.0.14
- 增加系统托盘图标
- 修改文档

### v0.0.13
- 修改界面
  - 增加克隆连接
  - 增加收藏夹功能
- 更新管理密码
  - 增加允许查看密码

### v0.0.12
- 增加设置
- 优化保存密码
- 优化主窗口、工具条
- 修复窗口截屏错误

### v0.0.11
- 修复 View 在正常窗口时不能获得焦点错误
- 增加其它语言翻译文件
- 修复 freerdp 编译错误
- 增加连接时提示密码输入框
  - TigerVNC
  - LibNVCServer
  - FreeRDP

### v0.0.10
- 增加缩放视图
- 修复 libvncserver 初始化客户端错误
- 重构 Viewer 参数
- 增加截屏

### v0.0.9
- 增加服务器
  - RFB
    - TigerVnc
    - LIBVNC
- 增加 ICE
- 重构框架
- 增加 Github actions 自动编译与分发
- 应用：增加由鼠标移动自动调整窗口滚动条
- 修复一些BUG

### v0.0.8
- 修复全屏时 Tab 错误
- 增加代理
  - 更新 TigerVnc 库
- 增加最近打开菜单
- 增加文档
  - 增加编译文档
  - 用 Doxygen 生成开发文档

### v0.0.7
- 用更新后的 RabbitCommon::CStyle 改变样式

### v0.0.6
- 用QUIWidget修改界面，它能自定义窗口标题栏样式

### v0.0.5
- 修复: LIBVNC光标问题
- 支持 Qt6
- 修复: 端口范围问题

### v0.0.4
- 完善自动化编译与分发
- 增加发送 ctl-alt-del

### v0.0.3
- 修复一些 BUG

### v0.0.2
- 修改插件相关接口
- 修改自动化接口
- 增加终端
- 增加 Telnet
- 增加 SSH
- RDP

### v0.0.1
- 界面
  - 屏幕缩放
- 支持的协议
  - RFB
- 显示
  - 屏幕
  - 光标
- 输入事件
  - 键盘
  - 鼠标
- 剪切板(文本)
  - Rdp
- 显示
  - 屏幕
  - 光标
- 输入事件
  - 鼠标
  - 键盘
- 剪切板
