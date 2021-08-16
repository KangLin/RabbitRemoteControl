// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTERBACKTHREAD_H
#define CCONNECTERBACKTHREAD_H

#pragma once

#include "Connect.h"
#include "Parameter.h"
#include "FrmViewer.h"

class CConnectThread;

/**
 * \~english
 * \brief it is implement a background connect thread of the remote desktop.
 * \note The interface only is implemented by plugin
 * \details 
 *   1. Implement \ref InstanceConnect()
 *   2. Implement \ref GetDialogSettings
 *   3. Implement \ref OnLoad(QDataStream& d) and \ref OnSave(QDataStream& d)
 * \see CConnectThread
 */
class VIEWER_EXPORT CConnecterDesktop : public CConnecter
{
    Q_OBJECT
public:
    explicit CConnecterDesktop(CPluginViewer *parent = nullptr);
    virtual ~CConnecterDesktop();
    
    virtual const QString Name();
    virtual QWidget* GetViewer() override;
    virtual QString ServerName() override;
    virtual int Load(QDataStream &d) override;
    virtual int Save(QDataStream &d) override;
    
    /// \~english New connect. the ownership is caller.
    ///  if don't use, the caller must delete it.
    virtual CConnect* InstanceConnect() = 0;

public Q_SLOTS:
    virtual int Connect() override;
    virtual int DisConnect() override;

protected:
    virtual int OnLoad(QDataStream& d);
    virtual int OnSave(QDataStream& d);

private:
    CConnectThread* m_pThread;
    CFrmViewer *m_pView;

protected:
    CParameter* m_pParameter;
};

#endif // CCONNECTERBACKTHREAD_H
