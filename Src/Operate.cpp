#include <QMetaMethod>
#include <QInputDialog>
#include <QCheckBox>
#include <QLoggingCategory>
#include <QApplication>

#include "Operate.h"
#include "Plugin.h"
#include "RabbitCommonTools.h"
#include "RabbitCommonDir.h"

static Q_LOGGING_CATEGORY(log, "Operate")

COperate::COperate(CPlugin *plugin)
    : QObject{plugin}
    , m_pPlugin(plugin)
    , m_pActionSettings(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
    if(QApplication::clipboard())
    {
        bool check = connect(QApplication::clipboard(), SIGNAL(dataChanged()),
                             this, SIGNAL(sigClipBoardChanged()));
        Q_ASSERT(check);
    }
}

COperate::~COperate()
{
    qDebug(log) << Q_FUNC_INFO;
}

const QString COperate::Id()
{
    Q_ASSERT(GetPlugin());
    QString szId = GetPlugin()->Name() + "_" + Protocol()
                   + "_" + QString::number((int)GetPlugin()->Type());
    return szId;
}

const QString COperate::Name()
{
    return GetPlugin()->DisplayName();
}

const QString COperate::Description()
{
    QString szDescription;
    if(!Name().isEmpty())
        szDescription = tr("Name: ") + Name() + "\n";

    if(!GetTypeName().isEmpty())
        szDescription += tr("Type:") + GetTypeName() + "\n";

    if(!Protocol().isEmpty()) {
        szDescription += tr("Protocol: ") + Protocol();
#ifdef DEBUG
        if(!GetPlugin()->DisplayName().isEmpty())
            szDescription += " - " + GetPlugin()->DisplayName();
#endif
        szDescription += "\n";
    }

    if(GetSecurityLevel() != SecurityLevel::No)
        szDescription += tr("Security level: ") + GetSecurityLevelString() + "\n";

    szDescription += tr("Description: ") + GetPlugin()->Description();
    return szDescription;
}

const QString COperate::Protocol() const
{
    return GetPlugin()->Protocol();
}

const QString COperate::GetTypeName() const
{
    Q_ASSERT(GetPlugin());
    return GetPlugin()->TypeName(GetPlugin()->Type());
}

const QIcon COperate::Icon() const
{
    return GetPlugin()->Icon();
}

int COperate::OpenDialogSettings(QWidget *parent)
{
    int nRet = QDialog::Accepted;
    QDialog* p = OnOpenDialogSettings(parent);
    if(p)
    {
        // The dialog is closed when the connect is close.
        bool check = connect(this, SIGNAL(sigFinished()),
                             p, SLOT(reject()));
        Q_ASSERT(check);
        p->setWindowIcon(this->Icon());
        p->setWindowTitle(tr("Set ") + GetPlugin()->DisplayName());
        p->setAttribute(Qt::WA_DeleteOnClose);
        nRet = RC_SHOW_WINDOW(p);
    } else {
        qCritical(log) << "The Protocol [" << Protocol() << "] don't settings dialog";
    }
    return nRet;
}

QMenu* COperate::GetMenu(QWidget* parent)
{
    if(m_Menu.actions().isEmpty())
        return nullptr;
    return &m_Menu;
}

CStats *COperate::GetStats()
{
    return nullptr;
}

COperate::SecurityLevel COperate::GetSecurityLevel()
{
    return SecurityLevel::No;
}

QString COperate::GetSecurityLevelString()
{
    return GetSecurityLevelString(GetSecurityLevel());
}

QString COperate::GetSecurityLevelString(SecurityLevel level)
{
    switch (level) {
    case SecurityLevel::No:
        return tr("No");
    case SecurityLevel::Secure:
        return tr("Secure");
    case SecurityLevel::NonSecureAuthentication:
        return tr("Non-secure authentication over a secure channel");
    case SecurityLevel::Normal:
        return tr("Channel is secure.");
    case SecurityLevel::SecureAuthentication:
        return tr("There is security verification, not a secure channel.");
    case SecurityLevel::Risky:
        return tr("Risky");
    default:
        break;
    }
    return QString();
}

QColor COperate::GetSecurityLevelColor()
{
    return GetSecurityLevelColor(GetSecurityLevel());
}

