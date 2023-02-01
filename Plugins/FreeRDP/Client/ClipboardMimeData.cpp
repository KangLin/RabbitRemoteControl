// Author: Kang Lin <kl222@126.com>

// see: https://learn.microsoft.com/en-us/windows/win32/shell/clipboard

#include "ClipboardMimeData.h"
#include "ClipboardFreeRDP.h"
#include <QDebug>
#include <QEventLoop>
#include <QRegularExpression>
#include <QImage>
#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>

#if !(defined (Q_OS_WINDOWS) || defined(Q_OS_WIN) || defined(Q_OS_WIN32) || defined(Q_OS_WINRT))
    //
    // format of CF_FILEGROUPDESCRIPTOR
    //
    typedef struct _FILEGROUPDESCRIPTORW { // fgd
         UINT cItems;
         FILEDESCRIPTORW fgd[1];
    } FILEGROUPDESCRIPTORW, * LPFILEGROUPDESCRIPTORW;
    
#endif

QAtomicInteger<qint32> CClipboardMimeData::m_nId(1);
static int g_UINT32 = qRegisterMetaType<UINT32>("UINT32");
CClipboardMimeData::CClipboardMimeData(CliprdrClientContext *pContext)
    : QMimeData(),
      m_Log("FreeRDP.Clipboard.MimeData"),
      m_pContext(pContext),
      m_pClipboard(nullptr),
      m_bExit(false)
{
    m_Id = m_nId++;
    while(0 == m_Id)
        m_Id = m_nId++;
    qDebug(m_Log) << "CClipboardMimeData::CClipboardMimeData:" << GetId();
 
    CClipboardFreeRDP* pThis = (CClipboardFreeRDP*)pContext->custom;
    m_pClipboard = pThis->m_pClipboard;
    bool check = false;
    check = connect(this, SIGNAL(sigRequestFileFromServer(const QString&,
                                                          const QString&,
                                                          const void*,
                                                          const UINT32)),
                    this, SLOT(slotRequestFileFromServer(const QString&,
                                                         const QString&,
                                                         const void*,
                                                         const UINT32)),
                    Qt::DirectConnection);
    Q_ASSERT(check);
}

CClipboardMimeData::~CClipboardMimeData()
{
    qDebug(m_Log) << "CClipboardMimeData::~CClipboardMimeData():" << GetId();
    m_bExit = true;
    emit sigContinue();
    qDebug(m_Log) << "CClipboardMimeData::~CClipboardMimeData() end:" << GetId();
}

const qint32 CClipboardMimeData::GetId() const
{
    return m_Id;
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
        qDebug(m_Log) << "Format Id:" << pFormat->formatId
                        << "name:" << pFormat->formatName;//*/
        AddFormat(pFormat->formatId, pFormat->formatName);
    }

    if(m_Formats.isEmpty())
        return 0;

    QString szFormats;
    for(auto it = m_Formats.begin(); it != m_Formats.end(); it++)
    {
        //*
        szFormats += QString::number(it->id) + "[";
        szFormats += it->name;
        szFormats += "]; "; //*/

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
            case CF_DIBV5:
            {
                m_indexString.insert("image/bmp", *it);
                break;
            }
            case CF_HDROP:
            {
                m_indexString.insert("text/uri-list", *it);
                break;
            }
            default:
            {
                const char* name = ClipboardGetFormatName(m_pClipboard, it->id);
                if(name)
                    m_indexString.insert(name, *it);
            }
            }
        } else {
            m_indexString.insert(it->name, *it);
            if("FileGroupDescriptorW" == it->name) {
#ifdef Q_OS_WINDOWS
                m_indexString.insert("text/uri-list", *it);
#else
                m_indexString.insert("x-special/gnome-copied-files", *it);
#endif
            } else if("FileGroupDescriptor" == it->name) {
#ifdef Q_OS_WINDOWS
                m_indexString.insert("text/uri-list", *it);
#else
                m_indexString.insert("x-special/gnome-copied-files", *it);
#endif
            } else if("UniformResourceLocatorW" == it->name) {
                m_indexString.insert("text/uri-list", *it);
            } else if("UniformResourceLocator" == it->name) {
                m_indexString.insert("text/uri-list", *it);
            } else if("x-special/gnome-copied-files" == it->name) {
                m_indexString.insert("text/uri-list", *it);
            } else if("text/html" != it->name && isHtml(it->name, false)) {
                m_indexString.insert("text/html", *it);
            } else if("text/plain" != it->name && isText(it->name, false)) {
                m_indexString.insert("text/plain", *it);
            } else if("image/bmp" != it->name && isImage(it->name)) {
                m_indexString.insert("image/bmp", *it);
            }
        }
    }

    m_lstFormats.clear();
    for(auto it = m_indexString.begin(); m_indexString.end() != it; it++)
    {
        if(!m_lstFormats.contains(it.key()))
            m_lstFormats << (it.key());
    }
    if(m_lstFormats.contains("image/bmp")
            && !m_lstFormats.contains("application/x-qt-image"))
    {
        m_lstFormats << ("application/x-qt-image");
    }

    // Only used by linux or unix
    if(m_lstFormats.contains("text/uri-list")
            && !m_lstFormats.contains("x-special/gnome-copied-files"))
    {
        m_lstFormats.push_front("x-special/gnome-copied-files");
        m_lstFormats.removeOne("text/uri-list");
        m_lstFormats.push_front("text/uri-list");
    }

    // Only used by windows
    if(m_lstFormats.contains("text/uri-list")
            && !m_lstFormats.contains("FileGroupDescriptorW"))
    {
        m_lstFormats.push_front("FileGroupDescriptorW");
        m_lstFormats.removeOne("text/uri-list");
        m_lstFormats.push_front("text/uri-list");
    }

    // Used to identify oneself
    m_lstFormats << MIME_TYPE_RABBITREMOTECONTROL_PLUGINS_FREERDP;

    qDebug(m_Log) << "CClipboardMimeData::SetFormat: input formats:" << szFormats
                  << "Formats:" << m_lstFormats;

    return 0;
}

