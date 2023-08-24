## 修改日志

### v0.0.26
+ 使用 RabbitCommon v2
+ FreeRDP:
  + 增加重定向
    + 音频输出、输入
    + 驱动器(文件)
    + 打印机
+ 应用程序增加打开连接配置列表窗口
+ 重命名 tag.sh 为 deploy.sh
+ 修复：
  - 修复在 LINUX 下有不同版本依赖库时,不能加载正确的依赖库
  - 修复　tab　键事件
+ 用 QFileDialog::getOpenFileName 替换 RabbitCommon::CDir::GetOpenFileName 等
+ 界面
  + 增加客户端参数
    + 在连接名前显示协议前缀
    + 在连接名中仅显示 IP:PORT
  + 增加主窗口参数
    + 标签显示图标
    + 标签显示提示

### v0.0.25
+ 为自动更新，修改自动化编译分发
+ 修改翻译
+ 增加图标主题
+ FreeRDP:
  - 完成的剪切板功能
    + 文件复制
      - 客户端到服务器端
      - 服务端到客户端（复制目录时会程序会崩溃）
+ 修改收藏夹右键菜单错误

### v0.0.24
+ 安装 FreeRDP 动态库

### v0.0.23
+ 重命名
  - 重命名目录 viewer 到 client
  - 重命名类 CPluginViewer 到 CPluginClient
  - 重命名类 CPluginViewerThread 到 CPluginClientThread
  - 重命名类 CParameterViewer 到 CParameterClient
  - 重命名类 CManagePlugin 到 CClient
  - 重命名函数 CConnecter::SetParameterViewer 到 CConnecter::SetParameterClient
+ FreeRDP:
  - 完成的剪切板功能
    + 文本
    + HTML
    + 图片
  - 增加喇叭声

### v0.0.22
+ 增加键盘挂钩
  - Windows
+ 重构参数

### v0.0.21
+ 在 Windows 下，增加导出崩溃文件
+ 在 Windows 下，安装 MSVC 系统库。详见: RabbitCommon
+ 修复除 0 引起的崩溃。影响前面所有发行版本。此错误是 RabbitCommon 引起的。
  详见：RabbitCommon: 7ed300a206d8a36c30003b6c19ad181682d0ff5f
+ Viewer: 在显示对话框中显示 QCheckBox
+ Viewer: 优化 CConnecter::Name() 和 CConnecter::ServerName()

### v0.0.20
+ 为 Windows 动态库增加版本后缀。详见: RabbitCommon
+ RabbitCommon 增加 openssl 版本判断。

### v0.0.19
+ 增加 OpenGL 显示窗口
+ 增加打开日志功能
+ 增加 ICE
  - RabbitVNC
  - TigerVNC
  
### v0.0.18
+ 整理文档
+ 服务
  - 增加服务配置程序
  - 增加 FreeRDP 服务
  - 增加获得光标
    + linux

### v0.0.17
+ 修复从服务器端断连，没有释放线程错误
+ 修改 FreeRDP 剪切板
  - 文本
  - 图像

### v0.0.16
+ CConnecter 增加阻塞显示对话框
+ 增加允许系统托盘图标设置
+ 增加玉兔插件
+ 修复一些错误
  - 工具栏显示错误
+ 支持 Qt6
  
### v0.0.15
+ 重构 CConnecter 参数接口
+ 重构 CConneterDesktop 接口
  + 增加 CConneterDesktopThread
  + 增加 PluginViewerThread
+ 修改文档
+ 增加连接时输入密码
+ 增加连接显示消息对话框
+ 启动时增加恢复上次关闭的连接
+ 修复一些错误
  - 修复得到名称、服务名错误
  - 修复收藏夹错误

### v0.0.14
+ 增加系统托盘图标
+ 修改文档

### v0.0.13
+ 修改界面
  - 增加克隆连接
  - 增加收藏夹功能
+ 更新管理密码
  - 增加允许查看密码

### v0.0.12
+ 增加设置
+ 优化保存密码
+ 优化主窗口、工具条
+ 修复窗口截屏错误

### v0.0.11
+ 修复 View 在正常窗口时不能获得焦点错误
+ 增加其它语言翻译文件
+ 修复 freerdp 编译错误
+ 增加连接时提示密码输入框
  - TigerVNC
  - LibNVCServer
  - FreeRDP

### v0.0.10
+ 增加缩放视图
+ 修复 libvncserver 初始化客户端错误
+ 重构 Viewer 参数
+ 增加截屏

### v0.0.9
+ 增加服务器
  - RFB
    + TigerVnc
    + LIBVNC
+ 增加 ICE
+ 重构框架
+ 增加 Github actions 自动编译与分发
+ 应用：增加由鼠标移动自动调整窗口滚动条
+ 修复一些BUG

### v0.0.8
+ 修复全屏时 Tab 错误
+ 增加代理
  - 更新 TigerVnc 库
+ 增加最近打开菜单
+ 增加文档
  - 增加编译文档
  - 用 Doxygen 生成开发文档

### v0.0.7
+ 用更新后的 RabbitCommon::CStyle 改变样式

### v0.0.6
+ 用QUIWidget修改界面，它能自定义窗口标题栏样式

### v0.0.5
+ 修复: LIBVNC光标问题
+ 支持 Qt6
+ 修复: 端口范围问题

### v0.0.4
+ 完善自动化编译与分发
+ 增加发送 ctl+alt+del

### v0.0.3
+ 修复一些 BUG

### v0.0.2
+ 修改插件相关接口
+ 修改自动化接口
+ 增加终端
+ 增加 Telnet
+ 增加 SSH
+ RDP

### v0.0.1
+ 界面
  - 屏幕缩放
+ 支持的协议
  - RFB
+ 显示
  - 屏幕
  - 光标
+ 输入事件
  - 键盘
  - 鼠标
+ 剪切板(文本)
  - Rdp
+ 显示
  - 屏幕
  - 光标
+ 输入事件
  - 鼠标
  - 键盘
+ 剪切板
