// Author: Kang Lin <kl222@126.com>

#include "ClipboardMimeData.h"
#include "RabbitCommonLog.h"
#include "ClipboardFreeRDP.h"
#include <QDebug>
#include <QEventLoop>
#include <QRegularExpression>
#include <QImage>

static int g_UINT32 = qRegisterMetaType<UINT32>("UINT32");
CClipboardMimeData::CClipboardMimeData(CliprdrClientContext *pContext)
    : QMimeData(),
      m_pContext(pContext),
      m_pClipboard(nullptr),
      m_bExit(false)
{
    CClipboardFreeRDP* pThis = (CClipboardFreeRDP*)pContext->custom;
    m_pClipboard = pThis->m_pClipboard;
}

CClipboardMimeData::~CClipboardMimeData()
{
    m_bExit = true;
    emit sigContinue();
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardMimeData::~CClipboardMimeData()");
}

int CClipboardMimeData::SetFormat(const CLIPRDR_FORMAT_LIST *pList)
{
    if(!m_pClipboard)
    {
        Q_ASSERT(FALSE);
        return -1;
    }
    m_Formats.clear();
    for (UINT32 i = 0; i < pList->numFormats; i++)
	{
		CLIPRDR_FORMAT* pFormat = &pList->formats[i];
        //*
        LOG_MODEL_DEBUG("FreeRdp", "Format Id: 0x%X; name: %s",
                        pFormat->formatId,
                        pFormat->formatName);//*/
        AddFormat(pFormat->formatId, pFormat->formatName);
    }
    
    if(m_Formats.isEmpty())
        return 0;

    for(auto it = m_Formats.begin(); it != m_Formats.end(); it++)
    {
        m_indexId.insert(it->id, *it);
        if(it->name.isEmpty())
        {
            switch (it->id) {
            case CF_TEXT:
            case CF_OEMTEXT:
            case CF_UNICODETEXT:
            case CF_LOCALE:
            {
                m_indexString.insert("text/plain", *it);
                break;
            }
            case CF_DIB:
            //case CF_BITMAP:
            //case CF_DIBV5:
            {
                m_indexString.insert("image/bmp", *it);
                break;
            }
            default:
            {
                const char* name = ClipboardGetFormatName(m_pClipboard, it->id);
                if(name)
                    m_indexString.insert(name, *it);
            }
            }
        }
        else
        {
            m_indexString.insert(it->name, *it);
            if("FileGroupDescriptorW" == it->name)
            {
                m_indexString.insert("text/uri-list", *it);
            }
            if("text/html" != it->name && isHtml(it->name, false))
            {
                m_indexString.insert("text/html", *it);
            }
            if("text/plain" != it->name && isText(it->name, false))
            {
                m_indexString.insert("text/plain", *it);
            }
            if("image/bmp" != it->name && isImage(it->name))
            {
                m_indexString.insert("image/bmp", *it);
            }
        }
    }

    m_lstFormats.clear();
    for(auto it = m_indexString.begin(); m_indexString.end() != it; it++)
    {
        if(!m_lstFormats.contains(it.key()))
            m_lstFormats << it.key();
    }
//    if(m_lstFormats.contains("text/uri-list")
//            && !m_lstFormats.contains("x-special/gnome-copied-files"))
//        m_lstFormats << "x-special/gnome-copied-files";
    qDebug() << "Formats:" << m_lstFormats;

    return 0;
}

int CClipboardMimeData::AddFormat(UINT32 id, const char *name)
{
    int nRet = 0;

    foreach(auto it, m_Formats)
    {
        if(it.id == id)
        {
            LOG_MODEL_WARNING("CClipboardMimeData", "Repeat format id: 0x%X", id);
            return -1;
        }

        if(name)
        {
            if(name == it.name)
            {
                LOG_MODEL_WARNING("CClipboardMimeData", "Repeat format name: %s", name);
                return -2;
            }
        }
    }

    _FORMAT f = {id, name, id};
    if(name)
    {
        f.localId = ClipboardRegisterFormat(m_pClipboard, name);
    }

    m_Formats.push_back(f);

    return nRet;
}

bool CClipboardMimeData::hasFormat(const QString &mimetype) const
{
    //*
    LOG_MODEL_DEBUG("FreeRdp", "CMimeData::hasFormat: %s",
                    mimetype.toStdString().c_str());//*/

    if(isImage(mimetype) && m_lstFormats.contains("image/bmp"))
        return true;
    if(isUrls(mimetype) && m_lstFormats.contains("text/uri-list"))
        return true;
    return m_lstFormats.contains(mimetype);
}

