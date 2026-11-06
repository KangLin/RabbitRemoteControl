// Author: Kang Lin <kl222@126.com>

// See: https://learn.microsoft.com/en-us/windows/win32/shell/clipboard

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

#include <winpr/image.h>

const QString& CClipboardMimeData::MimeTextPlain = "text/plain";
const QString& CClipboardMimeData::MimeTextUtf8 = CClipboardMimeData::MimeTextPlain + ";charset=utf-8";

const QString& CClipboardMimeData::MimeBmp = "image/bmp";
const QString& CClipboardMimeData::MimeXBmp = "image/x-bmp";
const QString& CClipboardMimeData::MimeXMsBmp = "image/x-MS-bmp";
const QString& CClipboardMimeData::MimeXWinBitmap = "image/x-win-bitmap";
const QString& CClipboardMimeData::MimeQtImage = "application/x-qt-image";

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
/*
 *      5. Windows:
 */
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

QList<QUrl> uriListToQUrls(const QByteArray &data)
{
    QList<QUrl> urls;

    // 按行分割（兼容 \r\n 和 \n）
    const QList<QByteArray> lines = data.split('\n');

    urls.reserve(lines.size());
    for (QByteArray line : lines) {
        line = line.trimmed();          // 去掉 \r 和首尾空白
        if (line.isEmpty())
            continue;                    // 跳过空行
        if (line.startsWith('#'))        // URI list 规范：以 # 开头的是注释
            continue;

        QUrl url = QUrl::fromEncoded(line, QUrl::StrictMode);
        if (url.isValid())
            urls.append(url);
    }
    return urls;
}

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
    m_pClipboard = ClipboardCreate();;

#ifndef HAVE_FILE_INTERFACE
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
#endif
}

CClipboardMimeData::~CClipboardMimeData()
{
    qDebug(log) << "CClipboardMimeData::~CClipboardMimeData():" << GetId();
    m_bExit = true;
    emit sigContinue();
    qDebug(log) << "CClipboardMimeData::~CClipboardMimeData() end:" << GetId();
    ClipboardDestroy(m_pClipboard);
}

const qint32 CClipboardMimeData::GetId() const
{
    return m_Id;
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
    if(name)
        f.localId = ClipboardRegisterFormat(m_pClipboard, name);

    m_Formats.push_back(f);

    return nRet;
}

int CClipboardMimeData::SetFormat(const CLIPRDR_FORMAT_LIST *pList)
{
    if(!m_pClipboard) {
        Q_ASSERT(FALSE);
        return -1;
    }
    m_Formats.clear();
    for (UINT32 i = 0; i < pList->numFormats; i++) {
        CLIPRDR_FORMAT* pFormat = &pList->formats[i];
        /*
        qDebug(log) << "Format Id:" << pFormat->formatId
                    << "name:" << pFormat->formatName; //*/
        AddFormat(pFormat->formatId, pFormat->formatName);
    }

    if(m_Formats.isEmpty())
        return 0;

    bool bText = false;
    bool bHtml = false;
    bool bImage = false;
    bool bFile = false;
    QStringList lstFormats;
    QString szFormats;
    for(auto it = m_Formats.begin(); it != m_Formats.end(); it++) {
        //*
        szFormats += "(" + QString::number(it->id) + "[";
        szFormats += it->name;
        szFormats += "]" + QString::number(it->localId) + "); "; //*/

        if(it->name.isEmpty()) {
            switch (it->id) {
            case CF_TEXT:
            case CF_OEMTEXT:
            case CF_UNICODETEXT:
            case CF_LOCALE:
            {
                bText = true;
                break;
            }
            case CF_DIB:
            //case CF_BITMAP:
            case CF_DIBV5:
            //case CF_TIFF:
            {
                bImage = true;
                break;
            }
            // case CF_HDROP:
            // {
            //     m_indexString.insert(MimeUriList, *it);
            //     break;
            // }
            default:
                break;
            }
        } else {
            lstFormats << it->name;
            if(isUrls(it->name)) {
                bFile = true;
            }
            if(isHtml(it->name)) {
                bText = true;
                bHtml = true;
            }
            if(isImage(it->name)) {
                bImage = true;
            }
        }
    }

    if(bText)
        lstFormats << GetMimeText();
    if(bHtml)
        lstFormats << GetMimeHtml();
    if(bImage)
        lstFormats << GetMimeImage();
    if(bFile)
        lstFormats << GetMimeFile();
    // Used to identify oneself
    lstFormats << MIME_TYPE_RABBITREMOTECONTROL_PLUGINS_FREERDP;

    // Remove duplicates
    m_lstFormats.clear();
    foreach (auto s, lstFormats) {
        if(m_lstFormats.contains(s))
            continue;
        m_lstFormats << s;
    }

    qDebug(log) << "SetFormat: input formats:" << szFormats
                << "Formats:" << m_lstFormats;

    return 0;
}

