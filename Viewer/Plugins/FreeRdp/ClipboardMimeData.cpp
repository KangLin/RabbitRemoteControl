#include "ClipboardMimeData.h"
#include "RabbitCommonLog.h"
#include "ClipboardFreeRdp.h"
#include <QDebug>
#include <QEventLoop>

CClipboardMimeData::CClipboardMimeData(CliprdrClientContext *pContext)
    : QMimeData(),
      m_pContext(pContext)
{
}

CClipboardMimeData::~CClipboardMimeData()
{
    qDebug() << "CMimeData::~CMimeData()";
    emit sigContinue();
}

int CClipboardMimeData::AddFormat(int id, const QString &name)
{
    int nRet = 0;
    if(!name.isEmpty())
    {
        foreach(auto f, m_Fomats)
        {
            if(f == name)
                return nRet;
        }
    }
    
    auto it = m_Fomats.find(id);
    if(m_Fomats.end() != it)
    {
        if(it.value().isEmpty())
        {
            if(name.isEmpty())
            {
                switch (id) {
                case CF_TEXT:
                    it.value() = "text/plain";
                    break;
                case CF_DIB:
                    it.value() = "image/bmp";
                    break;
                case CB_FORMAT_PNG:
                    it.value() = "image/png";
                    break;
                case CB_FORMAT_JPEG:
                    it.value() = "image/jpeg";
                    break;
                case CB_FORMAT_GIF:
                    it.value() = "image/gif";
                    break;
                case CB_FORMAT_HTML:
                    it.value() = "text/html";
                    break;
                default:
                    break;;
                }
                
            } else
                it.value() = name;
        }
        else
            LOG_MODEL_WARNING("FreeRdp", "The format is exist.");
        return nRet;
    }   
    
    QString szName = name;
    if(name.isEmpty())
    {
        switch (id) {
        case CF_TEXT:
            szName = "text/plain";
            break;
        case CF_DIB:
            szName = "image/bmp";
            break;
        case CB_FORMAT_PNG:
            szName = "image/png";
            break;
        case CB_FORMAT_JPEG:
            szName = "image/jpeg";
            break;
        case CB_FORMAT_GIF:
            szName = "image/gif";
            break;
        case CB_FORMAT_HTML:
            szName = "text/html";
            break;
        default:
            break;;
        }
    }
    
    m_Fomats.insert(id, szName);

    return nRet;
}

bool CClipboardMimeData::hasFormat(const QString &mimetype) const
{
    LOG_MODEL_DEBUG("FreeRdp", "CMimeData::hasFormat: %s", mimetype.toStdString().c_str());
    foreach(auto f, m_Fomats)
    {
        if(f == mimetype)
            return true;
    }
    return QMimeData::hasFormat(mimetype);
}

QStringList CClipboardMimeData::formats() const
{
    QStringList reList;    
    LOG_MODEL_DEBUG("FreeRdp", "CMimeData::formats");

    for(auto it = m_Fomats.begin(); it != m_Fomats.end(); it++)
    {
        if(it.value().isEmpty())
        {
            switch (it.key()) {
            case CF_TEXT:
                reList << "text/plain";
                break;
            case CF_DIB:
                reList << "image/bmp";
                break;
            case CB_FORMAT_PNG:
                reList << "image/png";
                break;
            case CB_FORMAT_JPEG:
                reList << "image/jpg" << "image/jpeg";
                break;
            case CB_FORMAT_GIF:
                reList << "image/gif";
                break;
            case CB_FORMAT_HTML:
                reList << "text/html";
                break;
            default:
                continue;
            }
        }
        else
            reList << it.value(); 
    }
    qDebug() << reList;
    
    return reList << QMimeData::formats();
}

QVariant CClipboardMimeData::retrieveData(const QString &mimetype, QVariant::Type preferredType) const
{
    LOG_MODEL_DEBUG("FreeRdp", "CMimeData::retrieveData: %s", mimetype.toStdString().c_str());
    if(!m_Data.isNull())
        return m_Data;
    
    UINT32 format = 0;
    for(auto it = m_Fomats.begin(); it != m_Fomats.end(); it++)
    {
        if(it.value().isEmpty()) break;
        if(it.value() == mimetype)
        {
            format = it.key();
            break;
        }
    }
    LOG_MODEL_DEBUG("FreeRdp", "CMimeData::retrieveData: format: %d", format);
    if(0 == format)
    {
        return QVariant();
    }
    if(CHANNEL_RC_OK != CClipboardFreeRdp::SendDataRequest(m_pContext, format))
        return QVariant();
    
    // add wait response event
    QEventLoop loop;
    connect(this, SIGNAL(sigContinue()), &loop, SLOT(quit()));
    loop.exec();
    
    LOG_MODEL_DEBUG("FreeRdp", "CMimeData::retrieveData end");
//    switch (preferredType) {
//    case QVariant::String:
//        return QVariant(QString(m_Data.toStdString().c_str()));
//        break;
//    }
    return m_Data;
    //return QMimeData::retrieveData(mimetype, preferredType);
}

int CClipboardMimeData::SetData(const char *data, int len)
{
    int nRet = 0;
    QByteArray d(data, len);
    m_Data = d;
    emit sigContinue();
    return nRet;
}
