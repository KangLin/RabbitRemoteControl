//! @author Kang Lin(kl222@126.com)

#include "IceSignalWebSocket.h"

#include <nlohmann/json.hpp>

#include <QDebug>

CIceSignalWebSocket::CIceSignalWebSocket(QObject *parent) : CIceSignal(parent)
{
    m_webSocket = std::make_shared<rtc::WebSocket>();
}

CIceSignalWebSocket::~CIceSignalWebSocket()
{
    qDebug() << "CIceSignalWebSocket::~CIceSignalWebSocket()";
}

int CIceSignalWebSocket::Write(const char *buf, int nLen)
{
    bool bRet = 0;
    QMutexLocker lock(&m_MutexSend);
    if(m_webSocket)
    {
        bRet = m_webSocket->send(std::string(buf, nLen));
        if(bRet)
            return nLen;
    }
    return -1;
}

int CIceSignalWebSocket::Read(char *buf, int nLen)
{
    int n = nLen;
    Q_ASSERT(false);
    return n;
}

int CIceSignalWebSocket::Open(const std::string &szServer, quint16 nPort,
                           const std::string &user, const std::string &password)
{
    Q_UNUSED(password);
    std::string szUrl;
    std::string wsPrefix = "ws://";

    const std::string url = wsPrefix + szServer + ":" +
                       std::to_string(nPort) + "/" + user;
    qDebug(m_Log, "Url is %s", url.c_str());
    return Open(url, user, password);
}

int CIceSignalWebSocket::Open(const std::string &szUrl,
                              const std::string &user,
                              const std::string &password)
{
    Q_UNUSED(password);
    if(!m_webSocket)
        m_webSocket = std::make_shared<rtc::WebSocket>();
    if(!m_webSocket) return -1;

    m_webSocket->onOpen([this]() {
        qDebug(m_Log, "WebSocket is open");
        emit sigConnected();
    });
    m_webSocket->onError([this](std::string szErr) {
        qDebug(m_Log, "WebSocket is error");
        emit sigError(-1, szErr.c_str());
    });
    m_webSocket->onClosed([this]() {
        qDebug(m_Log, "WebSocket is close");
        emit sigDisconnected();
    });
    m_webSocket->onMessage([this](std::variant<rtc::binary, std::string> data) {
        if (!std::holds_alternative<std::string>(data))
            return;

        nlohmann::json message = nlohmann::json::parse(std::get<std::string>(data));

        auto it = message.find("id");
        if (it == message.end())
            return;
        it = message.find("toUser");
        if (it == message.end())
            return;
        std::string toUser = it->get<std::string>();
        it = message.find("fromUser");
        if (it == message.end())
            return;
        std::string fromUser = it->get<std::string>();
        it = message.find("channelid");
        if (it == message.end())
            return;
        std::string channelid = it->get<std::string>();

        it = message.find("type");
        if (it == message.end())
            return;
        std::string type = it->get<std::string>();

        if(type == "offer")
        {
            auto sdp = message["description"].get<std::string>();
            emit sigOffer(fromUser.c_str(), toUser.c_str(), channelid.c_str(),
                          type.c_str(), sdp.c_str());
        }
        else if(type == "answer") {
            auto sdp = message["description"].get<std::string>();
            emit sigDescription(fromUser.c_str(), toUser.c_str(),
                                channelid.c_str(), type.c_str(), sdp.c_str());
        } else if (type == "candidate") {
            auto sdp = message["candidate"].get<std::string>();
            auto mid = message["mid"].get<std::string>();
            emit sigCandiate(fromUser.c_str(), toUser.c_str(),
                             channelid.c_str(), mid.c_str(), sdp.c_str());
        }
    });

    m_szUser = user;
    m_szUrl = szUrl;
    try {
        m_webSocket->open(szUrl);
    }  catch (std::exception &e) {
        qCritical(m_Log,
                        "Open websocket fail: %s; %s",
                        e.what(), szUrl.c_str());
        return -1;
    }

    return 0;
}

int CIceSignalWebSocket::Close()
{
    if(!m_webSocket) return -1;
    m_webSocket->close();
    return 0;
}

bool CIceSignalWebSocket::IsOpen()
{
    if(m_webSocket)
        return m_webSocket->isOpen();
    return false;
}

int CIceSignalWebSocket::SendCandiate(const QString& toUser,
                                      const QString &channelId,
                                      const rtc::Candidate &candidate,
                                      const QString &fromUser)
{
    std::string user = fromUser.toStdString();
    if(user.empty())
        user = m_szUser;
    nlohmann::json message = {{"id", toUser.toStdString()},
                              {"toUser", toUser.toStdString()},
                              {"fromUser", user},
                              {"channelid", channelId.toStdString()},
                              {"type", "candidate"},
                              {"candidate", std::string(candidate)},
                              {"mid", candidate.mid()}};

    std::string m = message.dump();
    return Write(m.c_str(), m.size());
}

int CIceSignalWebSocket::SendDescription(const QString& toUser,
                                         const QString& channelId,
                                         const rtc::Description &description,
                                         const QString& fromUser)
{
    std::string user = fromUser.toStdString();
    if(user.empty())
        user = m_szUser;
    nlohmann::json message = {
        {"id", toUser.toStdString()},
        {"toUser", toUser.toStdString()},
        {"fromUser", user},
        {"channelid", channelId.toStdString()},
        {"type", description.typeString()},
        {"description", std::string(description)}};

    std::string m = message.dump();
    return Write(m.c_str(), m.size());
}
