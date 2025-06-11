// Author: Kang Lin <kl222@126.com>

#include <QClipboard>
#include <QApplication>
#include <QGenericArgument>
#include <QCheckBox>
#include <QLoggingCategory>
#include <QInputDialog>
#include <QMetaMethod>

#include "Connecter.h"
#include "PluginClient.h"
#include "RabbitCommonTools.h"
#include "RabbitCommonDir.h"

static Q_LOGGING_CATEGORY(log, "Client.Connecter")
    
CConnecter::CConnecter(CPluginClient *plugin) : QObject(plugin),
    m_pPluginClient(plugin),
    m_pSettings(nullptr),
    m_pParameter(nullptr)
{
    bool check = false;
    
    if(QApplication::clipboard())
    {
        check = connect(QApplication::clipboard(), SIGNAL(dataChanged()),
                        this, SIGNAL(sigClipBoardChanged()));
        Q_ASSERT(check);
    }
}

CConnecter::~CConnecter()
{
    qDebug(log) << "CConnecter::~CConnecter";
}

const QString CConnecter::Id()
{
    QString szId = Protocol() + "_" + GetPlugClient()->Name();
    return szId;
}

const QString CConnecter::Name()
{
    return GetPlugClient()->DisplayName();
}

const QString CConnecter::Description()
{
    return tr("Name: ") + Name() + "\n"
           + tr("Protocol: ") + Protocol()
#ifdef DEBUG
           + " - " + GetPlugClient()->DisplayName()
#endif
           + "\n"
           + tr("Description: ") + GetPlugClient()->Description();
}

const QString CConnecter::Protocol() const
{
    return GetPlugClient()->Protocol();
}

const QIcon CConnecter::Icon() const
{
    return GetPlugClient()->Icon();
}

void CConnecter::slotSetClipboard(QMimeData* data)
{    
    QClipboard* pClipboard = QApplication::clipboard();
    if(pClipboard) {
        //        pClipboard->disconnect(this);
        
        pClipboard->setMimeData(data);
        
        //        bool check = connect(pClipboard, SIGNAL(dataChanged()),
        //                             this, SIGNAL(sigClipBoardChanged()));
        //        Q_ASSERT(check);
    }
}

int CConnecter::OpenDialogSettings(QWidget *parent)
{
    int nRet = QDialog::Accepted;
    QDialog* p = OnOpenDialogSettings(parent);
    if(p)
    {
        // The dialog is closed when the connect is close.
        bool check = connect(this, SIGNAL(sigDisconnected()),
                             p, SLOT(reject()));
        Q_ASSERT(check);
        p->setWindowIcon(this->Icon());
        p->setWindowTitle(tr("Set ") + GetPlugClient()->DisplayName());
        p->setAttribute(Qt::WA_DeleteOnClose);
        nRet = RC_SHOW_WINDOW(p);
    } else {
        qCritical(log) << "The Protocol [" << Protocol() << "] don't settings dialog";
    }
    return nRet;
}

QMenu* CConnecter::GetMenu(QWidget* parent)
{
    if(m_Menu.actions().isEmpty())
        return nullptr;
    return &m_Menu;
}

QString CConnecter::GetSettingsFile()
{
    if(m_szSettings.isEmpty())
    {
        m_szSettings = RabbitCommon::CDir::Instance()->GetDirUserData()
            + QDir::separator()
            + Id()
            + ".rrc";
    }
    return m_szSettings;
}

int CConnecter::SetSettingsFile(const QString &szFile)
{
    m_szSettings = szFile;
    return 0;
}

int CConnecter::Load(QString szFile)
{
    Q_ASSERT(!szFile.isEmpty());
    if(szFile.isEmpty())
    {
        qCritical(log) << "The load file is empty";
        return -1;
    }
    QSettings set(szFile, QSettings::IniFormat);
    return Load(set);
}

int CConnecter::Save(QString szFile)
{
    Q_ASSERT(!szFile.isEmpty());
    if(szFile.isEmpty())
    {
        qCritical(log) << "The load file is empty";
        return -1;
    }    
    QSettings set(szFile, QSettings::IniFormat);
    return Save(set);
}

int CConnecter::Load(QSettings &set)
{
    int nRet = 0;
    Q_ASSERT(m_pParameter);
    if(m_pParameter)
        nRet = m_pParameter->Load(set);
    return nRet;
}

int CConnecter::Save(QSettings &set)
{
    int nRet = 0;
    Q_ASSERT(m_pParameter);
    if(m_pParameter) {
        m_pParameter->Save(set);
    }
    return nRet;
}

int CConnecter::Initial()
{
    m_Menu.setIcon(m_pPluginClient->Icon());
    m_Menu.setTitle(m_pPluginClient->DisplayName());
    m_Menu.setToolTip(m_pPluginClient->DisplayName());
    m_Menu.setStatusTip(m_pPluginClient->DisplayName());

    m_pSettings = new QAction(QIcon::fromTheme("system-settings"),
                              tr("Settings"), &m_Menu);
    if(m_pSettings) {
        bool check = connect(m_pSettings, SIGNAL(triggered()),
                             this, SLOT(slotSettings()));
        Q_ASSERT(check);
    }
    return 0;
}

int CConnecter::Clean()
{
    return 0;
}

