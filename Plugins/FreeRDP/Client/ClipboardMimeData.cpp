// Author: Kang Lin <kl222@126.com>

// see: https://learn.microsoft.com/en-us/windows/win32/shell/clipboard

#include "ClipboardMimeData.h"
#include "ClipboardFreeRDP.h"
#include <QLoggingCategory>
#include <QEventLoop>
#include <QRegularExpression>
#include <QImage>
#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>

const QString& CClipboardMimeData::MimeTextPlain = "text/plain";
const QString& CClipboardMimeData::MimeTextUtf8 = CClipboardMimeData::MimeTextPlain + ";charset=utf-8";

const QString& CClipboardMimeData::MimeBmp = "image/bmp";
const QString& CClipboardMimeData::MimeXBmp = "image/x-bmp";
const QString& CClipboardMimeData::MimeXMsBmp = "image/x-MS-bmp";
const QString& CClipboardMimeData::MimeXWinBitmap = "image/x-win-bitmap";

const QString& CClipboardMimeData::MimeJxl = "image/jxl";
const QString& CClipboardMimeData::MimeAvif = "image/avif";
const QString& CClipboardMimeData::MimePng = "image/png";
const QString& CClipboardMimeData::MimeWebp = "image/webp";
const QString& CClipboardMimeData::MimeJpeg = "image/jpeg";
const QString& CClipboardMimeData::MimeTiff = "image/tiff";

const QString& CClipboardMimeData::MimeHtml = "text/html";
const QString& CClipboardMimeData::TypeHtmlFormat = "HTML Format";

// See: register_file_formats_and_synthesizers()[FreeRDP/winpr/libwinpr/clipboard/synthetic_file.c]
/*	    1. Gnome Nautilus based file manager (Nautilus only with version >= 3.30 AND < 40):
            TARGET: UTF8_STRING
            format: x-special/nautilus-clipboard\copy\n\file://path\n\0
*/
/* 	    2. Kde Dolpin and Qt:
            TARGET: text/uri-list
            format: file:path\r\n\0
            See:
              GTK: https://docs.gtk.org/glib/struct.Uri.html
              uri syntax: https://www.rfc-editor.org/rfc/rfc3986#section-3
              uri-lists format: https://www.rfc-editor.org/rfc/rfc2483#section-5
*/
const QString& CClipboardMimeData::MimeUriList = "text/uri-list";
/*	    3. Gnome and others (Unity/XFCE/Nautilus < 3.30/Nautilus >= 40):
            TARGET: x-special/gnome-copied-files
            format: copy\nfile://path\n\0
*/
const QString& CClipboardMimeData::MimeGnomeCopyiedFiles = "x-special/gnome-copied-files";
/*	    4. Mate Caja:
            TARGET: x-special/mate-copied-files
            format: copy\nfile://path\n
*/
const QString& CClipboardMimeData::MimeMateCopyiedFiles = "x-special/mate-copied-files";
const QString& CClipboardMimeData::TypeFileGroupDescriptorW = "FileGroupDescriptorW";
const QString& CClipboardMimeData::TypeFileGroupDescriptor = "FileGroupDescriptor";

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

static Q_LOGGING_CATEGORY(log, "FreeRDP.Clipboard.MimeData")

