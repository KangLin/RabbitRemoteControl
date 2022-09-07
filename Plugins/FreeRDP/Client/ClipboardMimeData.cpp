// Author: Kang Lin <kl222@126.com>

#include "ClipboardMimeData.h"
#include "RabbitCommonLog.h"
#include "ClipboardFreeRDP.h"
#include <QDebug>
#include <QEventLoop>
#include <QRegularExpression>
#include <QImage>
#include <QUrl>

static int g_UINT32 = qRegisterMetaType<UINT32>("UINT32");
CClipboardMimeData::CClipboardMimeData(CliprdrClientContext *pContext)
    : QMimeData(),
      m_pContext(pContext),
      m_pClipboard(nullptr),
      m_bExit(false)
{
    CClipboardFreeRDP* pThis = (CClipboardFreeRDP*)pContext->custom;
    m_pClipboard = pThis->m_pClipboard;
    bool check = false;
    check = connect(this, SIGNAL(sigRequestFileFromServer(const QString&, void*, UINT32)),
                    this, SLOT(slotRequestFileFromServer(const QString&, void*, UINT32)),
                    Qt::DirectConnection);
    Q_ASSERT(check);
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
        /*
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
            if("x-special/gnome-copied-files" == it->name)
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
    if(m_lstFormats.contains("text/uri-list")
            && !m_lstFormats.contains("x-special/gnome-copied-files"))
    {
        m_lstFormats.push_front("x-special/gnome-copied-files");
        m_lstFormats.removeOne("text/uri-list");
        m_lstFormats.push_front("text/uri-list");
    }
    
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
    /*
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
    //LOG_MODEL_DEBUG("FreeRdp", "CMimeData::formats");

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
    qDebug() << mimetype << preferredType << m_Variant;

    if(!m_Variant.isNull())
    {
        return m_Variant;
    }

    QString mt = mimetype;
    if(isImage(mt)) mt = "image/bmp";
    if(m_indexString.find(mt) == m_indexString.end())
        return QVariant();

    _FORMAT value;
    auto lstValue = m_indexString.values(mt);
    if(lstValue.isEmpty())
        return QVariant();
    value = *lstValue.crbegin();
    /*
    LOG_MODEL_DEBUG("FreeRdp",
                    "CMimeData::retrieveData: format: %d; name:%s; mimeData:%s",
                    value.id,
          value.name.toStdString().c_str(), mimetype.toStdString().c_str());//*/

    if(!m_pContext) return QVariant();

    emit sigSendDataRequest(m_pContext, value.id);

    // add wait response event
    QEventLoop loop;
    connect(this, SIGNAL(sigContinue()), &loop, SLOT(quit()), Qt::DirectConnection);
    loop.exec();
    //LOG_MODEL_DEBUG("FreeRdp", "CMimeData::retrieveData end");
    // Objecte destruct
    if(m_bExit)
        return QVariant();

    if(isUrls(mimetype))
    {
        QByteArray data = m_Variant.toByteArray();
        emit sigRequestFileFromServer(mimetype, data.data(), data.size());
    }
    return m_Variant;
}