int CConnecter::SetParameterClient(CParameterClient* pPara)
{
    if(!GetParameter())
    {
        QString szMsg = "The CConnecter is not parameters! "
                        "please first create parameters, "
                        "then call SetParameter in the ";
        szMsg += metaObject()->className() + QString("::")
                 + metaObject()->className();
        szMsg += QString(" or ") + metaObject()->className()
                 + QString("::") + "Initial()";
        szMsg += " to set the parameters pointer. "
                 "Default set CParameterClient for the parameters of connecter "
                 "(CParameterConnecter or its derived classes) "
                 "See: CClient::CreateConnecter. "
                 "If you are sure the parameter of connecter "
                 "does not need CParameterClient. "
                 "Please overload the SetParameterClient in the ";
        szMsg += QString(metaObject()->className()) + " . don't set it";
        qCritical(log) << szMsg.toStdString().c_str();
        Q_ASSERT(false);
        return -1;
    }
    return 0;
}

int CConnecter::SetParameter(CParameter *p)
{
    if(m_pParameter)
        m_pParameter->disconnect(this);

    m_pParameter = p;

    return 0;
}

CParameter* CConnecter::GetParameter()
{
    return m_pParameter;
}

CPluginClient* CConnecter::GetPlugClient() const
{
    return m_pPluginClient;
}

void CConnecter::slotShowServerName()
{
    emit sigUpdateName(Name());
}

void CConnecter::slotUpdateName()
{
    emit sigUpdateName(Name());
}

QObject* CConnecter::createObject(const QString& className, QObject* parent)
{
    Q_UNUSED(parent);
    int type =
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QMetaType::fromName(className.toStdString().c_str()).id();
#else
        QMetaType::type(className.toStdString().c_str());
#endif
    if(QMetaType::UnknownType == type)
    {
        qCritical(log) << className << " is QMetaType::UnknownType";
        return nullptr;
    }
    QObject *obj =
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        (QObject*)QMetaType(type).create();
#else
        (QObject*)QMetaType::create(type);
#endif
    if(nullptr == obj)
    {
        qCritical(log) << "QMetaType::create fail: " << type;
        return nullptr;
    }
    //const QMetaObject* metaObj = QMetaType::metaObjectForType(type);
    //QObject *obj = metaObj->newInstance(Q_ARG(QObject*, parent));
    return obj;
}

void CConnecter::slotBlockShowWidget(const QString& className, int &nRet, void* pContext)
{
    bool check = false;
    QObject *obj = createObject(className);
    Q_ASSERT_X(obj, "Connecter", QString("Create object failed: " + className).toStdString().c_str());
    /*
    const QMetaObject* metaObject = obj->metaObject();
    QStringList methods;
    for(int i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i)
        methods << QString::fromLatin1(metaObject->method(i).methodSignature());
    qCritical(log) << methods;
    //*/
    if(-1 == obj->metaObject()->indexOfMethod("SetContext(void*)"))
    {
        QString szErr;
        szErr = "The class " + className + " is not method SetContext. It must be SetContext(void*) method.";
        qCritical(log) << szErr;
        Q_ASSERT_X(false, "Connecter", szErr.toStdString().c_str());
    }
    obj->metaObject()->invokeMethod(obj, "SetContext", Q_ARG(void*, pContext));
    if(-1 < obj->metaObject()->indexOfMethod("SetConnecter(CConnecter*)"))
    {
        obj->metaObject()->invokeMethod(obj, "SetConnecter", Q_ARG(CConnecter*, this));
    }

    if(obj->inherits("QDialog"))
    {
        QDialog* pDlg = qobject_cast<QDialog*>(obj);
        pDlg->setAttribute(Qt::WA_DeleteOnClose);
        check = connect(this, SIGNAL(sigDisconnected()),
                        pDlg, SLOT(reject()));
        Q_ASSERT(check);
        nRet = RC_SHOW_WINDOW(pDlg);
    } else if(obj->inherits("QWidget")) {
        QWidget* pWdg = qobject_cast<QWidget*>(obj);
        pWdg->setAttribute(Qt::WA_DeleteOnClose);
        check = connect(this, SIGNAL(sigDisconnected()),
                        pWdg, SLOT(close()));
        Q_ASSERT(check);
        nRet = RC_SHOW_WINDOW(pWdg);
    }
}

void CConnecter::slotBlockShowMessageBox(const QString &szTitle, const QString &szMessage,
                                         QMessageBox::StandardButtons buttons,
                                         QMessageBox::StandardButton &nRet,
                                         bool &checkBox,
                                         QString szCheckBoxContext)
{
    QCheckBox* pBox = nullptr;
    QMessageBox msg(QMessageBox::Information,
                    szTitle, szMessage, buttons, GetViewer());
    if(!szCheckBoxContext.isEmpty())
    {
        pBox = new QCheckBox(szCheckBoxContext);
        if(pBox)
            pBox->setCheckable(true);
        msg.setCheckBox(pBox);
    }
    nRet = (QMessageBox::StandardButton)RC_SHOW_WINDOW(&msg);
    if(pBox)
        checkBox = pBox->isChecked();
}

void CConnecter::slotBlockInputDialog(const QString &szTitle, const QString &szLable,
                                      const QString &szMessage,
                                      QString& szText)
{
    bool ok = false;
    QString t = QInputDialog::getText(nullptr,
                                      szTitle,
                                      szLable,
                                      QLineEdit::Normal,
                                      szMessage,
                                      &ok);
    if(ok && !t.isEmpty())
        szText = t;
}

void CConnecter::slotSettings()
{
    OpenDialogSettings();
}
