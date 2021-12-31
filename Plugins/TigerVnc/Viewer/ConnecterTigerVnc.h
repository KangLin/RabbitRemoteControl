// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTERTIGERVNC_H
#define CCONNECTERTIGERVNC_H

#include "ConnecterDesktopThread.h"
#include "DlgSettingsTigerVnc.h"
#include "ParameterTigerVnc.h"

class CConnecterTigerVnc : public CConnecterDesktopThread
{
    Q_OBJECT

public:
    explicit CConnecterTigerVnc(CPluginViewer *parent);
    virtual ~CConnecterTigerVnc() override;
    virtual qint16 Version() override;

protected:
    virtual QDialog* GetDialogSettings(QWidget* parent = nullptr) override;
    virtual CConnect* InstanceConnect() override;
    virtual QString ServerName() override;

public Q_SLOTS:
    virtual int Connect() override;
    virtual int DisConnect() override;

Q_SIGNALS:
    void sigConnect(CConnecter*);
    void sigDisconnect(CConnecter*);
    
private:
    CParameterTigerVnc m_Para;
};

#endif // CCONNECTERTIGERVNC_H