bool CClipboardMimeData::hasFormat(const QString &mimetype) const
{
    //*
    qDebug(log) << "hasFormat:"
                << mimetype.toStdString().c_str()
                << m_lstFormats.contains(mimetype); //*/
    return m_lstFormats.contains(mimetype);
}

QStringList CClipboardMimeData::formats() const
{
    //*
    qDebug(log) << "formats:" <<  m_lstFormats; //*/
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    //*
    qDebug(log) << "retrieveData:" << GetId() << mimeType
                << preferredType; //*/
#endif

    if(MIME_TYPE_RABBITREMOTECONTROL_PLUGINS_FREERDP == mimeType)
        return GetId();

    if(!m_lstFormats.contains(mimeType)) {
        qCritical(log) << "Don't contains" << mimeType;
        return QVariant();
    }

    UINT32 id = GetIDFromMime(mimeType);

    //*
    qDebug(log) << "retrieveData: sigSendDataRequest format id:"
                << id << "mime:" << mimeType
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                << preferredType
#endif
        ; //*/

    if(!m_pContext || 0 == id) return QVariant();

    emit sigSendDataRequest(m_pContext, id, mimeType);

    // add wait response event
    QEventLoop loop;
    connect(this, SIGNAL(sigContinue()), &loop, SLOT(quit()), Qt::DirectConnection);
    loop.exec();
    qDebug(log) << "retrieveData end";
    // Objecte destruct
    if(m_bExit)
        return QVariant();

    if(m_Variant.isValid() && !m_Variant.isNull()) {
        if(isUrls(mimeType))
        {
#ifndef HAVE_FILE_INTERFACE
            QByteArray data = m_Variant.toByteArray();
            emit sigRequestFileFromServer(mimeType, mimeType, data.data(), data.size());
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
    qDebug(log) << "slotServerFormatData:" << id << szMimeType; //*/

    UINT32 srcId = 0;
    UINT32 dstId = 0;

    do {
        if(!pData || 0 == nLen) {
            m_pContext = nullptr;
            break;
        }

        _FORMAT format = {0, nullptr, 0};
        foreach(auto f, m_Formats) {
            if(id == f.id) {
                format = f;
                break;
            }
        }
        if(0 == format.id) {
            qCritical(log) << "Don't find id in m_Formats" << id;
            break;
        }

        switch(format.id) {
        case CF_TEXT:
        case CF_OEMTEXT:
        case CF_UNICODETEXT:
        case CF_DIB:
        case CF_DIBV5:
            srcId = format.id;
            break;
        default:
            srcId = format.id;
            if(!format.name.isEmpty()) {
                srcId = ClipboardGetFormatId(m_pClipboard, format.name.toStdString().c_str());
                qDebug(log) << "Get srcId:" << srcId << "name:" << format.name;
            }
        }

        QString mt = szMimeType;
        if(MimeQtImage == szMimeType)
            mt = MimeBmp;
        dstId = ClipboardGetFormatId(m_pClipboard, mt.toStdString().c_str());
        if(0 == dstId)
            qCritical(log) << "The dstId is 0";
        UINT32 size = nLen;
        void* pDstData = (void*)pData;
        if(srcId != dstId) {
            bool bSuccess = ClipboardSetData(m_pClipboard, srcId, pData, nLen);
            if(!bSuccess) {
                qCritical(log) << "ClipboardSetData fail: srcId:" << srcId
                               << "id:" << id << "name:" << format.name
                               << "mime:" << szMimeType;
                break;
            }
            pDstData = ClipboardGetData(m_pClipboard, dstId, &size);
            if(!pDstData) {
                qCritical(log) << "ClipboardGetData fail: dstId:" << dstId
                            << "srcId:" << srcId
                            << "id:" << id << "name:" << format.name
                            << "mime:" << szMimeType;
                break;
            }
        }

        QByteArray baDstData((char*)pDstData, size);
        if(baDstData.isEmpty()) break;
        if(CF_TEXT == id || CF_OEMTEXT == id) {
#if defined(Q_OS_WINDOWS)
            m_Variant = QString::fromLocal8Bit(baDstData);
#else
            m_Variant = QString::fromLatin1(baDstData);
#endif
            break;
        }
        if(MimeQtImage == szMimeType) {
            QImage img;
            img.loadFromData(baDstData, "BMP");
            m_Variant = img;
            break;
        }

        m_Variant = baDstData;

#if HAVE_FILE_INTERFACE
        if(isUrls(szMimeType)) {
            CClipboardFreeRDP* pThis = CClipboardFreeRDP::GetThis(m_pContext);
            if (cliprdr_file_context_has_local_support(pThis->m_pFileContext))
            {
                // data the file list [MS-RDPECLIP] 2.2.5.2.3 Packed File List (CLIPRDR_FILELIST)
                if (!cliprdr_file_context_update_server_data(
                        pThis->m_pFileContext, m_pClipboard, pData, nLen))
                {
                    qCritical(log) << "File clipboard failed to update";
                }
            }
        }
#endif

    } while(0);
    emit sigContinue();
    return;
}

bool CClipboardMimeData::isText(QString mimeType, bool bRegular) const
{
    //qDebug(log) << "CClipboardMimeData::isText:" << mimeType;
    if("UTF8_STRING" == mimeType) return true;
    if("TEXT" == mimeType) return true;
    if("STRING" == mimeType) return true;
    if("COMPOUND_TEXT" == mimeType) return true;
    if(MimeTextPlain == mimeType) return true;
    if(MimeTextUtf8 == mimeType) return true;
    return false;

    if(bRegular) {
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

bool CClipboardMimeData::isBitmap(QString mimeType, bool bRegular) const
{
    if(winpr_image_format_is_supported(WINPR_IMAGE_BITMAP)
        && (MimeBmp == mimeType
            || MimeXBmp == mimeType
            || MimeXMsBmp == mimeType
            || MimeXWinBitmap == mimeType))
        return true;
    return false;
}

bool CClipboardMimeData::isImage(QString mimeType, bool bRegular) const
{
    //qDebug(log) << "CClipboardMimeData::isImage:" << mimeType;

    if(isBitmap(mimeType, bRegular)
        || MimeJxl == mimeType
        || MimeAvif == mimeType
        || (MimePng == mimeType && winpr_image_format_is_supported(WINPR_IMAGE_PNG))
#if FREERDP_VERSION_MAJOR > 3 || (FREERDP_VERSION_MAJOR == 3 && FREERDP_VERSION_MINOR >=3)
        || (MimeWebp == mimeType && winpr_image_format_is_supported(WINPR_IMAGE_WEBP))
        || (MimeJpeg == mimeType && winpr_image_format_is_supported(WINPR_IMAGE_JPEG))
#endif
        || MimeTiff == mimeType
        )
        return true;
    // QClipboard return QImage mimeType is "application/x-qt-image"
    if(MimeQtImage == mimeType) return true;
    return false;
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

const QStringList CClipboardMimeData::GetMimeText()
{
    return {MimeTextPlain};
}

const QStringList CClipboardMimeData::GetMimeHtml()
{
    return {TypeHtmlFormat, MimeHtml};
}

const QStringList CClipboardMimeData::GetMimeImage()
{
    return {MimeBmp, MimeXBmp, MimeXMsBmp, MimeXWinBitmap, MimeQtImage,
            MimeJxl, MimeAvif, MimePng, MimeWebp, MimeJpeg, MimeTiff};
}

const QStringList CClipboardMimeData::GetMimeFile()
{
    return {TypeFileGroupDescriptorW, TypeFileGroupDescriptor,
            MimeUriList, MimeGnomeCopyiedFiles, MimeMateCopyiedFiles};
}

UINT32 CClipboardMimeData::GetIDFromMime(const QString& szMime) const
{
    QString m = szMime;
    if(isHtml(szMime))
        m = TypeHtmlFormat;
    if(isUrls(szMime))
        m = TypeFileGroupDescriptorW;
    foreach(auto f, m_Formats) {
        if(f.name.isEmpty()) continue;
        if(f.name == m)
            return f.id;
    }
    if(isImage(szMime)) {
        UINT32 id = 0;
        foreach (auto f, m_Formats) {
            if(CF_DIB == f.id || CF_DIBV5 == f.id)
                return f.id;
            if(!id && !f.name.isEmpty() && isImage(f.name))
                id = f.id;
        }
        return id ? id : CF_DIB;
    }
    if(isText(szMime)) {
        UINT32 id = 0;
        UINT32 idOther = 0;
        foreach(auto f, m_Formats) {
            if(CF_UNICODETEXT == f.id) {
                return CF_UNICODETEXT;
            }
            if(!id && (CF_TEXT == f.id || CF_OEMTEXT == f.id || CF_LOCALE == f.id)) {
                id = f.id;
                continue;
            }
            if (!idOther && !f.name.isEmpty() && isText(szMime))
                idOther = f.id;
        }
        return id ? id : idOther ? idOther : CF_UNICODETEXT;
    }
    return 0;
}

#ifndef HAVE_FILE_INTERFACE
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
#endif
