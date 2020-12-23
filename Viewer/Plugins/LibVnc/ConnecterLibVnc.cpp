#include "ConnecterLibVnc.h"

CConnecterLibVnc::CConnecterLibVnc(QObject *parent) : CConnecter(parent)
{
}

QString CConnecterLibVnc::ServerName()
{
    //TODO: add server name
    return QString();
}

QString CConnecterLibVnc::Name()
{
    return "LibVnc";
}

QString CConnecterLibVnc::Description()
{
    return Protol() + ":";
}

QString CConnecterLibVnc::Protol()
{
    return "RFB";
}

qint16 CConnecterLibVnc::Version()
{
    return 0;
}

QDialog *CConnecterLibVnc::GetDialogSettings(QWidget *parent)
{
    return nullptr;
}

int CConnecterLibVnc::Load(QDataStream &d)
{
    int nRet = 0;
    
    return nRet;
}

int CConnecterLibVnc::Save(QDataStream &d)
{
    int nRet = 0;
    
    return nRet;
}

int CConnecterLibVnc::Connect()
{
    int nRet = 0;
    
    return nRet;
}

int CConnecterLibVnc::DisConnect()
{
    int nRet = 0;
    
    return nRet;
}

void CConnecterLibVnc::slotSetClipboard(QMimeData *data)
{
}
