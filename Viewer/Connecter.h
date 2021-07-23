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
#include "viewer_export.h"

class CPlugin;

/** 
 * \~english
 * \brief Connecter interface
 * \note  The class is a interface used by Use UI
 * \details Basic implementations of the following types have been provided:
 * 1. Desktop type: \ref CConnecterDesktop
 * 2. Termianal type: \ref CConnecterPluginsTerminal
 * 
 * \~
 * \see   CPluginFactory CFrmViewer CConnect
 * \ingroup VIEWER_API VIEWER_PLUGIN_API
 */
class VIEWER_EXPORT CConnecter : public QObject
{
    Q_OBJECT
    
public:
    /**
     * \~english \param parent: The parent pointer must be specified as the corresponding CPluginFactory derived class
     */
    explicit CConnecter(CPlugin *parent);
    virtual ~CConnecter();
    
    virtual const QString Id();
    virtual const QString Name();
    virtual const QString Description();
    virtual const QString Protol() const;
    virtual qint16 Version() = 0;
    virtual const QIcon Icon() const;
    
    /**
     * \~english
     * \brief Current connect server name. eg: Server name or Ip:Port 
     * \return Current connect server name.
     */
    virtual QString ServerName();
    /**
     * \~english
     * \brief Get Viewer 
     * \return CFrmViewer*: the ownership is a instance of this class
     */
    virtual QWidget* GetViewer() = 0;
    /**
     * \~english
     * \brief Open settings dialog
     * \param parent
     * \return DialogCode
     *   \li QDialog::Accepted
     *   \li QDialog::Rejected
     *   \li -1: error
     */
    virtual int OpenDialogSettings(QWidget* parent = nullptr) = 0;

    const CPlugin* GetPluginFactory() const;
    
    /**
     * \~english \brief Load parameters from file
     */
    virtual int Load(QDataStream& d) = 0;
    /**
     * \~english Save parameters to file
     */
    virtual int Save(QDataStream& d) = 0;
    
public Q_SLOTS:
    /**
     * \~english \brief Start connect
     * \note Call by user, The plugin don't call it. 
     *       When plugin is connected, it emit sigConnected()
     */
    virtual int Connect() = 0;
    /**
     * \~english \brief Close connect
     * \note Call by user, The plugin don't call it.
     *       When plugin is disconnected, it emit sigDisconnected()
     */
    virtual int DisConnect() = 0;
    
    // Follow slot only is used by plugin
    virtual void slotSetClipboard(QMimeData *data);
    virtual void slotSetServerName(const QString &szName);
    
Q_SIGNALS:
    void sigConnected();
    void sigDisconnected();

    /// \~english \note Please use slotSetServerName, when is useed in plugin
    void sigUpdateName(const QString& szName);

    void sigError(const int nError, const QString &szError);
    void sigInformation(const QString& szInfo);
    
private:
    const CPlugin* m_pPluginFactory;

    QString m_szServerName;
};

#endif // CCONNECTER_H
