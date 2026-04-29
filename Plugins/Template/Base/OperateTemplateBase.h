// Author: Kang Lin <kl222@126.com>

#pragma once

#include "Operate.h"

class CBackend;
class CBackendThread;
class CFrmViewerTemplateBase;
class CParameterTemplateBase;
class COperateTemplateBase : public COperate
{
    Q_OBJECT
public:
    COperateTemplateBase(CPlugin *plugin);
    virtual ~COperateTemplateBase();

    /*!
     * \brief Get parameter
     */
    [[nodiscard]] virtual CParameterTemplateBase* GetParameter() const;

    // COperate interface
public:
    virtual const qint16 Version() const override;
    virtual QWidget *GetViewer() override;
    virtual int Start() override;
    virtual int Stop() override;

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
    CParameterTemplateBase* m_pPara;
    CFrmViewerTemplateBase* m_pViewer;
    CBackendThread* m_pThread;
};
