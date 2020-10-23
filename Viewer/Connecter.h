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

    virtual CFrmViewer* GetViewer() = 0;
    virtual QWidget* GetDialogSettings() = 0;

    virtual int Connect() = 0;
    virtual int DisConnect() = 0;
};

#endif // CCONNECTER_H
