#include "MimeData.h"
#include "RabbitCommonLog.h"

CMimeData::CMimeData(CliprdrClientContext *pContext)
    : QMimeData(),
      m_pContext(pContext)
{
}

int CMimeData::AddFormat(int id, const QString &name)
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
            it.value() = name;
        else
            LOG_MODEL_WARNING("FreeRdp", "The format is exist.");
    }   
    
    m_Fomats.insert(id, name);
    
    return nRet;
}

bool CMimeData::hasFormat(const QString &mimetype) const
{
    LOG_MODEL_DEBUG("FreeRdp", "CMimeData::hasFormat: %s", mimetype.toStdString().c_str());
    foreach(auto f, m_Fomats)
    {
        if(f == mimetype)
            return true;
    }
    return QMimeData::hasFormat(mimetype);
}

QStringList CMimeData::formats() const
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
            }
        }
        else
            reList << it.value(); 
    }
    return reList << QMimeData::formats();
}

QVariant CMimeData::retrieveData(const QString &mimetype, QVariant::Type preferredType) const
{
    LOG_MODEL_DEBUG("FreeRdp", "CMimeData::retrieveData: %s", mimetype.toStdString().c_str());
    //TODO: add wait response event
    //m_Loop.exec();
    return QMimeData::retrieveData(mimetype, preferredType);
}
