//! @author: Kang Lin (kl222@126.com)

#include "Parameter.h"

CParameter::CParameter(QObject *parent) : QObject(parent),
    bSavePassword(false),
    bOnlyView(false),
    bLocalCursor(true),
    bClipboard(true)
{}