QStringList CClipboardMimeData::formats() const
{ 
    LOG_MODEL_DEBUG("FreeRdp", "CMimeData::formats");
    qDebug() << m_lstFormats;
    return m_lstFormats;
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
QVariant CClipboardMimeData::retrieveData(const QString &mimetype,
                                          QMetaType preferredType) const
#else
QVariant CClipboardMimeData::retrieveData(const QString &mimetype,
                                          QVariant::Type preferredType) const
#endif
{
    LOG_MODEL_DEBUG("FreeRdp", "CMimeData::retrieveData: %s; type:%d",
                    mimetype.toStdString().c_str(), preferredType);
    qDebug() << mimetype << preferredType;

    if(!m_Variant.isNull())
    {
        if(isUrls(mimetype))
        {
            //TODO: Add request file from server
            ;
        }
        return m_Variant;
    }

    QString mt = mimetype;
    if(isImage(mt)) mt = "image/bmp";
    //if(isUrls(mt)) mt = "text/uri-list";
    if(m_indexString.find(mt) == m_indexString.end())
        return QVariant();

    auto value = m_indexString.value(mt);

    LOG_MODEL_DEBUG("FreeRdp", "CMimeData::retrieveData: format: %d; name:%s; mimeData:%s",
                    value.id, value.name.toStdString().c_str(), mimetype.toStdString().c_str());

    if(!m_pContext) return QVariant();

    emit sigSendDataRequest(m_pContext, value.id);

    // add wait response event
    QEventLoop loop;
    connect(this, SIGNAL(sigContinue()), &loop, SLOT(quit()), Qt::DirectConnection);
    loop.exec();
    LOG_MODEL_DEBUG("FreeRdp", "CMimeData::retrieveData end");
    // Objecte destruct
    if(m_bExit)
        return QVariant();

    return m_Variant;
}

//! if(pData == nullptr && nLen == 0) is Notify clipboard program has exited
void CClipboardMimeData::slotServerFormatData(const BYTE* pData, UINT32 nLen,
                                             UINT32 id)
{
    LOG_MODEL_DEBUG("FreeRdp",
                    "CClipboardMimeData::slotServerFormatData: id:%d",
                    id);

    UINT32 srcId = 0;
    UINT32 dstId = 0;
    do{
        if(!pData && 0 == nLen)
        {
            m_bExit = true;
            m_pContext = nullptr;
            break;
        }

        if(m_indexId.find(id) == m_indexId.end())
            break;

        auto it = m_indexId[id];
        switch (id) {
        case CF_DIB:
        //case CF_DIBV5:
        {
            srcId = it.localId;
            dstId = ClipboardGetFormatId(m_pClipboard, "image/bmp");
            break;
        }
        default:
        {
            srcId = it.localId;
            if(it.name.isEmpty())
                dstId = it.localId;
            else {
                if(isHtml(it.name, false))
                    dstId = ClipboardGetFormatId(m_pClipboard, "text/html");
                else if(isUrls(it.name, false))
                    dstId = ClipboardGetFormatId(m_pClipboard, "text/uri-list");
            }
        }
        }
        bool bSuccess = ClipboardSetData(m_pClipboard, srcId, pData, nLen);
        if(!bSuccess) break;

        UINT32 size = 0;
        void* data = ClipboardGetData(m_pClipboard, dstId, &size);
        if(!data)
        {
            LOG_MODEL_ERROR("CClipboardMimeData",
                            "ClipboardGetData fail: dstId: %d srcId: %d",
                            dstId, srcId);
            break;
        }

        QByteArray d((char*)data, size);
        if(d.isEmpty()) break;

        switch (id) {
        case CF_UNICODETEXT:
        {
            m_Variant = QString::fromUtf16((const char16_t*)data, size / 2);
            break;
        }
        default:
            break;
        }
        if("UTF8_STRING" == it.name) {
            m_Variant = QString::fromUtf8(d);
        } else if(isHtml(it.name)) {
            m_Variant = QString(d);
        } else if(ClipboardGetFormatId(m_pClipboard, "image/bmp") == dstId) {
            QImage img;
            if(img.loadFromData(d, "BMP"))
                m_Variant = img;
        } else if(isUrls(it.name)) {
            qDebug() << d;
            m_Variant = d;
        }
        else
            m_Variant = QVariant(d);

    }while(0);
    emit sigContinue();
    return;
}

bool CClipboardMimeData::isText(QString mimeType, bool bRegular) const
{
//    LOG_MODEL_DEBUG("CClipboardMimeData", "CClipboardMimeData::isText: %s",
//                    mimeType.toStdString().c_str());
    if("UTF8_STRING" == mimeType) return true;
    if("TEXT" == mimeType) return true;
    if("STRING" == mimeType) return true;
    if("text/plain" == mimeType) return true;
    if(bRegular)
    {
        QRegularExpression re("text/plain[;]*.*",
                              QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match = re.match(mimeType);
        if(match.hasMatch())
            return true;
    }
    return false;
}

bool CClipboardMimeData::isHtml(QString mimeType, bool bRegular) const
{
//    LOG_MODEL_DEBUG("CClipboardMimeData", "CClipboardMimeData::isHtml: %s",
//                    mimeType.toStdString().c_str());
    
    if("text/html" == mimeType || "HTML Format" == mimeType)
        return true;

    return false;
}

bool CClipboardMimeData::isUrls(QString mimeType, bool bRegular) const
{
//    LOG_MODEL_DEBUG("CClipboardMimeData", "CClipboardMimeData::isUrls: %s",
//                    mimeType.toStdString().c_str());
    
    if("FileGroupDescriptorW" == mimeType || "text/uri-list" == mimeType
            || "x-special/gnome-copied-files" == mimeType)
        return true;

    return false;
}

bool CClipboardMimeData::isImage(QString mimeType, bool bRegular) const
{
//    LOG_MODEL_DEBUG("CClipboardMimeData", "CClipboardMimeData::isImage: %s",
//                    mimeType.toStdString().c_str());

    if("image/bmp" == mimeType) return true;
    // QClipboard return QImage mimeType is "application/x-qt-image"
    if("application/x-qt-image" == mimeType) return true;
    if(bRegular)
    {
        QRegularExpression re("image/.*",
                              QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match = re.match(mimeType);
        if(match.hasMatch())
            return true;
    }
    return false;
}
