// Author: Kang Lin <kl222@126.com>

#include "Hook.h"

#include <QDebug>

CHook::CHook(QObject *parent)
    : QObject(parent)
{}

CHook::~CHook()
{
    qDebug() << "CHook::~CHook()";
}
