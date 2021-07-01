// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTERPLUGINSTERMINAL_H
#define CCONNECTERPLUGINSTERMINAL_H

#include "ConnectThread.h"
#include "Connecter.h"
#include "FrmTermWidget.h"
#include "ParameterTerminal.h"
#include "rabbitremotecontrolterminal_export.h"

class CConnectThreadTerminal;
class RABBITREMOTECONTROLTERMINAL_EXPORT CConnecterPluginsTerminal : public CConnecter
{
    Q_OBJECT
public:
    explicit CConnecterPluginsTerminal(CPluginFactory *parent);
    virtual ~CConnecterPluginsTerminal();

    int OnRun();
    
    QWidget* GetViewer() override;
    virtual qint16 Version() override;
    virtual int Load(QDataStream &d) override;
    virtual int Save(QDataStream &d) override;
    /**
     * @brief Open settings dialog
     * @param parent
     * @return DialogCode
     *   QDialog::Accepted
     *   QDialog::Rejected
     *   -1: error
     */
    virtual int OpenDialogSettings(QWidget* parent = nullptr) override;
    virtual QString ServerName() override;

public Q_SLOTS:
    virtual int Connect() override;
    virtual int DisConnect() override;

private Q_SLOTS:
    void slotTerminalTitleChanged();
    void slotZoomReset();
    
protected:
    virtual QDialog* GetDialogSettings(QWidget *parent) = 0;
    virtual int SetParamter();

    // Instance connect
    virtual CConnect* InstanceConnect();
    /**
     * @brief OnConnect
     * @return 0: emit sigConnected()
     *        <0: emit sigDisconnected();
     *        >0: don't things
     */
    virtual int OnConnect();
    virtual int OnDisConnect();
    virtual int OnLoad(QDataStream& d);
    virtual int OnSave(QDataStream& d);
    
    virtual CParameterTerminal* GetPara() = 0;
    
    CFrmTermWidget* m_pConsole;
    CConnectThreadTerminal* m_pThread;
    bool m_bThread;
    bool m_bExit;
};

#endif // CCONNECTERPLUGINSTERMINAL_H
