// Author: Kang Lin <kl222@126.com>

#pragma once
#include "OperateTerminal.h"

class CTerminal : public COperateTerminal
{
    Q_OBJECT

public:
    explicit CTerminal(CPlugin *parent);
    virtual ~CTerminal() override;

public:
    virtual const QString Name() override;

    [[nodiscard]] virtual CBackend *InstanceBackend() override;

    virtual int Start() override;
    virtual int Stop() override;

protected:
    virtual int Initial() override;
    virtual void SetShotcuts(bool bEnable) override;

private:
   [[nodiscard]] virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

private:
    CParameterTerminalBase m_Parameters;
    QAction* m_pOpenFolderWithExplorer;
    QAction* m_pCopyToClipboard;
};
