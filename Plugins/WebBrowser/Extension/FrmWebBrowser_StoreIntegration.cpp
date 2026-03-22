// 在 FrmWebBrowser.h 中添加
private:
    CFrmExtensionStore* m_pExtensionStore;
    CFrmChromeExtensionManager* m_pExtensionManager;

// 在 FrmWebBrowser.cpp 构造函数中添加
CFrmWebBrowser::CFrmWebBrowser(CParameterWebBrowser* pPara, bool bMenuBar, QWidget *parent)
    : QWidget(parent)
    // ... 其他初始化
    , m_pExtensionStore(nullptr)
    , m_pExtensionManager(nullptr)
{
    // ... 其他代码
    
    // 创建扩展管理器和商城
    m_pExtensionManager = new CFrmChromeExtensionManager(this);
    m_pExtensionStore = new CFrmExtensionStore(this);
    m_pExtensionStore->SetExtensionManager(m_pExtensionManager);
    
    // 设置商城 API 地址 (可从配置读取)
    QString storeUrl = m_pPara->GetExtensionStoreUrl();
    if(!storeUrl.isEmpty()) {
        m_pExtensionStore->SetAPIBaseUrl(storeUrl);
    }
}

// 在菜单初始化中添加
int CFrmWebBrowser::InitMenu(QMenu* pMenu)
{
    if(!pMenu) return -1;
    
    // ... 其他菜单项
    
    // 添加扩展管理菜单
    QMenu* pExtMenu = pMenu->addMenu(tr("Extensions"));
    
    QAction* pManager = pExtMenu->addAction(tr("Manage Extensions"));
    connect(pManager, &QAction::triggered, this, [this]() {
        if(m_pExtensionManager) {
            auto profile = GetProfile(false);
            if(profile) {
                m_pExtensionManager->SetProfile(profile);
            }
            m_pExtensionManager->show();
            m_pExtensionManager->activateWindow();
        }
    });
    
    QAction* pStore = pExtMenu->addAction(tr("Extension Store"));
    connect(pStore, &QAction::triggered, this, [this]() {
        if(m_pExtensionStore) {
            m_pExtensionStore->GetPopularExtensions();
            m_pExtensionStore->show();
            m_pExtensionStore->activateWindow();
        }
    });
    
    return 0;
}