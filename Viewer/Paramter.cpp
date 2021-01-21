//! @author: Kang Lin(kl222@126.com)

#include "Paramter.h"

CParamter::CParamter(QObject *parent) : QObject(parent),
    bSavePassword(false),
    bOnlyView(false),
    bLocalCursor(true),
    bClipboard(true)
{}
