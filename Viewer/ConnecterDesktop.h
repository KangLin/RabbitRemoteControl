#ifndef CCONNECTERDESKTOP_H
#define CCONNECTERDESKTOP_H

#include "ConnecterDesktopThread.h"

/**
 * \~chinese
 * \brief 它仅实现 Qt 事件(非阻塞)
 * \note 此接口仅由插件实现。
 *       如果要实现一个后台线程处理多个连接，此控制者插件接口必须由 CPluginViewerThread 派生。
 * 
 * \~english
 * \brief The class only supports the Qt event (the signal-slot mechanism of QObject. no-block)
 * \note The interface only is implemented by plug-in.
 *       If implements a background thread to handle
 *       multiple remote desktop connections,
 *       The plugin interface must be derived from CPluginViewerThread 
 * \~  
 * \see CPluginViewerThread CPluginThread CManageConnecter
 */
class VIEWER_EXPORT CConnecterDesktop : public CConnecterDesktopThread
{
    Q_OBJECT

public:
    CConnecterDesktop(CPluginViewer *parent = nullptr);

public slots:
    virtual int Connect() override;
    virtual int DisConnect() override;
    
Q_SIGNALS:
    void sigConnect(CConnecterDesktop*);
    void sigDisconnect(CConnecterDesktop*);

};

#endif // CCONNECTERDESKTOP_H
