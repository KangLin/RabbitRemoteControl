// Author: Kang Lin <kl222@126.com>

#pragma once
#include "plugin_export.h"
#include "ParameterOperate.h"
#include "ParameterTerminal.h"
#include "ParameterSSH.h"

/*!
 * \~chinese
 * \brief 终端基本参数
 * \~english
 * \brief Terminal base parameter
 * \~
 * \ingroup gOperateTerminal
 */
class PLUGIN_EXPORT CParameterTerminalBase : public CParameterOperate
{
    Q_OBJECT
public:
    explicit CParameterTerminalBase(CParameterOperate *parent = nullptr,
                                    const QString& szPrefix = QString());

    CParameterTerminal m_Terminal;

public:
    [[nodiscard]] const QString GetName() const;
    void SetName(const QString& szName);
private:
    QString m_szName;
    
public:
    [[nodiscard]] QStringList GetCommands() const;
    void SetCommands(const QStringList& cmd);
private:
    QStringList m_lstCommands;

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;

    // CParameterOperate interface
protected slots:
    virtual void slotSetGlobalParameters() override;
};
