#ifndef CICE_H
#define CICE_H

#include <QObject>
#include <QSharedPointer>
#include <QTranslator>

#include "IceSignal.h"
#include "ParameterICE.h"
#include "FrmParameterICE.h"

class CHANNEL_EXPORT CICE : public QObject
{
    Q_OBJECT
public:
    explicit CICE(QObject *parent = nullptr);
    virtual ~CICE();
    
    static CICE* Instance();
    
    QSharedPointer<CIceSignal> GetSignal();
    CParameterICE* GetParameter();
    QWidget* GetParameterWidget(QWidget* parent = nullptr);
    
public Q_SLOTS:
    void slotStart();
    void slotStop();
    void slotIceChanged();
    void slotConnected();
    void slotDisconnected();
    void slotError(int nError, const QString& szError);

private:
    QSharedPointer<CIceSignal> m_Signal;
    CParameterICE m_Parameter;
    QTranslator m_Translator;
};

#endif // CICE_H