//! if(pData == nullptr && nLen == 0) is Notify clipboard program has exited
void CClipboardMimeData::slotServerFormatData(const BYTE* pData, UINT32 nLen,
                                             UINT32 id)
{
    /*
    LOG_MODEL_DEBUG("FreeRdp",
                    "CClipboardMimeData::slotServerFormatData: id:%d",
                    id);//*/

    UINT32 srcId = 0;
    UINT32 dstId = 0;
    do{
        if(!pData && 0 == nLen)
        {
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
                else
                    dstId = it.localId;
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
        case CF_TEXT:
        {
            m_Variant = QString::fromLatin1(d);
            break;
        }
        case CF_OEMTEXT:
        {
#ifdef Q_OS_WINDOWS
            m_Variant = QString::fromLocal8Bit(d);
#else
            m_Variant = d;
#endif
            break;
        }
        case CF_UNICODETEXT:
        {
            m_Variant = QString::fromUtf16((const char16_t*)data, size / 2);
            break;
        }
        default:
            if("UTF8_STRING" == it.name) {
                m_Variant = QString::fromUtf8(d);
            } else if(isHtml(it.name)) {
                m_Variant = QString(d);
            } else if(ClipboardGetFormatId(m_pClipboard, "image/bmp") == dstId) {
                QImage img;
                if(img.loadFromData(d, "BMP"))
                    m_Variant = img;
            } else
                m_Variant = QVariant(d);
        }
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

void CClipboardMimeData::slotRequestFileFromServer(const QString &mimetype,
                                              void *pData, UINT32 nLen)
{
    /*
    LOG_MODEL_DEBUG("CClipboardMimeData",
                    "CClipboardMimeData::slotRequestFileFromServer:%s; %s",
                    mimetype.toStdString().c_str(),
                    pData);//*/
    // Get file index and file name
    QString szFiles = QString::fromLatin1((char*)pData, nLen);
    QStringList lstFile = szFiles.split("\r\n");
    for(int i = 0; i < lstFile.size() - 1; i++)
    {
        QString szFile = lstFile.at(i);
        QSharedPointer<_CliprdrFileStream> stream
                = QSharedPointer<_CliprdrFileStream>(new _CliprdrFileStream());
        stream->m_File.setFileName(lstFile.at(i));
        stream->m_Success = false;
        m_Stream.insert(i, stream);
        // Get file size
        UINT rc = sendRequestFilecontents(i,  FILECONTENTS_SIZE, 0, 0, 8);
        if(CHANNEL_RC_OK != rc)
        {
            continue;
        }
        if(stream->m_Data.isNull() || stream->m_Data.size() == 0)
            continue;
        ULARGE_INTEGER size, offset;
        offset.QuadPart = 0;
        size.QuadPart = *((LONGLONG*)(stream->m_Data.data()));
        if(size.QuadPart <= 0)
            continue;
        /*
        LOG_MODEL_DEBUG("CClipboardMimeData", "File %s length:%d %d",
                        szFile.toStdString().c_str(), size.u.HighPart,
                        size.u.LowPart);//*/
        // Open local file
        if(!stream->m_File.open(QFile::WriteOnly))
        {
            return;
        }
        bool bSuccess = true;
        do {
            UINT32 nBlock = 2 << 15;
            UINT32 nLen = size.QuadPart - offset.QuadPart;
            if(nLen > nBlock)
                nLen = nBlock;
            // Request file from server
            rc = sendRequestFilecontents(i,  FILECONTENTS_RANGE,
                                         offset.u.HighPart,
                                         offset.u.LowPart,
                                         nLen);
            if(CHANNEL_RC_OK != rc)
            {
                bSuccess = false;
                break;
            }
            if(stream->m_Data.isNull() || stream->m_Data.size() == 0)
            {
                bSuccess = false;
                break;
            }
            // Save to local file
            stream->m_File.write(stream->m_Data);
            offset.QuadPart += stream->m_Data.size();
        } while(offset.QuadPart < size.QuadPart);
        stream->m_File.close();
        if(!bSuccess)
            stream->m_File.remove();
        stream->m_Success = bSuccess;
    }
    // Convert file list
    // "x-special/gnome-copied-files" format is copy\nLocalFile1\nLocalFile2\n...
    // "text/uri-list" format is LocalFile1\r\nLocalFile2\r\n...
    if("x-special/gnome-copied-files" == mimetype)
    {
        QByteArray gnomeFormat;
        gnomeFormat.append("copy\n");
        int b = 0;
        foreach(auto s, m_Stream)
        {
            if(!s->m_Success)
                continue;
            QString fileName;
            if(b)
                fileName += "\n";
            else
                b=1;
            fileName += QUrl::fromLocalFile(s->m_File.fileName()).toEncoded();
            gnomeFormat.append(fileName);
        }
        m_gnomeFiles = gnomeFormat;
        m_Variant = gnomeFormat;
    }
    //*
    if("text/uri-list" == mimetype || "FileGroupDescriptorW" == mimetype)
    {
        QByteArray uriFormat;
        foreach(auto s, m_Stream)
        {
            if(!s->m_Success)
                continue;
            QString fileName;
            fileName += QUrl::fromLocalFile(s->m_File.fileName()).toEncoded();
            fileName += "\r\n";
            uriFormat.append(fileName);
        }
        m_uriFiles = uriFormat;
        m_Variant = uriFormat;
    } //*/

    qDebug() << "CClipboardMimeData::slotRequestFileFromServer::QVariant:" << m_Variant;

    return;
}

UINT CClipboardMimeData::sendRequestFilecontents(UINT32 listIndex,
        UINT32 dwFlags,
        DWORD nPositionHigh,
        DWORD nPositionLow,
        UINT32 cbRequested)
{
	UINT rc = ERROR_INTERNAL_ERROR;
    if(!m_pContext) return rc;

    CLIPRDR_FILE_CONTENTS_REQUEST fileContentsRequest = {0};
	fileContentsRequest.streamId = listIndex;
	fileContentsRequest.listIndex = listIndex;
	fileContentsRequest.dwFlags = dwFlags;
    switch (dwFlags)
    {
    /*
     * [MS-RDPECLIP] 2.2.5.3 File Contents Request PDU (CLIPRDR_FILECONTENTS_REQUEST).
     *
     * A request for the size of the file identified by the lindex field. The size MUST be
     * returned as a 64-bit, unsigned integer. The cbRequested field MUST be set to
     * 0x00000008 and both the nPositionLow and nPositionHigh fields MUST be
     * set to 0x00000000.
     */
    case FILECONTENTS_SIZE:
        fileContentsRequest.cbRequested = sizeof(UINT64);
        fileContentsRequest.nPositionHigh = 0;
        fileContentsRequest.nPositionLow = 0;
        break;
    case FILECONTENTS_RANGE:
        fileContentsRequest.cbRequested = cbRequested;
        fileContentsRequest.nPositionHigh = nPositionHigh;
        fileContentsRequest.nPositionLow = nPositionLow;
        break;
    }
    fileContentsRequest.clipDataId = 0;
    fileContentsRequest.msgFlags = 0;
    rc = m_pContext->ClientFileContentsRequest(m_pContext, &fileContentsRequest);

    // add wait response event
    QEventLoop loop;
    connect(this, SIGNAL(sigContinue()), &loop, SLOT(quit()), Qt::DirectConnection);
    loop.exec();

    // Objecte destruct
    if(m_bExit)
        return CHANNEL_RC_NULL_DATA;

	return rc;
}

void CClipboardMimeData::slotServerFileContentsRespose(UINT32 streamId,
                                                       QByteArray &data)
{
    /*
    LOG_MODEL_DEBUG("CClipboardMimeData",
                    "CClipboardMimeData::slotServerFileContentsRespose: index:%d,data length:%d",
                    streamId, data.size());//*/
    auto stream = m_Stream.find(streamId);
    do{
        if(m_Stream.end() == stream || data.isNull())
            break;
        QSharedPointer<_CliprdrFileStream> s = *stream;
        s->m_Data = data;
    }while(0);
    emit sigContinue();
}
