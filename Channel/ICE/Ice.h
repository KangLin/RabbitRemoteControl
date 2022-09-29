//! @author Kang Lin <kl222@126.com>

#ifndef CICE_H
#define CICE_H

#include <QObject>
#include <QSharedPointer>
#include <QTranslator>

#include "IceSignal.h"
#include "ParameterICE.h"
#include "FrmParameterICE.h"

/*!
 * \~chinese
 * \brief 提供管理、访问ICE的一个单例对象
 * 
 * \~english
 * \brief Provides a singleton object for management and access to ICE
 * 
 * \~
 * \ingroup LIBAPI_ICE
 * \see CIceSignal CParameterICE CFrmParameterICE
 */
class CHANNEL_EXPORT CICE : public QObject
{
    Q_OBJECT
public:
    explicit CICE(QObject *parent = nullptr);
    virtual ~CICE();
    
    /*!
     * \brief Single instance
     * \return 
     */
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
    QLoggingCategory m_Log;
};

#endif // CICE_H
