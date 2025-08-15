// Author: Kang Lin <kl222@126.com>

#pragma once

#include "ParameterTerminalBase.h"
#include "ParameterNet.h"

class CParameterRawStream : public CParameterTerminalBase
{
    Q_OBJECT

public:
    CParameterRawStream(CParameterOperate *parent = nullptr,
                         const QString& szPrefix = QString());

    CParameterNet m_Net;

    enum class TYPE {
        TCP,
        NamePipe
    };
    static const QString GetTypeName(TYPE type);
    TYPE GetType() const;
    void SetType(TYPE type);
private:
    TYPE m_Type;

protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};
