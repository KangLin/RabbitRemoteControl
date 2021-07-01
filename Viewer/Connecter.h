// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTER_H
#define CCONNECTER_H

#pragma once

#include <QObject>
#include <QDir>
#include <QtPlugin>
#include <QDataStream>
#include <QDialog>
#include <QIcon>
#include <QMimeData>
#include "rabbitremotecontrol_export.h"

class CPluginFactory;

/**
 * \~chinese 描述连接接口。
 * \note 此类是用户接使用接口，由插件实现
 * 
 * \~english
 * \brief Connecter interface
 * \note  The class is a interface used by Use UI
 * 
 * \~
 * \see   CPluginFactory CFrmViewer CConnect
 * \ingroup API PLUGIN_API
 */
class RABBITREMOTECONTROL_EXPORT CConnecter : public QObject
{
    Q_OBJECT
    
public:
    /**
     * \~chinese \param parent: 此指针必须是相应的 CPluginFactory 派生类的实例指针
     * \~english \param parent: The parent pointer must be specified as the corresponding CPluginFactory derived class
     */
    explicit CConnecter(CPluginFactory *parent);
    virtual ~CConnecter();
    
    virtual const QString Id();
    virtual const QString Name();
    virtual const QString Description();
    virtual const QString Protol() const;
    virtual qint16 Version() = 0;
    virtual const QIcon Icon() const;
    
    /**
     * \~chinese
     * \brief 当前连接名。例如：服务名或 IP:端口
     * \return 返回服务名
     * 
     * \~english
     * \brief Current connect server name. eg: Server name or Ip:Port 
     * \return Current connect server name.
     */
    virtual QString ServerName();
    /**
     * \~chinese 
     * \brief 得到显示视图
     * \return CFrmViewer*: 视图指针。它的所有者是本类的实例
     * 
     * \~english
     * \brief Get Viewer 
     * \return CFrmViewer*: the ownership is a instance of this class
     */
    virtual QWidget* GetViewer() = 0;
    /**
     * \~chinese 
     * \brief 打开设置对话框
     * \param parent
     * \return DialogCode
     *   QDialog::Accepted: 接收
     *   QDialog::Rejected: 拒绝
     *   -1: 错误
     *   
     * \~english
     * \brief Open settings dialog
     * \param parent
     * \return DialogCode
     *   QDialog::Accepted
     *   QDialog::Rejected
     *   -1: error
     */
    virtual int OpenDialogSettings(QWidget* parent = nullptr) = 0;

    const CPluginFactory* GetPluginFactory() const;
    virtual int Load(QDataStream& d) = 0;
    virtual int Save(QDataStream& d) = 0;
    
public Q_SLOTS:
    virtual int Connect() = 0;
    virtual int DisConnect() = 0;
    virtual void slotSetClipboard(QMimeData *data);
    virtual void slotSetServerName(const QString &szName);
    
Q_SIGNALS:
    void sigConnected();
    void sigDisconnected();
    
    /// \~chinese \note 在插件中使用时，请使用 slotSetServerName
    /// \~english \note Please use slotSetServerName, when is useed in plugin
    void sigUpdateName(const QString& szName);

    void sigError(const int nError, const QString &szError);
    void sigInformation(const QString& szInfo);
    
private:
    const CPluginFactory* m_pPluginFactory;

    QString m_szServerName;
};

#endif // CCONNECTER_H
