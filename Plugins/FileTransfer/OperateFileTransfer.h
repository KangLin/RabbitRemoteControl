// Author: Kang Lin <kl222@126.com>

#pragma once
#include "Operate.h"

class COperateFileTransfer : public COperate
{
    Q_OBJECT
public:
    explicit COperateFileTransfer(CPlugin *plugin);
    virtual ~COperateFileTransfer();
    
    // COperate interface
public:
    virtual const qint16 Version() const override;
    virtual QWidget *GetViewer() override;
    virtual int Start() override;
    virtual int Stop() override;
    
protected:
    virtual int SetGlobalParameters(CParameterPlugin *pPara) override;
    
private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;
};
