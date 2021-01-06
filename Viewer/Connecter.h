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
    explicit CConnecter(CPluginFactory *parent = nullptr);

    /**
     * @brief Name, The name must same CPluginFactory::Name
     * @return 
     */
    virtual QString Name() = 0;
    virtual QString Description() = 0;
    virtual QString Protol() = 0;
    virtual qint16 Version() = 0;
    virtual QIcon Icon();
    
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

protected:
    QString m_szServerName;
};

#endif // CCONNECTER_H
