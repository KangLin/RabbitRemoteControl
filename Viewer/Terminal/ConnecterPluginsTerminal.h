// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTERPLUGINSTERMINAL_H
#define CCONNECTERPLUGINSTERMINAL_H

#include "ConnectThread.h"
#include "Connecter.h"
#include "FrmTermWidget.h"
#include "ParameterTerminal.h"

class CConnectThreadTerminal;
class TERMINAL_EXPORT CConnecterPluginsTerminal : public CConnecter
{
    Q_OBJECT
public:
    explicit CConnecterPluginsTerminal(CPluginViewer *parent);
    virtual ~CConnecterPluginsTerminal() override;
    
    QWidget* GetViewer() override;
    virtual qint16 Version() override;
    virtual int Load(QSettings &set) override;
    virtual int Save(QSettings &set) override;
    
    virtual QString ServerName() override;

    // Instance connect
    virtual CConnect* InstanceConnect();
    
public Q_SLOTS:
    virtual int Connect() override;
    virtual int DisConnect() override;

private Q_SLOTS:
    void slotTerminalTitleChanged();
    void slotZoomReset();
    
protected:
    virtual int SetParamter();

    /**
     * @brief OnConnect
     * @return 0: emit sigConnected()
     *        <0: emit sigDisconnected();
     *        >0: don't things
     */
    virtual int OnConnect();
    virtual int OnDisConnect();
    
    virtual CParameterTerminal* GetPara();
    
    CFrmTermWidget* m_pConsole;
    CConnectThreadTerminal* m_pThread;
    bool m_bThread;
    bool m_bExit;
    CParameterTerminal* m_pPara;
};

#endif // CCONNECTERPLUGINSTERMINAL_H
