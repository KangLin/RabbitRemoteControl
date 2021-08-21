//! @author Kang Lin(kl222@126.com)

#ifndef CSIGNAL_H
#define CSIGNAL_H

#include <QObject>
#include <string>

#include "rtc/rtc.hpp"

class CIceSignal : public QObject
{
    Q_OBJECT

public:
    explicit CIceSignal(QObject *parent = nullptr);
    virtual ~CIceSignal();

    virtual int Open(const std::string& szServer, quint16 nPort,
                     const std::string& user = std::string(),
                     const std::string& password = std::string()) = 0;

    virtual int Close() = 0;
    virtual bool IsOpen() = 0;

    virtual int SendDescription(const QString& toUser,
                                const QString& channelId,
                                const rtc::Description& description,
                                const QString& fromUser = QString()) = 0;
    virtual int SendCandiate(const QString& toUser,
                             const QString& channelId,
                             const rtc::Candidate& candidate,
                             const QString& fromUser = QString()) = 0;

    /**
     * @brief Write
     * @param buf
     * @param nLen
     * @return If success, return count bytes of be send
     *         If fail, return -1
     */
    virtual int Write(const char* buf, int nLen) = 0;
    /**
     * @brief Read
     * @param buf
     * @param nLen
     * @return If success, return count bytes of be read
     *         If fail, return -1
     */
    virtual int Read(char* buf, int nLen) = 0;

Q_SIGNALS:
    void sigConnected();
    void sigDisconnected();
    void sigReadyRead();
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
    void sigCandiate(const QString& fromUser,
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
};

#endif // CSIGNAL_H
