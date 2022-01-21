// Author: Kang Lin <kl222@126.com>

#include "ClipboardMimeData.h"
#include "RabbitCommonLog.h"
#include "ClipboardFreeRdp.h"
#include <QDebug>
#include <QEventLoop>

CClipboardMimeData::CClipboardMimeData(CliprdrClientContext *pContext)
    : QMimeData(),
      m_pContext(pContext),
      m_pClipboard(nullptr)
{
}

CClipboardMimeData::~CClipboardMimeData()
{
    qDebug() << "CMimeData::~CMimeData()";
    emit sigContinue();
}

int CClipboardMimeData::SetFormat(const CLIPRDR_FORMAT_LIST *pList)
{
    ClipboardDestroy(m_pClipboard);
    m_Formats.clear();
    for (UINT32 i = 0; i < pList->numFormats; i++)
	{
		CLIPRDR_FORMAT* pFormat = &pList->formats[i];
        //TODO: Delete it
        LOG_MODEL_DEBUG("FreeRdp", "cb_cliprdr_server_format_list FormatId: 0x%X; name: %s",
                        pFormat->formatId,
                        pFormat->formatName);
        
        AddFormat(pFormat->formatId, pFormat->formatName);
	}
    return 0;
}

int CClipboardMimeData::AddFormat(int id, const char *name)
{
    int nRet = 0;
        
    auto it = m_Formats.find(id);
    if(m_Formats.end() != it)
        return nRet;
    
    if(name)
        ClipboardRegisterFormat(m_pClipboard, name);
    m_Formats.insert(id, name);

    return nRet;
}

bool CClipboardMimeData::hasFormat(const QString &mimetype) const
{
    LOG_MODEL_DEBUG("FreeRdp", "CMimeData::hasFormat: %s", mimetype.toStdString().c_str());
    foreach(auto f, m_Formats)
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

    for(auto it = m_Formats.begin(); it != m_Formats.end(); it++)
    {
        if(it.value().isEmpty())
        {
            switch (it.key()) {
            case CF_TEXT:
            case CF_OEMTEXT:
            case CF_UNICODETEXT:
                reList << "text/plain";
                break;
            case CF_DIB:
            case CF_BITMAP:
            case CF_DIBV5:
                reList << "image/bmp";
                break;
            
            default:
                continue;
            }
        }
        else
        {
            reList << it.value();
            if("FileGroupDescriptorW" == it.value())
                if(!reList.contains("text/uri-list"))
                    reList << "text/uri-list";
            if("HTML Format" == it.value())
                if(!reList.contains("text/html"))
                    reList << "text/html";
        }
    }
    qDebug() << reList;
    
    return reList << QMimeData::formats();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
QVariant CClipboardMimeData::retrieveData(const QString &mimetype, QMetaType preferredType) const
#else
QVariant CClipboardMimeData::retrieveData(const QString &mimetype, QVariant::Type preferredType) const
#endif
{
    LOG_MODEL_DEBUG("FreeRdp", "CMimeData::retrieveData: %s", mimetype.toStdString().c_str());
    if(!m_Data.isNull())
        return GetData(mimetype, preferredType);
    
    UINT32 format = 0;
    for(auto it = m_Formats.begin(); it != m_Formats.end(); it++)
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

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
QVariant CClipboardMimeData::GetData(const QString &mimetype, QMetaType preferredType) const
#else
QVariant CClipboardMimeData::GetData(const QString &mimetype, QVariant::Type  preferredType) const
#endif
{
    return QVariant();
}
