//! @author Kang Lin <kl222@126.com>

#include "IceSignal.h"
#include <QDebug>

CIceSignal::CIceSignal(QObject *parent) : QObject(parent),
    m_Log("Channel.ICE")
{
}

CIceSignal::~CIceSignal()
{
    qDebug(m_Log) << "CIceSignal::~CIceSignal()";
}
