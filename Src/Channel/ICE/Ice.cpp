//! @author Kang Lin <kl222@126.com>

#include "Ice.h"
#include "ChannelIce.h"
#ifdef HAVE_QXMPP
#include "IceSignalQxmpp.h"
#endif
#include "FrmParameterICE.h"
#include "RabbitCommonDir.h"

#include <QCoreApplication>
#include <QLocale>
#include <QDebug>

CICE::CICE(QObject *parent)
    : QObject{parent},
      m_Log("Channel.ICE")
{
    bool check = false;
    QString szTranslatorFile = RabbitCommon::CDir::Instance()->GetDirTranslations()
            + "/Channel_" + QLocale::system().name() + ".qm";
    if(!m_Translator.load(szTranslatorFile))
        qCritical(m_Log) << "Open translator file fail:" << szTranslatorFile;
    qApp->installTranslator(&m_Translator);
    
#ifdef HAVE_QXMPP
    m_Signal = QSharedPointer<CIceSignal>(new CIceSignalQxmpp(this));
    if(m_Signal)
    {
        check = connect(m_Signal.data(), SIGNAL(sigConnected()),
                        this, SLOT(slotConnected()));
        Q_ASSERT(check);
        check = connect(m_Signal.data(), SIGNAL(sigDisconnected()),
                        this, SLOT(slotDisconnected()));
        Q_ASSERT(check);
        check = connect(m_Signal.data(), SIGNAL(sigError(int, const QString&)),
                        this, SLOT(slotError(int, const QString&)));
        Q_ASSERT(check);
    }
#endif

#ifdef HAVE_ICE
    check = connect(GetParameter(), SIGNAL(sigIceDebugChanged(bool)),
                    &g_LogCallback, SLOT(slotEnable(bool)));
    Q_ASSERT(check);
    g_LogCallback.slotEnable(GetParameter()->GetIceDebug());
#endif
}

CICE::~CICE()
{
    qApp->removeTranslator(&m_Translator);
}

CICE* CICE::Instance()
{
    static CICE* p = nullptr;
    if(!p) p = new CICE();
    return p;
}

QSharedPointer<CIceSignal> CICE::GetSignal()
{
    return m_Signal;
}

CParameterICE* CICE::GetParameter()
{
    return &m_Parameter;
}

QWidget* CICE::GetParameterWidget(QWidget *parent)
{
    return new CFrmParameterICE(GetParameter(), parent);
}

void CICE::slotIceChanged()
{
    QSharedPointer<CIceSignal> signal = GetSignal();
    if(!signal) return;
    if(signal->IsConnected())
        signal->Close();
    CParameterICE* pPara = GetParameter();
    if(pPara->getIce())
    {
        signal->Open(pPara->getSignalServer(), pPara->getSignalPort(),
                     pPara->getSignalUser(), pPara->getSignalPassword());
    }
}

void CICE::slotStart()
{
    QSharedPointer<CIceSignal> signal = GetSignal();
    if(!signal)
    {
        qCritical(m_Log) << "The signal is null";
        return;
    }
    if(signal->IsConnected())
        return;
    CParameterICE* pPara = GetParameter();
    if(pPara->getIce())
    {
        signal->Open(pPara->getSignalServer(), pPara->getSignalPort(),
                     pPara->getSignalUser(), pPara->getSignalPassword());
    }
}

void CICE::slotStop()
{
    QSharedPointer<CIceSignal> signal = GetSignal();
    if(!signal) return;
    //if(signal->IsConnected())
        signal->Close();
}

void CICE::slotConnected()
{
    CParameterICE* pPara = GetParameter();
    qInfo(m_Log) << "User" << pPara->getSignalUser()
                  << "is connected to signal server:"
                  << pPara->getSignalServer() << ":" << pPara->getSignalPort();
}

void CICE::slotDisconnected()
{
    CParameterICE* pPara = GetParameter();
    qInfo(m_Log) << "User" << pPara->getSignalUser()
                  << "is disconnected to signal server:"
                  << pPara->getSignalServer() << ":" << pPara->getSignalPort();
}

void CICE::slotError(int nError, const QString& szError)
{
    CParameterICE* pPara = GetParameter();
    qInfo(m_Log) << "User" << pPara->getSignalUser()
                  << "signal server["
                  << pPara->getSignalServer() << ":" << pPara->getSignalPort()
                  << "] error:" << nError << szError;
}
