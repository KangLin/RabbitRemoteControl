// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "ParameterRawStream.h"
#include "ParameterPlugin.h"

static Q_LOGGING_CATEGORY(log, "RawStream.Parameter")

CParameterRawStream::CParameterRawStream(
    CParameterOperate *parent, const QString &szPrefix)
    : CParameterTerminalBase{parent, szPrefix}
    , m_Net(this)
    , m_Type(TYPE::TCP)
{
    m_Net.m_User.SetType(QList<CParameterUser::TYPE>()
                         << CParameterUser::TYPE::None
                         << CParameterUser::TYPE::UserPassword);
}

int CParameterRawStream::OnLoad(QSettings &set)
{
    set.beginGroup("RawStream");
    SetType((TYPE)set.value("Type", (int)GetType()).toInt());
    set.endGroup();
    return CParameterTerminalBase::OnLoad(set);
}

int CParameterRawStream::OnSave(QSettings &set)
{
    set.beginGroup("RawStream");
    set.setValue("Type", (int)GetType());
    set.endGroup();
    return CParameterTerminalBase::OnSave(set);
}

const QString CParameterRawStream::GetTypeName(TYPE type)
{
    QString szType;
    switch((TYPE)type) {
    case TYPE::TCP:
        szType = "TCP";
        break;
    case TYPE::NamePipe:
        szType = tr("Name pipe");
        break;
    }
    return szType;
}

CParameterRawStream::TYPE CParameterRawStream::GetType() const
{
    return m_Type;
}

void CParameterRawStream::SetType(TYPE type)
{
    if(m_Type == type)
        return;
    m_Type = type;
    SetModified(true);
}
