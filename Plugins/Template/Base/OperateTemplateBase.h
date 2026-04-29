// Author: Kang Lin <kl222@126.com>

#pragma once

#include "Operate.h"

class CParameterTemplateBase;
class COperateTemplateBase : public COperate
{
    Q_OBJECT
public:
    COperateTemplateBase(CPlugin *plugin);
    virtual ~COperateTemplateBase();

    // COperate interface
public:
    virtual const qint16 Version() const override;
    virtual QWidget *GetViewer() override;
    virtual int Start() override;
    virtual int Stop() override;

protected:
    virtual int SetPluginParameters(CParameterPlugin *pPara) override;

private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

protected:
    virtual int Load(QSettings &set) override;
    virtual int Save(QSettings &set) override;

private:
    CParameterTemplateBase* m_pPara;
};
