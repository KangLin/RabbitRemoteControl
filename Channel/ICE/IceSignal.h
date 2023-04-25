//! @author Kang Lin <kl222@126.com>

#ifndef CSIGNAL_H
#define CSIGNAL_H

#include <QObject>
#include <string>
#include <QLoggingCategory>

#include "rtc/rtc.hpp"
#include "channel_export.h"

/*!
 * \~chinese
 * \brief ICE 信令接口类
 * \details 此信令基于 [libdatachannel](https://github.com/paullouisageneau/libdatachannel)
 * 
 * \~english
 * \brief The ICE signal interface class
 * \details The signal is base on [libdatachannel](https://github.com/paullouisageneau/libdatachannel)
 * 
 * \~
 * \ingroup LIBAPI_ICE
 */
class CHANNEL_EXPORT CIceSignal : public QObject
{
    Q_OBJECT

public:
    explicit CIceSignal(QObject *parent = nullptr);
    virtual ~CIceSignal();

    virtual int Open(const QString& szServer, quint16 nPort,
                     const QString& user = QString(),
                     const QString& password = QString()) = 0;

    virtual int Close() = 0;
    virtual bool IsConnected() = 0;

    virtual int SendDescription(const QString& toUser,
                                const QString& channelId,
                                const rtc::Description& description,
                                const QString& fromUser = QString()) = 0;
    virtual int SendCandidate(const QString& toUser,
                             const QString& channelId,
                             const rtc::Candidate& candidate,
                             const QString& fromUser = QString()) = 0;

Q_SIGNALS:
    void sigConnected();
    void sigDisconnected();
    void sigError(int nError, const QString& szError);

    /**
     * @brief sigOffer
     * @param user
     * @param id: channel id
     */
    void sigOffer(const QString& fromUser,
                  const QString& toUser,
                  const QString& channelId,
                  const QString& type,
                  const QString& sdp);
    /**
     * @brief sigCandiate
     * @param user
     * @param id: channel id
     * @param mid
     * @param sdp
     */
    void sigCandidate(const QString& fromUser,
                     const QString& toUser,
                     const QString& channelId,
                     const QString& mid,
                     const QString& sdp);
    /**
     * @brief sigDescription
     * @param user
     * @param id: channel id
     * @param type
     * @param sdp
     */
    void sigDescription(const QString& fromUser,
                        const QString& toUser,
                        const QString& channelId,
                        const QString& type,
                        const QString& sdp);
    
protected:
    QLoggingCategory m_Log;
};

#endif // CSIGNAL_H