QColor COperate::GetSecurityLevelColor(SecurityLevel level)
{
    switch (level) {
    case SecurityLevel::Secure:
        return Qt::GlobalColor::green;
    case SecurityLevel::NonSecureAuthentication:
        return Qt::GlobalColor::blue;
    case SecurityLevel::Normal:
        return Qt::GlobalColor::yellow;
    case SecurityLevel::SecureAuthentication:
        return Qt::GlobalColor::darkRed;
    case SecurityLevel::Risky:
        return Qt::GlobalColor::red;
    default:
        break;
    }
    return QColor();
}

QString COperate::GetSettingsFile()
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

int COperate::SetSettingsFile(const QString &szFile)
{
    m_szSettings = szFile;
    return 0;
}

int COperate::Load(QString szFile)
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

int COperate::Save(QString szFile)
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

int COperate::Load(QSettings &set)
{
    int nRet = 0;
    Q_UNUSED(set);
    return nRet;
}

int COperate::Save(QSettings &set)
{
    int nRet = 0;
    Q_UNUSED(set);
    return nRet;
}

int COperate::Initial()
{
    m_Menu.setIcon(m_pPlugin->Icon());
    m_Menu.setTitle(m_pPlugin->DisplayName());
    m_Menu.setToolTip(m_pPlugin->DisplayName());
    m_Menu.setStatusTip(m_pPlugin->DisplayName());
    
    m_pActionSettings = new QAction(QIcon::fromTheme("system-settings"),
                              tr("Settings"), &m_Menu);
    if(m_pActionSettings) {
        bool check = connect(m_pActionSettings, SIGNAL(triggered()),
                             this, SLOT(slotSettings()));
        Q_ASSERT(check);
    }
    return 0;
}

int COperate::Clean()
{
    return 0;
}

void COperate::slotSettings()
{
    int nRet = OpenDialogSettings();
    if(QDialog::Accepted == nRet)
        emit sigUpdateParameters(this);
}

CPlugin* COperate::GetPlugin() const
{
    return m_pPlugin;
}

int COperate::SetGlobalParameters(CParameterPlugin *pPara)
{
    auto pPlugin = pPara;
    if(pPlugin)
    {
        bool check = connect(pPlugin, SIGNAL(sigShowProtocolPrefixChanged()),
                             this, SLOT(slotUpdateName()));
        Q_ASSERT(check);
        check = connect(pPlugin, SIGNAL(sigSHowIpPortInNameChanged()),
                        this, SLOT(slotUpdateName()));
        Q_ASSERT(check);
    }
    return 0;
}

void COperate::slotUpdateName()
{
    emit sigUpdateName(Name());
}

QObject* COperate::createObject(const QString& className, QObject* parent)
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

void COperate::slotBlockShowWidget(const QString& className, int &nRet, void* pContext)
{
    bool check = false;
    QObject *obj = createObject(className);
    Q_ASSERT_X(obj, "Connecter", QString("Create object failed: " + className).toStdString().c_str());
    /*
    const QMetaObject* metaObject = obj->metaObject();
    QStringList methods;
    for(int i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i)
        methods << QString::fromLatin1(metaObject->method(i).methodSignature());
    qDebug(log) << methods;
    //*/
    if(-1 == obj->metaObject()->indexOfMethod("SetContext(void*)"))
    {
        QString szErr;
        szErr = "The class " + className + " is not method SetContext. It must be SetContext(void*) method.";
        qCritical(log) << szErr;
        Q_ASSERT_X(false, "Connecter", szErr.toStdString().c_str());
    }
    obj->metaObject()->invokeMethod(obj, "SetContext", Q_ARG(void*, pContext));

    if(obj->inherits("QDialog"))
    {
        QDialog* pDlg = qobject_cast<QDialog*>(obj);
        pDlg->setAttribute(Qt::WA_DeleteOnClose);
        check = connect(this, SIGNAL(sigFinished()),
                        pDlg, SLOT(reject()));
        Q_ASSERT(check);
        nRet = RC_SHOW_WINDOW(pDlg);
    } else if(obj->inherits("QWidget")) {
        QWidget* pWdg = qobject_cast<QWidget*>(obj);
        pWdg->setAttribute(Qt::WA_DeleteOnClose);
        check = connect(this, SIGNAL(sigFinished()),
                        pWdg, SLOT(close()));
        Q_ASSERT(check);
        nRet = RC_SHOW_WINDOW(pWdg);
    }
}

void COperate::slotBlockShowMessageBox(const QString &szTitle, const QString &szMessage,
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

void COperate::slotBlockInputDialog(const QString &szTitle, const QString &szLable,
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

void COperate::slotSetClipboard(QMimeData* data)
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
