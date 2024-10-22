#include <QLoggingCategory>

#include "ConnecterConnect.h"
#include "PluginClientThread.h"

static Q_LOGGING_CATEGORY(log, "Client.Connecter.Connect")
CConnecterConnect::CConnecterConnect(CPluginClient *plugin)
    : CConnecter(plugin)
    , m_pConnect(nullptr)
{
    qDebug(log) << "CConnecterConnect::CConnecterConnect";
    m_Menu.setIcon(plugin->Icon());
    m_Menu.setTitle(plugin->DisplayName());
    m_Menu.setToolTip(plugin->DisplayName());
    m_Menu.setStatusTip(plugin->DisplayName());
    m_Menu.addAction(QIcon::fromTheme("camera-photo"), tr("ScreenShot"),
                     this, SIGNAL(sigSceenShot()));
#if HAVE_QT6_RECORD
    QAction* pRecord = m_Menu.addAction(
        QIcon::fromTheme("media-record"), tr("Record"),
        this, [&](){
            QAction* pRecord = qobject_cast<QAction*>(sender());
            if(pRecord)
            {
                bool checked = pRecord->isChecked();
                if(checked) {
                    pRecord->setIcon(QIcon::fromTheme("media-playback-stop"));
                    pRecord->setText(tr("Stop record"));
                }
                else {
                    pRecord->setIcon(QIcon::fromTheme("media-playback-start"));
                    pRecord->setText(tr("Start record"));
                }
                emit sigRecord(checked);
            }
        });
    pRecord->setCheckable(true);
#endif
}

CConnecterConnect::~CConnecterConnect()
{
    qDebug(log) << "CConnecterConnect::~CConnecterConnect()";
}

CParameterBase *CConnecterConnect::GetParameter()
{
    return CConnecter::GetParameter();
}

int CConnecterConnect::Connect()
{
    qDebug(log) << "CConnecterConnect::Connect";
    CPluginClientThread* pPlug
        = qobject_cast<CPluginClientThread*>(GetPlugClient());
    if(pPlug)
        emit sigOpenConnect(this);
    else {
        m_pConnect = InstanceConnect();
        if(m_pConnect) {
            int nRet = m_pConnect->Connect();
            emit sigConnected();
            return nRet;
        }
    }
    return 0;
}

int CConnecterConnect::DisConnect()
{
    qDebug(log) << "CConnecterConnect::DisConnect";
    CPluginClientThread* pPlug
        = qobject_cast<CPluginClientThread*>(GetPlugClient());
    if(pPlug)
        emit sigCloseconnect(this);
    else {
        if(m_pConnect) {
            int nRet = m_pConnect->Disconnect();
            m_pConnect->deleteLater();
            emit sigDisconnected();
            return nRet;
        }
    }
    return 0;
}

QMenu *CConnecterConnect::GetMenu(QWidget *parent)
{
    if(m_Menu.actions().isEmpty())
        return nullptr;
    return &m_Menu;
}
