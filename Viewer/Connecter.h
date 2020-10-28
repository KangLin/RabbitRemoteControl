#ifndef CCONNECTER_H
#define CCONNECTER_H

#include <QObject>
#include <QDir>
#include "FrmViewer.h"
#include <QtPlugin>

class RABBITREMOTECONTROL_EXPORT CConnecter : public QObject
{
    Q_OBJECT
    
public:
    explicit CConnecter(QObject *parent = nullptr);

    // CFrmViewer* ower is caller. The caller must delete it, when don't use.
    virtual CFrmViewer* GetViewer() = 0;
    // QDialog* ower is caller. The caller must delete it, when don't use.
    virtual QDialog* GetDialogSettings(QWidget* parent = nullptr) = 0;

public Q_SLOTS:
    virtual int Connect() = 0;
    virtual int DisConnect() = 0;
    
Q_SIGNALS:
    void sigConnected();
    void sigDisconnected();
};

#endif // CCONNECTER_H
