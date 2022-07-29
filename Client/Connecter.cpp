// Author: Kang Lin <kl222@126.com>

#include "Connecter.h"
#include <QClipboard>
#include <QApplication>
#include <QDebug>
#include <QGenericArgument>
#include <QRegularExpression>
#include <QCheckBox>

#include "PluginClient.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"
#ifdef BUILD_QUIWidget
    #include "QUIWidget/QUIWidget.h"
#endif
#include "ParameterConnecter.h"

CConnecter::CConnecter(CPluginClient *parent) : QObject(parent),
    m_pPluginClient(parent),
    m_pParameter(nullptr)
{
    /* 因为断连会从用户调用或者远程端断开连接或出错
     * 从用户端调用会直接调用 DisConnect
     * 从远程端断开连接或出错，则会触发 sigDisconnected 而不会调用 DisConnect，所以需要在此关联
     * 这里隐式是第一个调用，否则对象先释放，则从远程端断连则不会执行。
     */
    bool check = connect(this, SIGNAL(sigDisconnected()),
                         this, SLOT(DisConnect()));
    Q_ASSERT(check);
}

CConnecter::~CConnecter()
{
    LOG_MODEL_DEBUG("CConnecter", "CConnecter::~CConnecter");
}

const QString CConnecter::Id()
{
    QString szId = Protol() + "_" + m_pPluginClient->Name();
    if(GetParameter())
    {
        if(GetParameter()->GetHost().isEmpty())
        {
            if(!GetParameter()->GetName().isEmpty())
                szId += "_" + GetParameter()->GetName();
        } else {
            szId += "_" + GetParameter()->GetHost()
                    + "_" + QString::number(GetParameter()->GetPort());
        }
    }
    szId = szId.replace(QRegularExpression("[@:/#%!^&*\\.]"), "_");
    return szId;
}

/*!
 * \~chinese
 * \brief 显示顺序：
 *        - 用户参数设置的名称
 *        - 如果允许，远程服务名。
 *        - 远程地址
 *
 * \~english
 *  Display order:
 *  - User paramter Name()
 *  - if enable, Server name
 *  - Host and port
 *  
 * \~
 * \see ServerName()
 */
const QString CConnecter::Name()
{
    if(GetParameter() && !GetParameter()->GetName().isEmpty())
        return GetParameter()->GetName();

    return ServerName();
}

const QString CConnecter::Description()
{
    return tr("Name:") + Name() + " "
            + tr("Protol:") + Protol() + " "
            + tr("Server name:") + ServerName() + " "
            + m_pPluginClient->Description();
}

const QString CConnecter::Protol() const
{
    return m_pPluginClient->Protol();
}

const QIcon CConnecter::Icon() const
{
    return m_pPluginClient->Icon();
}

void CConnecter::slotSetClipboard(QMimeData* data)
{
    QClipboard* pClipboard = QApplication::clipboard();
    if(pClipboard)
        pClipboard->setMimeData(data);
}

void CConnecter::slotSetServerName(const QString& szName)
{
    m_szServerName = szName;
    emit sigUpdateName(Name());
}

QString CConnecter::ServerName()
{
    if(m_szServerName.isEmpty() && GetParameter())
        return GetParameter()->GetServerName();
    return m_szServerName;
}

int CConnecter::OpenDialogSettings(QWidget *parent)
{
    int nRet = -1;
    QDialog* p = GetDialogSettings(parent);
    if(p)
    {
        // The dialog is closed when the connect is close.
        bool check = connect(this, SIGNAL(sigDisconnected()),
                             p, SLOT(reject()));
        Q_ASSERT(check);
        p->setWindowIcon(this->Icon());
        p->setWindowTitle(tr("Set ") + m_pPluginClient->DisplayName());
#ifdef BUILD_QUIWidget
        QUIWidget* quiwidget = new QUIWidget();
        quiwidget->setMainWidget(p);
        check = connect(p, SIGNAL(accepted()), quiwidget, SLOT(accept()));
        Q_ASSERT(check);
        check = connect(p, SIGNAL(rejected()), quiwidget, SLOT(reject()));
        Q_ASSERT(check);
        p = quiwidget;
#endif
        
        p->setAttribute(Qt::WA_DeleteOnClose);
        nRet = p->exec();
    } else {
        LOG_MODEL_ERROR("CConnecter",  "The protol[%s] don't settings dialog", Protol().toStdString().c_str());
    }
    return nRet;
}