int CClipboardMimeData::AddFormat(UINT32 id, const char *name)
{
    int nRet = 0;

    foreach(auto it, m_Formats)
    {
        if(it.id == id)
        {
            qWarning(m_Log) << "Repeat format id:" << id;
            return -1;
        }

        if(name)
        {
            if(name == it.name)
            {
                qWarning(m_Log) << "Repeat format name:" << name;
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
    qDebug(m_Log) << "CClipboardMimeData::hasFormat:"
                    << mimetype.toStdString().c_str();//*/

    if(isImage(mimetype) && m_lstFormats.contains("image/bmp"))
        return true;
    if(isUrls(mimetype) && m_lstFormats.contains("text/uri-list"))
        return true;
    return m_lstFormats.contains(mimetype);
}

QStringList CClipboardMimeData::formats() const
{
    /*
    qDebug(m_Log) << "CClipboardMimeData::formats:" <<  m_lstFormats; //*/
    return m_lstFormats;
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
QVariant CClipboardMimeData::retrieveData(const QString &mimeType,
                                          QMetaType preferredType) const

#else
QVariant CClipboardMimeData::retrieveData(const QString &mimeType,
                                          QVariant::Type preferredType) const
#endif
{
    //*
    qDebug(m_Log) << "CClipboardMimeData::retrieveData:" << GetId() << mimeType
                     << "Variant:" << m_Variant; //*/
    if(MIME_TYPE_RABBITREMOTECONTROL_PLUGINS_FREERDP == mimeType)
        return GetId();

    QString mt = mimeType;
    if(isImage(mt)) mt = "image/bmp";
    if(m_indexString.find(mt) == m_indexString.end())
        return QVariant();

    _FORMAT value;
    auto lstValue = m_indexString.values(mt);
    if(lstValue.isEmpty())
        return QVariant();
    value = *lstValue.crbegin();
    //*
    qDebug(m_Log) << "CClipboardMimeData::retrieveData: format id:" << value.id
         << "name:" << value.name << "mimeData:" << mimeType; //*/

    if(!m_pContext) return QVariant();

    emit sigSendDataRequest(m_pContext, value.id);

    // add wait response event
    QEventLoop loop;
    connect(this, SIGNAL(sigContinue()), &loop, SLOT(quit()), Qt::DirectConnection);
    loop.exec();
    qDebug(m_Log) << "CClipboardMimeData::retrieveData end";
    // Objecte destruct
    if(m_bExit)
        return QVariant();

    if(isUrls(mimeType) && !m_Variant.isNull())
    {
        QByteArray data = m_Variant.toByteArray();
        emit sigRequestFileFromServer(mimeType, value.name, data.data(), data.size());
    }
    return m_Variant;
}

//! if(pData == nullptr && nLen == 0) is Notify clipboard program has exited
void CClipboardMimeData::slotServerFormatData(const BYTE* pData, UINT32 nLen,
                                             UINT32 id)
{
    //*
    qDebug(m_Log) << "CClipboardMimeData::slotServerFormatData: id:" << id;//*/

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
        case CF_DIBV5:
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
            qDebug(m_Log) << "ClipboardGetData fail: dstId:" << dstId
                           << "srcId:" << srcId;
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
    //qDebug(m_Log) << "CClipboardMimeData::isText:" << mimeType;
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
    //qDebug(m_Log) << "CClipboardMimeData::isHtml:" << mimeType;
    
    if("text/html" == mimeType || "HTML Format" == mimeType)
        return true;
    
    return false;
}

bool CClipboardMimeData::isUrls(QString mimeType, bool bRegular) const
{
    //qDebug(m_Log) << "CClipboardMimeData::isUrls:" << mimeType;
    
    if("FileGroupDescriptorW" == mimeType
            || "FileGroupDescriptor" == mimeType
            || "UniformResourceLocatorW" == mimeType
            || "UniformResourceLocator" == mimeType
            || "text/uri-list" == mimeType
            || "x-special/gnome-copied-files" == mimeType)
        return true;

    return false;
}

bool CClipboardMimeData::isImage(QString mimeType, bool bRegular) const
{
    //qDebug(m_Log) << "CClipboardMimeData::isImage:" << mimeType;

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

void CClipboardMimeData::slotRequestFileFromServer(const QString &mimeType,
                                                   const QString &valueName,
                                                   const void *pData,
                                                   const UINT32 nLen)
{
    return; //TODO:  delete it!!!
    //*
    qDebug(m_Log) << "CClipboardMimeData::slotRequestFileFromServer:"
                  << valueName << mimeType << pData;//*/
    if(!("FileGroupDescriptorW" == valueName
         || "FileGroupDescriptor" == valueName))
            return;

    int srcId = ClipboardGetFormatId(m_pClipboard, valueName.toStdString().c_str());
    int dstId = ClipboardGetFormatId(m_pClipboard, mimeType.toStdString().c_str());
    bool bSuccess = ClipboardSetData(m_pClipboard, srcId, pData, nLen);
    if(!bSuccess) {
        qCritical(m_Log) << "ClipboardSetData fail: dstId:" << dstId
                       << "srcId:" << srcId;
        return;
    }

    UINT32 size = 0;
    void* data = ClipboardGetData(m_pClipboard, dstId, &size);
    if(!data) {
        qCritical(m_Log) << "ClipboardGetData fail: dstId:" << dstId
                       << "srcId:" << srcId;
        return;
    }
    QString szFiles = QString::fromLatin1((char*)data, size);
    QStringList lstFile = szFiles.split("\n");
    free(data);

    FILEGROUPDESCRIPTORW* pDes = (FILEGROUPDESCRIPTORW*)pData;
    for(int i = 0; i < pDes->cItems; i++)
    {
        QString szFile = lstFile[i].trimmed();
        szFile = QUrl(szFile).toLocalFile();
        QFileInfo fileInfo(szFile);
        QDir d(fileInfo.absolutePath());
        if(!d.exists())
            d.mkpath(fileInfo.absolutePath());

        QSharedPointer<_CliprdrFileStream> stream
                = QSharedPointer<_CliprdrFileStream>(new _CliprdrFileStream());
        stream->m_Success = false;
        stream->m_File.setFileName(szFile) ;
        m_Stream.insert(i, stream);
        
        if(pDes->fgd[i].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;
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

        qDebug(m_Log) << "File" << szFile
                       << ";Length:" << size.u.HighPart << size.u.LowPart;
        // Open local file
        if(!stream->m_File.open(QFile::WriteOnly))
        {
            qCritical(m_Log) << "Open file fail:" << szFile
                              << stream->m_File.errorString();
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
    if("x-special/gnome-copied-files" == mimeType)
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
            gnomeFormat.append(fileName.toStdString().c_str());
        }
        m_gnomeFiles = gnomeFormat;
        m_Variant = gnomeFormat;
    }
    //*
    // "text/uri-list" format is LocalFile1\r\nLocalFile2\r\n...
    // See:
    //   URI is specified by RFC 8089: https://datatracker.ietf.org/doc/html/rfc8089
    //   uri syntax: https://www.rfc-editor.org/rfc/rfc3986#section-3
    //   uri-lists format: https://www.rfc-editor.org/rfc/rfc2483#section-5
    if("text/uri-list" == mimeType || "FileGroupDescriptorW" == mimeType)
    {
        QByteArray uriFormat;
        foreach(auto s, m_Stream)
        {
            if(!s->m_Success)
                continue;
            QString fileName;
            fileName += QUrl::fromLocalFile(s->m_File.fileName()).toEncoded();
            fileName += "\r\n";
            uriFormat.append(fileName.toStdString().c_str());
        }
        m_uriFiles = uriFormat;
        m_Variant = uriFormat;
    } //*/

    qDebug(m_Log) << "CClipboardMimeData::slotRequestFileFromServer::QVariant:" << m_Variant;

    return;
}

UINT CClipboardMimeData::sendRequestFilecontents(UINT32 listIndex,
        UINT32 dwFlags,
        DWORD nPositionHigh,
        DWORD nPositionLow,
        UINT32 cbRequested)
{
    //*
    qDebug(m_Log) << "CClipboardMimeData::sendRequestFilecontents";//*/
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
    //*
    qDebug(m_Log) << "CClipboardMimeData::slotServerFileContentsRespose: index:"
                   << streamId << ";Data length:" << data.size();//*/
    auto stream = m_Stream.find(streamId);
    do{
        if(m_Stream.end() == stream || data.isNull())
            break;
        QSharedPointer<_CliprdrFileStream> s = *stream;
        s->m_Data = data;
    }while(0);
    emit sigContinue();
}
