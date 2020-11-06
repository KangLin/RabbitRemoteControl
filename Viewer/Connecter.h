#ifndef CCONNECTER_H
#define CCONNECTER_H

#include <QObject>
#include <QDir>
#include "FrmViewer.h"
#include <QtPlugin>
#include <QDataStream>

class RABBITREMOTECONTROL_EXPORT CConnecter : public QObject
{
    Q_OBJECT
    
public:
    /**
     * @brief CConnecter
     * @param parent: The parent pointer must be specified as the corresponding CManageConnecter derived class
     */
    explicit CConnecter(QObject *parent = nullptr);

    virtual QString Name();
    virtual QString Description();
    virtual QString Protol() = 0;
    
    /**
     * @brief GetViewer 
     * @return CFrmViewer* ower is caller. The caller must delete it, when don't use.
     */
    virtual CFrmViewer* GetViewer() = 0;
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
};

#endif // CCONNECTER_H