int CConnecter::Load(QString szFile)
{
    Q_ASSERT(!szFile.isEmpty());
    if(szFile.isEmpty())
    {
        LOG_MODEL_ERROR("CConnecter", "The load file is empty");
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
        LOG_MODEL_ERROR("CConnecter", "The load file is empty");
        return -1;
    }    
    QSettings set(szFile, QSettings::IniFormat);
    return Save(set);
}

CParameterConnecter* CConnecter::GetParameter()
{
    return m_pParameter;
}

int CConnecter::SetParameterViewer(CParameterViewer* pPara)
{
    if(GetParameter())
    {
        GetParameter()->m_pParameterViewe = pPara;
        if(pPara)
            GetParameter()->SetSavePassword(pPara->GetSavePassword());
        return 0;
    } else {
        LOG_MODEL_ERROR("CConnecter",
                "If the parameters(CParameterConnecter or its derived classes) "
                "requires a CParameterViewer. "
                "Please instantiate the parameters "
                "and call SetParameter in the %s::%s to set the parameters pointer. "
                "If you are sure the parameter does not need CParameterViewer. "
                "please overload the SetParameterViewer in the %s. don't set it",
                metaObject()->className(),
                metaObject()->className(),
                metaObject()->className());
        Q_ASSERT(false);
    }
    return -1;
}

int CConnecter::SetParameter(CParameterConnecter *p)
{
    if(GetParameter())
        GetParameter()->disconnect(this);

    m_pParameter = p;

    if(GetParameter())
    {
        bool check = false;
        check = connect(GetParameter(), SIGNAL(sigNameChanged()),
                        this, SLOT(slotUpdateName()));
        Q_ASSERT(check);
        check = connect(GetParameter(), SIGNAL(sigShowServerNameChanged()),
                        this, SLOT(slotShowServerName()));
        Q_ASSERT(check);   
    }
    return 0;
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
    int type = QMetaType::type(className.toStdString().c_str());
    if(QMetaType::UnknownType == type)
    {
        qCritical() << className << " is QMetaType::UnknownType";
        return nullptr;
    }
    QObject *obj = (QObject*)QMetaType::create(type);
    if(nullptr == obj)
    {
        qCritical() << "QMetaType::create fail: " << type;
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
    Q_ASSERT(obj);
    if(!obj) return;
    
    obj->metaObject()->invokeMethod(obj, "SetContext", Q_ARG(void*, pContext));
    obj->metaObject()->invokeMethod(obj, "SetConnecter", Q_ARG(CConnecter*, this));
    if(obj->inherits("QDialog"))
    {
        QDialog* pDlg = qobject_cast<QDialog*>(obj);
        pDlg->setAttribute(Qt::WA_DeleteOnClose);
        check = connect(this, SIGNAL(sigDisconnected()),
                        pDlg, SLOT(reject()));
        Q_ASSERT(check);
        nRet = pDlg->exec();
    } else if(obj->inherits("QWidget")) {
        QWidget* pWdg = qobject_cast<QWidget*>(obj);
        pWdg->setAttribute(Qt::WA_DeleteOnClose);
        check = connect(this, SIGNAL(sigDisconnected()),
                        pWdg, SLOT(close()));
        Q_ASSERT(check);
        pWdg->show();
    }
}

void CConnecter::slotBlockShowMessage(QString title, QString message,
                                      QMessageBox::StandardButtons buttons,
                                      QMessageBox::StandardButton &nRet,
                                      bool &checkBox,
                                      QString szCheckBoxContext)
{
    QCheckBox* pBox = nullptr;
    QMessageBox msg(QMessageBox::Information, title, message, buttons, GetViewer());
    if(!szCheckBoxContext.isEmpty())
    {
        pBox = new QCheckBox(szCheckBoxContext);
        if(pBox)
            pBox->setCheckable(true);
        msg.setCheckBox(pBox);
    }
    nRet = (QMessageBox::StandardButton)msg.exec();
    if(pBox)
        checkBox = pBox->isChecked();
}
