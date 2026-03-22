// 在 ParameterWebBrowser.h 中添加
public:
    QString GetExtensionStoreUrl() const;
    int SetExtensionStoreUrl(const QString &url);
private:
    QString m_szExtensionStoreUrl;

// 在 ParameterWebBrowser.cpp 中实现
QString CParameterWebBrowser::GetExtensionStoreUrl() const
{
    return m_szExtensionStoreUrl;
}

int CParameterWebBrowser::SetExtensionStoreUrl(const QString &url)
{
    if(m_szExtensionStoreUrl == url)
        return 0;
    m_szExtensionStoreUrl = url;
    SetModified(true);
    return 0;
}

// 在 Load 函数中添加
int CParameterWebBrowser::Load(QSettings &set)
{
    // ... 其他代码
    m_szExtensionStoreUrl = set.value("Extension/StoreUrl", 
        "https://api.github.com/repos/KangLin/RabbitRemoteControl/contents/ExtensionStore")
        .toString();
    return 0;
}

// 在 Save 函数中添加
int CParameterWebBrowser::Save(QSettings &set)
{
    // ... 其他代码
    set.setValue("Extension/StoreUrl", m_szExtensionStoreUrl);
    return 0;
}