#include "ConnectLibVnc.h"
#include "RabbitCommonLog.h"
#include <QDebug>

char* gThis = "This pointer";

CConnectLibVnc::CConnectLibVnc(CConnecterLibVnc *pConnecter, QObject *parent)
    : CConnect(pConnecter, parent),
      m_pClient(nullptr)
{
    
}

CConnectLibVnc::~CConnectLibVnc()
{
    qDebug() << "CConnectLibVnc::~CConnectLibVnc()";
}

int CConnectLibVnc::Connect()
{
    int nRet = 0;
    
    return nRet;
}

int CConnectLibVnc::Disconnect()
{
    int nRet = 0;
    
    return nRet;
}

int CConnectLibVnc::Process()
{
    int nRet = 0;
    nRet = WaitForMessage(m_pClient, 500); /* useful for timeout to be no more than 10 msec per second (=10000/framerate usec) */
    if (nRet < 0) return nRet;
    
    if(!HandleRFBServerMessage(m_pClient))
        return -1;
    
    return nRet;
}

void CConnectLibVnc::slotClipBoardChange()
{
}

int CConnectLibVnc::Initialize()
{
    int nRet = 0;

    if(m_pClient) Q_ASSERT(!m_pClient);

    m_pClient = rfbGetClient(8,3,4);
    if(!m_pClient)
    {
        LOG_MODEL_ERROR("LibVnc", "rfbGetClient fail");
        return -1;
    }
    m_pClient->MallocFrameBuffer = cb_resize;
    m_pClient->canHandleNewFBSize = true;
    m_pClient->GotFrameBufferUpdate = cb_update;
    m_pClient->HandleKeyboardLedState = cb_kbd_leds;
    m_pClient->HandleTextChat = cb_text_chat;
    m_pClient->GotXCutText = cb_got_selection;
    m_pClient->GetCredential = cb_get_credential;

    rfbClientSetClientData(m_pClient, gThis, this);
    
    if(!rfbInitConnection(m_pClient)) {
        return -2;
     }
    return nRet;
}

int CConnectLibVnc::Clean()
{
    if(m_pClient)
    {
        rfbClientCleanup(m_pClient);
        m_pClient = nullptr;
    }
    return 0;
}

rfbBool CConnectLibVnc::cb_resize(rfbClient* client)
{
    LOG_MODEL_DEBUG("LibVnc", "CConnectLibVnc::cb_resize");
    return TRUE;
}

void CConnectLibVnc::cb_update(rfbClient *cl, int x, int y, int w, int h)
{
    LOG_MODEL_DEBUG("LibVnc", "CConnectLibVnc::cb_update");
    CConnectLibVnc* pThis = (CConnectLibVnc*)rfbClientGetClientData(cl, gThis);
}

void CConnectLibVnc::cb_got_selection(rfbClient *cl, const char *text, int len)
{
    LOG_MODEL_DEBUG("LibVnc", "CConnectLibVnc::cb_got_selection");
}

void CConnectLibVnc::cb_kbd_leds(rfbClient *cl, int value, int pad)
{
    LOG_MODEL_DEBUG("LibVnc", "CConnectLibVnc::cb_kbd_leds");
}

void CConnectLibVnc::cb_text_chat(rfbClient *cl, int value, char *text)
{
    LOG_MODEL_DEBUG("LibVnc", "CConnectLibVnc::cb_text_chat");
}

rfbCredential* CConnectLibVnc::cb_get_credential(rfbClient *cl, int credentialType)
{
    LOG_MODEL_DEBUG("LibVnc", "CConnectLibVnc::cb_get_credential");
    CConnectLibVnc* pThis = (CConnectLibVnc*)rfbClientGetClientData(cl, gThis);
    rfbCredential *c = (rfbCredential*)malloc(sizeof(rfbCredential));
    c->userCredential.username = (char*)malloc(RFB_BUF_SIZE);
    memset(c->userCredential.username, 0, RFB_BUF_SIZE);
    c->userCredential.password = (char*)malloc(RFB_BUF_SIZE);
    memset(c->userCredential.password, 0, RFB_BUF_SIZE);
    
    if(credentialType != rfbCredentialTypeUser) {
        LOG_MODEL_ERROR("LibVnc", "something else than username and password required for authentication\n");
        return NULL;
    }
    
    LOG_MODEL_INFO("LibVnc", "username and password required for authentication!\n");
    
    printf("user: ");
    memcpy(c->userCredential.username,
           pThis->m_szUser.toStdString().c_str(),
           pThis->m_szUser.toStdString().length());
    printf("pass: ");
    memcpy(c->userCredential.password,
           pThis->m_szPassword.toStdString().c_str(),
           pThis->m_szPassword.toStdString().length());
    
    return c;
}
