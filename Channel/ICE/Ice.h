#ifndef CICE_H
#define CICE_H

#include <QObject>
#include <QSharedPointer>
#include "IceSignal.h"
#include "ParameterICE.h"
#include "FrmParameterICE.h"

class CHANNEL_EXPORT CICE : public QObject
{
    Q_OBJECT
public:
    explicit CICE(QObject *parent = nullptr);

    static CICE* Instance();
    
    QSharedPointer<CIceSignal> GetSignal();
    CParameterICE* GetParameter();
    QWidget* GetWidget(QWidget* parent);
    
public Q_SLOTS:
    void slotIceChanged();
    void slotConnected();
    void slotDisconnected();
    void slotError(int nError, const QString& szError);

private:
    QSharedPointer<CIceSignal> m_Signal;
    CParameterICE m_Parameter;
};

#endif // CICE_H