CClipboardMimeData::CClipboardMimeData(CliprdrClientContext *pContext)
    : QMimeData(),
    m_pContext(pContext),
    m_pClipboard(nullptr),
    m_bExit(false)
{
    m_Id = m_nId++;
    while(0 == m_Id)
        m_Id = m_nId++;
    qDebug(log) << "CClipboardMimeData::CClipboardMimeData:" << GetId();

    CClipboardFreeRDP* pThis = CClipboardFreeRDP::GetThis(pContext);
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
    qDebug(log) << "CClipboardMimeData::~CClipboardMimeData():" << GetId();
    m_bExit = true;
    emit sigContinue();
    qDebug(log) << "CClipboardMimeData::~CClipboardMimeData() end:" << GetId();
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
        qDebug(log) << "Format Id:" << pFormat->formatId
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
        szFormats += "]" + QString::number(it->localId) + "; "; //*/

        m_indexId.insert(it->id, *it);
        if(it->name.isEmpty())
        {
            switch (it->id) {
            case CF_TEXT:
            case CF_OEMTEXT:
            case CF_UNICODETEXT:
            case CF_LOCALE:
            {
                m_indexString.insert(MimeTextPlain, *it);
                break;
            }
            case CF_DIB:
            //case CF_BITMAP:
            case CF_DIBV5:
            case CF_TIFF:
            {
                m_indexString.insert(MimeBmp, *it);
                break;
            }
            case CF_HDROP:
            {
                m_indexString.insert(MimeUriList, *it);
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
            if(TypeFileGroupDescriptorW == it->name) {
                m_indexString.insert(MimeUriList, *it);
#if defined(Q_OS_LINUX)
                m_indexString.insert(MimeGnomeCopyiedFiles, *it);
                m_indexString.insert(MimeMateCopyiedFiles, *it);
#endif
            } else if(TypeFileGroupDescriptor == it->name) {
                m_indexString.insert(MimeUriList, *it);
#if defined(Q_OS_LINUX)
                m_indexString.insert(MimeGnomeCopyiedFiles, *it);
                m_indexString.insert(MimeMateCopyiedFiles, *it);
#endif
            } else if("UniformResourceLocatorW" == it->name) {
                m_indexString.insert(MimeUriList, *it);
            } else if("UniformResourceLocator" == it->name) {
                m_indexString.insert(MimeUriList, *it);
            } else if(MimeGnomeCopyiedFiles == it->name) {
                m_indexString.insert(MimeUriList, *it);
            } else if(MimeHtml != it->name && isHtml(it->name, false)) {
                m_indexString.insert(MimeHtml, *it);
            } else if(MimeTextPlain != it->name && isText(it->name, false)) {
                m_indexString.insert(MimeTextPlain, *it);
            } else if(MimeBmp != it->name && isImage(it->name)) {
                m_indexString.insert(MimeBmp, *it);
                m_indexString.insert("application/x-qt-image", *it);
            }
        }
    }

    m_lstFormats.clear();
    for(auto it = m_indexString.begin(); m_indexString.end() != it; it++) {
        if(!m_lstFormats.contains(it.key()))
            m_lstFormats << (it.key());
    }
    if(m_lstFormats.contains(MimeBmp)
        && !m_lstFormats.contains("application/x-qt-image")) {
        m_lstFormats << ("application/x-qt-image");
    }

    // Only used by linux or unix
    if(m_lstFormats.contains(MimeUriList)
        && !m_lstFormats.contains(MimeGnomeCopyiedFiles))
    {
        m_lstFormats.push_front(MimeGnomeCopyiedFiles);
        m_lstFormats.removeOne(MimeUriList);
        m_lstFormats.push_front(MimeUriList);
    }

    // Only used by windows
    if(m_lstFormats.contains(MimeUriList)
        && !m_lstFormats.contains(TypeFileGroupDescriptorW))
    {
        m_lstFormats.push_front(TypeFileGroupDescriptorW);
        m_lstFormats.removeOne(MimeUriList);
        m_lstFormats.push_front(MimeUriList);
    }

    // Used to identify oneself
    m_lstFormats << MIME_TYPE_RABBITREMOTECONTROL_PLUGINS_FREERDP;

    qDebug(log) << "CClipboardMimeData::SetFormat: input formats:" << szFormats
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
            qWarning(log) << "Repeat format id:" << id;
            return -1;
        }
        if(name) {
            if(name == it.name)
            {
                qWarning(log) << "Repeat format name:" << name;
                return -2;
            }
        }
    }

    _FORMAT f = {id, name, id};
    if(name) {
        f.localId = ClipboardRegisterFormat(m_pClipboard, name);
    }

    m_Formats.push_back(f);

    return nRet;
}

bool CClipboardMimeData::hasFormat(const QString &mimetype) const
{
    //*
    qDebug(log) << "CClipboardMimeData::hasFormat:"
                << mimetype.toStdString().c_str();//*/

    if(isImage(mimetype) && m_lstFormats.contains(MimeBmp))
        return true;
    if(isUrls(mimetype) && m_lstFormats.contains(MimeUriList))
        return true;
    return m_lstFormats.contains(mimetype);
}

