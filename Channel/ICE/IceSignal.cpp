//! @author Kang Lin(kl222@126.com)

#include "IceSignal.h"
#include <QDebug>

CIceSignal::CIceSignal(QObject *parent) : QObject(parent)
{
}

CIceSignal::~CIceSignal()
{
    qDebug() << "CIceSignal::~CIceSignal()";
}
