// 在 FrmWebBrowser.h 中添加
private:
    CFrmChromeExtensionManager* m_pExtensionManager;

// 在 FrmWebBrowser.cpp 构造函数中添加
CFrmWebBrowser::CFrmWebBrowser(CParameterWebBrowser* pPara, bool bMenuBar, QWidget *parent)
    : QWidget(parent)
    // ... 其他初始化
    , m_pExtensionManager(nullptr)
{
    // ... 其他代码
    
    // 创建扩展管理器
    m_pExtensionManager = new CFrmChromeExtensionManager(this);
}

// 在 FrmWebBrowser.cpp 初始化菜单中添加
int CFrmWebBrowser::InitMenu(QMenu* pMenu)
{
    if(!pMenu) return -1;
    
    // ... 其他菜单项
    
    // 添加扩展管理菜单
    QAction* pExtensions = pMenu->addAction(tr("Manage Extensions"));
    connect(pExtensions, &QAction::triggered, this, [this]() {
        if(m_pExtensionManager) {
            auto profile = GetProfile(false);
            if(profile) {
                m_pExtensionManager->SetProfile(profile);
            }
            m_pExtensionManager->show();
            m_pExtensionManager->activateWindow();
        }
    });
    
    return 0;
}

// 在启动时加载已安装的扩展
int CFrmWebBrowser::Start()
{
    qDebug(log) << Q_FUNC_INFO;
    
    auto profile = GetProfile(false);
    if(profile && m_pExtensionManager) {
        // 设置扩展管理器
        m_pExtensionManager->SetProfile(profile);
        
        // 加载已安装的扩展
        auto extensions = m_pExtensionManager->GetInstalledExtensions();
        qDebug(log) << "Loaded" << extensions.count() << "extensions";
        
        for(const auto &extId : extensions) {
            auto info = m_pExtensionManager->GetExtensionInfo(extId);
            bool enabled = info.value("enabled", true).toBool();
            
            if(enabled) {
                qDebug(log) << "Extension enabled:" << info.value("name").toString();
            }
        }
    }
    
    return 0;
}