QStringList CClipboardMimeData::formats() const
{
    /*
    qDebug(log) << "CClipboardMimeData::formats:" <<  m_lstFormats; //*/
    return m_lstFormats;
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
QVariant CClipboardMimeData::retrieveData(
    const QString &mimeType, QMetaType preferredType) const

#else
QVariant CClipboardMimeData::retrieveData(
    const QString &mimeType, QVariant::Type preferredType) const
#endif
{
    //*
    qDebug(log) << "CClipboardMimeData::retrieveData:" << GetId() << mimeType
                << "Variant:" << m_Variant; //*/
    if(MIME_TYPE_RABBITREMOTECONTROL_PLUGINS_FREERDP == mimeType)
        return GetId();

    QString mt = mimeType;
    if(isImage(mt)) mt = MimeBmp;
    if(m_indexString.find(mt) == m_indexString.end())
        return QVariant();

    _FORMAT value;
    auto lstValue = m_indexString.values(mt);
    if(lstValue.isEmpty())
        return QVariant();
    value = *lstValue.crbegin();
    //*
    qDebug(log) << "CClipboardMimeData::retrieveData: format id:" << value.id
                << "name:" << value.name << "mimeData:" << mimeType; //*/

    if(m_Variant.isValid() && !m_Variant.isNull()) {
        if(isUrls(mimeType))
        {
#ifndef HAVE_FILE_INTERFACE
            QByteArray data = m_Variant.toByteArray();
            emit sigRequestFileFromServer(mimeType, value.name, data.data(), data.size());
#endif
        }
        return m_Variant;
    }
    if(!m_pContext) return QVariant();

    emit sigSendDataRequest(m_pContext, value.id, mimeType);

    // add wait response event
    QEventLoop loop;
    connect(this, SIGNAL(sigContinue()), &loop, SLOT(quit()), Qt::DirectConnection);
    loop.exec();
    qDebug(log) << "CClipboardMimeData::retrieveData end";
    // Objecte destruct
    if(m_bExit)
        return QVariant();

    if(m_Variant.isValid() && !m_Variant.isNull()) {
        if(isUrls(mimeType))
        {
#ifndef HAVE_FILE_INTERFACE
            QByteArray data = m_Variant.toByteArray();
            emit sigRequestFileFromServer(mimeType, value.name, data.data(), data.size());
#endif
        }
    }
    return m_Variant;
}

//! if(pData == nullptr && nLen == 0) is Notify clipboard program has exited
void CClipboardMimeData::slotServerFormatData(
    const BYTE* pData, UINT32 nLen, UINT32 id, QString szMimeType)
{
    //*
    qDebug(log) << Q_FUNC_INFO << id << szMimeType;//*/

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
            dstId = ClipboardGetFormatId(m_pClipboard, MimeBmp.toStdString().c_str());
            break;
        }
        default:
        {
            srcId = it.localId;
            if(it.name.isEmpty())
                dstId = it.localId;
            else {
                if(isHtml(it.name, false))
                    dstId = ClipboardGetFormatId(
                        m_pClipboard, MimeHtml.toStdString().c_str());
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
            qDebug(log) << "ClipboardGetData fail: dstId:" << dstId
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
            m_Variant = QString((QChar*)data);
            break;
        }
        default:
            if("UTF8_STRING" == it.name) {
                m_Variant = QString::fromUtf8(d);
            } else if(isHtml(it.name)) {
                m_Variant = QString(d);
            } else if(ClipboardGetFormatId(m_pClipboard, MimeBmp.toStdString().c_str()) == dstId) {
                QImage img;
                if(img.loadFromData(d, "BMP"))
                    m_Variant = img;
            } else if(isUrls(it.name)) {

#if HAVE_FILE_INTERFACE
                    CClipboardFreeRDP* pThis = CClipboardFreeRDP::GetThis(m_pContext);
                    if (cliprdr_file_context_has_local_support(pThis->m_pFileContext))
                    {
                        if (!cliprdr_file_context_update_server_data(
                                pThis->m_pFileContext, m_pClipboard, data, size))
                        {
                            qCritical(log) << "File clipboard failed to update";
                        }
                    }

                    auto dstId = ClipboardGetFormatId(m_pClipboard, szMimeType.toStdString().c_str());
                    UINT32 size = 0;
                    void* pData = ClipboardGetData(m_pClipboard, dstId, &size);
                    if(pData && size > 0) {
                        QByteArray d((char*)pData, size);
                        m_Variant = d;
                    }
#endif

            } else
                m_Variant = QVariant(d);
        }
    }while(0);
    emit sigContinue();
    return;
}

