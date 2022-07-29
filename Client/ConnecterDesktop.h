#ifndef CCONNECTERDESKTOP_H
#define CCONNECTERDESKTOP_H

#include "ConnecterDesktopThread.h"

/**
 * \~chinese
 * \brief 它仅实现 Qt 事件(非阻塞)
 * \details
 * 序列图：\image html docs/Image/PluginClientNoBlockSequenDiagram.svg
 * \note 此接口仅由插件实现。
 *       如果要实现一个后台线程处理多个连接，此客户端插件接口必须由 CPluginClientThread 派生。 \n
 * 
 * \~english
 * \brief The class only supports the Qt event (the signal-slot mechanism of QObject, no-block)
 * \details
 * Sequen diagram: \image html docs/Image/PluginClientNoBlockSequenDiagram.svg
 * \note The interface only is implemented by plug-in.
 *       If implements a background thread to handle
 *       multiple remote desktop connections,
 *       The plugin interface must be derived from CPluginClientThread. \n
 * 
 * \~
 * \see CPluginClientThread CPluginThread CManageConnect
 */
class CLIENT_EXPORT CConnecterDesktop : public CConnecterDesktopThread
{
    Q_OBJECT

public:
    CConnecterDesktop(CPluginClient *parent = nullptr);
    virtual ~CConnecterDesktop();

public slots:
    virtual int Connect() override;
    virtual int DisConnect() override;
    
Q_SIGNALS:
    void sigConnect(CConnecterDesktop*);
    void sigDisconnect(CConnecterDesktop*);

};

#endif // CCONNECTERDESKTOP_H
