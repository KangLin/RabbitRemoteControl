// Author: Kang Lin <kl222@126.com>

#include "ClipboardMimeData.h"
#include "RabbitCommonLog.h"
#include "ClipboardFreeRdp.h"
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
{}

CClipboardMimeData::~CClipboardMimeData()
{
    m_bExit = true;
    emit sigContinue();
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardMimeData::~CClipboardMimeData()");
}

int CClipboardMimeData::SetFormat(const CLIPRDR_FORMAT_LIST *pList)
{
    ClipboardDestroy(m_pClipboard);
    m_pClipboard = ClipboardCreate();
    if(!m_pClipboard) return -1;
    m_Formats.clear();
    for (UINT32 i = 0; i < pList->numFormats; i++)
	{
		CLIPRDR_FORMAT* pFormat = &pList->formats[i];
        //*
        LOG_MODEL_DEBUG("FreeRdp", "FormatId: 0x%X; name: %s",
                        pFormat->formatId,
                        pFormat->formatName);//*/
        AddFormat(pFormat->formatId, pFormat->formatName);
	}

    if(m_Formats.isEmpty())
        return 0;

    for(auto it = m_Formats.begin(); it != m_Formats.end(); it++)
    {
        if(it->name.isEmpty())
        {
            switch (it->id) {
            case CF_TEXT:
            case CF_OEMTEXT:
            case CF_UNICODETEXT:
            {
                if(m_outFormats.find("text/plain") == m_outFormats.end())
                    m_outFormats["text/plain"] = it->id;
            }
                break;
            case CF_DIB:
            case CF_BITMAP:
            case CF_DIBV5:
            {
                if(m_outFormats.find("image/bmp") == m_outFormats.end())
                    m_outFormats["image/bmp"] = it->id;
            }
                break;
            default:
            {
                const char* name = ClipboardGetFormatName(m_pClipboard, it->id);
                if(name)
                    m_outFormats[name] = it->id;
            }
                break;
            }
        }
        else
        {
            if(m_outFormats.find(it->name) == m_outFormats.end())
                continue;
            m_outFormats[it->name] = it->id;
            if("FileGroupDescriptorW" == it->name)
                if(m_outFormats.find("text/uri-list") == m_outFormats.end())
                {
                    m_outFormats["text/uri-list"] = it->id;
                }
            if("HTML Format" == it->name)
                if(m_outFormats.find("text/html") == m_outFormats.end())
                {
                    m_outFormats["text/html"] = it->id;
                }
            if(isText(it->name))
                if(m_outFormats.find("text/plain") == m_outFormats.end())
                    m_outFormats["text/plain"] = it->id;
            if(isImage(it->name))
                if(m_outFormats.find("image/bmp") == m_outFormats.end())
                    m_outFormats["image/bmp"] = it->id;
        }
    }

    if(m_lstFormats.isEmpty())
        for(auto it = m_outFormats.begin(); m_outFormats.end() != it; it++)
        {
            m_lstFormats << it.key();
        }

    qDebug() << m_lstFormats;
    return 0;
}

int CClipboardMimeData::AddFormat(UINT32 id, const char *name)
{
    int nRet = 0;

    foreach(auto it, m_Formats)
    {
        if(it.id == id)
            return -1;
    }

    _FORMAT f = {id, name};
    f.id = id;
    f.name = name;
    if(name)
        ClipboardRegisterFormat(m_pClipboard, name);

    m_Formats.push_back(f);

    return nRet;
}

bool CClipboardMimeData::hasFormat(const QString &mimetype) const
{
    //*
    LOG_MODEL_DEBUG("FreeRdp", "CMimeData::hasFormat: %s",
                    mimetype.toStdString().c_str());//*/

    if(isText(mimetype)) return true;
    if(isImage(mimetype)) return true;
    if(m_outFormats.find(mimetype) == m_outFormats.end())
        return true;
    return false;
}

