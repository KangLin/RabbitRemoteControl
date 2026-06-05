// Author: Kang Lin <kl222@126.com>

#pragma once

#include "Operate.h"

class CBackend;
class CBackendThread;
class CFrmViewerTemplateQtEvent;
class CParameterTemplateQtEvent;
class COperateTemplateQtEvent : public COperate
{
    Q_OBJECT
public:
    COperateTemplateQtEvent(CPlugin *plugin);
    virtual ~COperateTemplateQtEvent();

    /*!
     * \brief Get parameter
     */
    [[nodiscard]] virtual CParameterTemplateQtEvent* GetParameter() const;

    // COperate interface
public:
    virtual const qint16 Version() const override;
    virtual QWidget *GetViewer() override;
    virtual int Start() override;
    virtual int Stop() override;

Q_SIGNALS:
    /*!
     * \brief New backend siganl
     * \param pOperate
     * \see CPluginThread CPluginBackendThread::CreateOperate CManageBackend
     */
    void sigNewBackend(COperate* pOperate);
    /*!
     * \brief Delete backend signal
     * \param pOperate
     * \see CPluginThread CPluginBackendThread::CreateOperate CManageBackend
     */
    void sigDeleteBackend(COperate* pOperate);

protected:
    virtual int SetPluginParameters(CParameterPlugin *pPara) override;
    Q_INVOKABLE virtual CBackend* InstanceBackend();
    Q_INVOKABLE virtual int Initial() override;
    Q_INVOKABLE virtual int Clean() override;
    virtual int Load(QSettings &set) override;
    virtual int Save(QSettings &set) override;

private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

private:
    CParameterTemplateQtEvent* m_pPara;
    CFrmViewerTemplateQtEvent* m_pViewer;
};
