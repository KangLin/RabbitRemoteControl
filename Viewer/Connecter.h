//! @author: Kang Lin(kl222@126.com)

#ifndef CCONNECTER_H
#define CCONNECTER_H

#include <QObject>
#include <QDir>
#include <QtPlugin>
#include <QDataStream>
#include <QIcon>
#include "FrmViewer.h"

/**
 * @brief The CConnecter class
 * @addtogroup API
 */
class RABBITREMOTECONTROL_EXPORT CConnecter : public QObject
{
    Q_OBJECT
    
public:
    /**
     * @brief CConnecter
     * @param parent: The parent pointer must be specified as the corresponding CManageConnecter derived class
     */
    explicit CConnecter(QObject *parent = nullptr);

    /**
     * @brief Current connect name. eg: Server name or Ip 
     * @return Current connect name.
     */
    virtual QString Name() = 0;
    virtual QString Description() = 0;
    virtual QString Protol() = 0;
    virtual QIcon Icon();
    
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
    
Q_SIGNALS:
    void sigConnected();
    void sigDisconnected();
    
private:
    CFrmViewer *m_pView;
};

#endif // CCONNECTER_H