QStringList CClipboardMimeData::formats() const
{ 
    //LOG_MODEL_DEBUG("FreeRdp", "CMimeData::formats");
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
    LOG_MODEL_DEBUG("FreeRdp", "CMimeData::retrieveData: %s; type:0x%X",
                    mimetype.toStdString().c_str(), preferredType);
    auto it = GetValue(mimetype, preferredType);
    if(!it.isNull()) return it;

    UINT32 format = 0;
    QString name = mimetype;
    for(auto it = m_outFormats.begin(); it != m_outFormats.end(); it++)
    {
        if(it.key() == mimetype)
        {   
            format = it.value();
            break;
        }
    }

    LOG_MODEL_DEBUG("FreeRdp", "CMimeData::retrieveData: format: %d;mimeData:%s",
                    format, mimetype.toStdString().c_str());
    while(0 == format)
    {
        auto text = m_outFormats.find("text/plain");
        if(text != m_outFormats.end() && isText(mimetype))
        {
            format = text.value();
            break;
        }
        auto image = m_outFormats.find("image/bmp");
        if(image != m_outFormats.end() && isImage(mimetype))
        {
            format = image.value();
            break;
        }
        return QVariant();
    }
    
    if(!m_pContext) return QVariant();

    emit sigSendDataRequest(m_pContext, format, name);
    
    // add wait response event
    QEventLoop loop;
    connect(this, SIGNAL(sigContinue()), &loop, SLOT(quit()), Qt::DirectConnection);
    loop.exec();
    LOG_MODEL_DEBUG("FreeRdp", "CMimeData::retrieveData end");
    // Objecte destruct
    if(m_bExit)
        return QVariant(preferredType);

    it = GetValue(mimetype, preferredType);
    if(!it.isNull()) return it;

    return QVariant();
}

//! if(pData == nullptr && nLen == 0) is Notify clipboard program has exited
int CClipboardMimeData::slotServerFormatData(const BYTE* pData, UINT32 nLen,
                                             UINT32 id, QString name)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardMimeData::slotServerFormatData: id:%d,name:%s",
                    id, name.toStdString().c_str());
    int nRet = 0;
    UINT32 dstFormatId = 0;
    if(!pData && 0 == nLen)
    {
        m_bExit = true;
        m_pContext = nullptr;
    }
    else
    {
        switch (id) {
        case CF_TEXT:
        case CF_OEMTEXT:
        case CF_UNICODETEXT:
        {
            dstFormatId = CF_UNICODETEXT;
        }
            break;
        case CF_BITMAP:
            dstFormatId = CF_BITMAP;
            break;
        default:
            dstFormatId = ClipboardGetFormatId(m_pClipboard, name.toStdString().c_str());
        }
        bool bSuccess = ClipboardSetData(m_pClipboard, id, pData, nLen);
        if(bSuccess)
        {
            UINT32 size = 0;
            void* data = ClipboardGetData(m_pClipboard, dstFormatId, &size);
            QByteArray d((char*)data, size);
            if(!d.isEmpty())
                m_Variant[name] = QVariant(d);
        }
    }
    emit sigContinue();
    return nRet;
}

bool CClipboardMimeData::isText(QString mimeType) const
{
    if(m_outFormats.find("text/plain") != m_outFormats.end())
    {
        QRegularExpression re("text/plain[;]*.*",
                              QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match = re.match(mimeType);
        if(match.hasMatch())
            return true;
    }
    return false;
}

bool CClipboardMimeData::isImage(QString mimeType) const
{
    if(m_outFormats.find("image/bmp") != m_outFormats.end())
    {
        QRegularExpression re(".*image.*",
                              QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match = re.match(mimeType);
        if(match.hasMatch())
            return true;
    }
    return false;
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
QVariant CClipboardMimeData::GetValue(QString mimeType, QMetaType preferredType) const
#else
QVariant CClipboardMimeData::GetValue(QString mimeType, QVariant::Type preferredType) const
#endif
{
    auto it = m_Variant.find(mimeType);
    if(m_Variant.end() != it)
    {
        if(isText(mimeType))
        {
            return QVariant(QMetaType(QMetaType::QString), it.value().data());
        }
        if(isImage(mimeType))
        {
            QImage img;
            if(img.load(it.value().toByteArray(), "bmp"))
                return QVariant(img);
        }
        return QVariant(preferredType, it.value().data());
    }
    return QVariant();
}
