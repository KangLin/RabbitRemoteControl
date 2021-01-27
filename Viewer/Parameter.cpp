//! @author: Kang Lin (kl222@126.com)

#include "Parameter.h"

CParameter::CParameter(QObject *parent) : QObject(parent),
    nPort(0),
    bSavePassword(false),
    bOnlyView(false),
    bLocalCursor(true),
    bClipboard(true)
{}

QDataStream &operator<<(QDataStream &data, const CParameter &para)
{
    data << para.szHost
         << para.nPort
         << para.szUser
         << para.bSavePassword;
    
    if(para.bSavePassword)
        data << para.szPassword;
    
    data << para.bOnlyView
         << para.bLocalCursor
         << para.bClipboard
            ;
    return data;
}

QDataStream &operator>>(QDataStream &data, CParameter &para)
{
    data >> para.szHost
            >> para.nPort
            >> para.szUser
            >> para.bSavePassword
            ;
    if(para.bSavePassword)
        data >> para.szPassword;
    
    data >> para.bOnlyView
            >> para.bLocalCursor
            >> para.bClipboard
            ;
    return data;
}
