//! @author: Kang Lin(kl222@126.com)

#ifndef CCONNECTER_H
#define CCONNECTER_H

#include <QObject>
#include <QDir>
#include <QtPlugin>
#include <QDataStream>
#include <QIcon>
#include <QMimeData>
#include "FrmViewer.h"

class CPluginFactory;

/**
 * @brief The CConnecter class
 * @see   CPluginFactory CFrmViewer CConnect
 * @addtogroup API
 */
class RABBITREMOTECONTROL_EXPORT CConnecter : public QObject
{
    Q_OBJECT
    
public:
    /**
     * @brief CConnecter
     * @param parent: The parent pointer must be specified as the corresponding CPluginFactory derived class
     */
    explicit CConnecter(CPluginFactory *parent);

    virtual const QString Name() const;
    virtual const QString Description() const;
    virtual const QString Protol() const;
    virtual qint16 Version() = 0;
    virtual const QIcon Icon() const;
    
    /**
     * @brief Current connect server name. eg: Server name or Ip:Port 
     * @return Current connect server name.
     */
    virtual QString GetServerName();
    /**
     * @brief GetViewer 
     * @return CFrmViewer* ower is caller. The caller must delete it, when don't use.
     */
    virtual CFrmViewer* GetViewer();
    // QDialog* ower is caller. The caller must delete it, when don't use.
    virtual QDialog* GetDialogSettings(QWidget* parent = nullptr) = 0;

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
    // Please use slotSetServerName, when is useed in plugin
    void sigServerName(const QString& szName);

    void sigError(const int nError, const QString &szError);
    void sigInformation(const QString& szInfo);
    
private:
    CFrmViewer *m_pView;
    const CPluginFactory* m_pPluginFactory;
    
protected:
    QString m_szServerName;
};

#endif // CCONNECTER_H
