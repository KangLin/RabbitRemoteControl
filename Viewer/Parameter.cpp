// Author: Kang Lin <kl222@126.com>

#include "Parameter.h"

CParameter::CParameter(QObject *parent) : QObject(parent),
    nPort(0),
    bSavePassword(false),
    bOnlyView(false),
    bLocalCursor(true),
    bClipboard(true),
    eProxyType(emProxy::No),
    nProxyPort(1080)
{}

QDataStream &operator<<(QDataStream &data, const CParameter &para)
{
    data << para.szName
         << para.szHost
         << para.nPort
         << para.szUser
         << para.bSavePassword;
    
    if(para.bSavePassword)
        data << para.szPassword;
    
    data << para.bOnlyView
         << para.bLocalCursor
         << para.bClipboard
            
         << (uchar)para.eProxyType
         << para.szProxyHost
         << para.nProxyPort
         << para.szProxyUser
         << para.szProxyPassword
            ;
    return data;
}

QDataStream &operator>>(QDataStream &data, CParameter &para)
{
    data >> para.szName
            >> para.szHost
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
    
    uchar proxyType = 0;
    data >> proxyType;
    para.eProxyType = (CParameter::emProxy) proxyType;
    data >> para.szProxyHost
            >> para.nProxyPort
            >> para.szProxyUser
            >> para.szProxyPassword
            ;
    
    return data;
}