bool CClipboardMimeData::isText(QString mimeType, bool bRegular) const
{
    //qDebug(log) << "CClipboardMimeData::isText:" << mimeType;
    if("UTF8_STRING" == mimeType) return true;
    if("TEXT" == mimeType) return true;
    if("STRING" == mimeType) return true;
    if(MimeTextPlain == mimeType) return true;
    if(MimeTextUtf8 == mimeType) return true;
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
    //qDebug(log) << "CClipboardMimeData::isHtml:" << mimeType;
    
    if(MimeHtml == mimeType || TypeHtmlFormat == mimeType)
        return true;
    
    return false;
}

bool CClipboardMimeData::isUrls(QString mimeType, bool bRegular) const
{
    //qDebug(log) << "CClipboardMimeData::isUrls:" << mimeType;

    if(TypeFileGroupDescriptorW == mimeType
        || TypeFileGroupDescriptor == mimeType
        || "UniformResourceLocatorW" == mimeType
        || "UniformResourceLocator" == mimeType
        || MimeUriList == mimeType
        || MimeGnomeCopyiedFiles == mimeType
        || MimeMateCopyiedFiles == mimeType)
        return true;

    return false;
}

bool CClipboardMimeData::isImage(QString mimeType, bool bRegular) const
{
    //qDebug(log) << "CClipboardMimeData::isImage:" << mimeType;

    if(MimeBmp == mimeType
        || MimeXBmp == mimeType
        || MimeXMsBmp == mimeType
        || MimeXWinBitmap == mimeType
        || MimeJxl == mimeType
        || MimeAvif == mimeType
        || MimePng == mimeType
        || MimeWebp == mimeType
        || MimeJpeg == mimeType
        || MimeTiff == mimeType
        ) return true;
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
    qDebug(log) << "CClipboardMimeData::slotRequestFileFromServer:"
                << valueName << mimeType << pData;//*/
    if(!(TypeFileGroupDescriptorW == valueName
          || TypeFileGroupDescriptor == valueName))
        return;

    int srcId = ClipboardGetFormatId(m_pClipboard, valueName.toStdString().c_str());
    int dstId = ClipboardGetFormatId(m_pClipboard, mimeType.toStdString().c_str());
    bool bSuccess = ClipboardSetData(m_pClipboard, srcId, pData, nLen);
    if(!bSuccess) {
        qCritical(log) << "ClipboardSetData fail: dstId:" << dstId
                       << "srcId:" << srcId;
        return;
    }

    UINT32 size = 0;
    void* data = ClipboardGetData(m_pClipboard, dstId, &size);
    if(!data) {
        qCritical(log) << "ClipboardGetData fail: dstId:" << dstId
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

        qDebug(log) << "File" << szFile
                    << ";Length:" << size.u.HighPart << size.u.LowPart;
        // Open local file
        if(!stream->m_File.open(QFile::WriteOnly))
        {
            qCritical(log) << "Open file fail:" << szFile
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
    if(MimeGnomeCopyiedFiles == mimeType)
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
    if(MimeUriList == mimeType || TypeFileGroupDescriptorW == mimeType)
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

    qDebug(log) << "CClipboardMimeData::slotRequestFileFromServer::QVariant:" << m_Variant;

    return;
}

UINT CClipboardMimeData::sendRequestFilecontents(UINT32 listIndex,
                                                 UINT32 dwFlags,
                                                 DWORD nPositionHigh,
                                                 DWORD nPositionLow,
                                                 UINT32 cbRequested)
{
    //*
    qDebug(log) << "CClipboardMimeData::sendRequestFilecontents";//*/
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
    qDebug(log) << "CClipboardMimeData::slotServerFileContentsRespose: index:"
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
