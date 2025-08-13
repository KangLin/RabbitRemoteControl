// Author: Kang Lin <kl222@126.com>

#pragma once
#include "OperateTerminal.h"
#include "TerminalParameter.h"

class CTerminal : public COperateTerminal
{
    Q_OBJECT

public:
    explicit CTerminal(CPlugin *parent);
    virtual ~CTerminal() override;

public:
    [[nodiscard]] virtual const QString Id() override;
    [[nodiscard]] virtual const QString Name() override;
    [[nodiscard]] virtual const QString Description() override;

    [[nodiscard]] virtual CBackend *InstanceBackend() override;

    virtual int Start() override;
    virtual int Stop() override;

protected:
    virtual int Initial() override;
    virtual void SetShotcuts(bool bEnable) override;

private:
   [[nodiscard]] virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;
private Q_SLOTS:
   void slotTerminalTitleChanged();
private:
    CTerminalParameter m_Parameters;
    QAction* m_pOpenFolderWithExplorer;
    QAction* m_pCopyToClipboard;

    // COperateTerminal interface
public:
    virtual int SetParameter(CParameterTerminalBase *pPara) override;
};
