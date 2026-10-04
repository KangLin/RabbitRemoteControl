// Author: Kang Lin <kl222@126.com>
// See: https://github.com/KangLin/Documents/blob/master/qt/clipboard.md
// RDP protocol: https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpeclip/fb9b7e0b-6db4-41c2-b83c-f889c1ee7688

#include "ClipboardFreeRDP.h"
#include <QClipboard>
#include <QApplication>
#include <QMimeData>
#include <QtDebug>
#include <QImage>
#include <QBuffer>
#include <QStandardPaths>
#include <QDir>

#include <winpr/image.h>
#include "BackendFreeRDP.h"
#include "ClipboardMimeData.h"

static Q_LOGGING_CATEGORY(log, "FreeRDP.Clipboard")

CClipboardFreeRDP::CClipboardFreeRDP(CBackendFreeRDP *parent) : QObject(parent)
    , m_pConnect(parent)
    , m_pCliprdrClientContext(nullptr)
    , m_pClipboard(nullptr)
#if HAVE_FILE_INTERFACE
    , m_pFileContext(nullptr)
#else
    , m_FileCapabilityFlags(0)
    , m_bFileSupported(false)
    , m_bFileFormatsRegistered(false)
#endif
{
    qDebug(log) << "CClipboardFreeRDP::CClipboardFreeRDP()";
    m_pClipboard = ClipboardCreate();
#if HAVE_FILE_INTERFACE
    m_pFileContext = cliprdr_file_context_new(this);
    cliprdr_file_context_set_locally_available(m_pFileContext, TRUE);
#else
    if (ClipboardGetFormatId(m_pClipboard, "text/uri-list"))
        m_bFileFormatsRegistered = true;
    wClipboardDelegate* pDelegate = ClipboardGetDelegate(m_pClipboard);
    pDelegate->custom = this;
    /* Set up a filesystem base path for local URI */
    QString szPath = QStandardPaths::writableLocation(
                         QStandardPaths::TempLocation)
                     + QDir::separator() + "Rabbit"
                     + QDir::separator() + "RabbitRemoteControl";
    qDebug(log) << "Delegate base path:" << szPath;

    pDelegate->basePath = _strdup(szPath.toStdString().c_str());
    pDelegate->ClipboardFileSizeSuccess = cb_clipboard_file_size_success;
    pDelegate->ClipboardFileSizeFailure = cb_clipboard_file_size_failure;
    pDelegate->ClipboardFileRangeSuccess = cb_clipboard_file_range_success;
    pDelegate->ClipboardFileRangeFailure = cb_clipboard_file_range_failure;

#if FREERDP_VERSION_MAJOR > 2 || (FREERDP_VERSION_MAJOR == 2 && FREERDP_VERSION_MINOR > 7)
    pDelegate->IsFileNameComponentValid = cbIsFileNameComponentValid;
#endif
#endif
}

CClipboardFreeRDP::~CClipboardFreeRDP()
{
    qDebug(log) << "CClipboardFreeRdp::~CClipboardFreeRdp()";

#if HAVE_FILE_INTERFACE
    cliprdr_file_context_free(m_pFileContext);
#else
    // Notify clipboard program has exited
    emit sigServerFormatData(nullptr, 0, 0, QString());
    QByteArray data;
    emit sigServerFileContentsRespose(-1, data);
#endif
    ClipboardDestroy(m_pClipboard);
}

CClipboardFreeRDP* CClipboardFreeRDP::GetThis(CliprdrClientContext *context)
{
    CClipboardFreeRDP* pThis = nullptr;

#ifdef HAVE_FILE_INTERFACE
    pThis = static_cast<CClipboardFreeRDP*>(
        cliprdr_file_context_get_context(
            static_cast<CliprdrFileContext*>(context->custom)));
#else
    pThis = static_cast<CClipboardFreeRDP*>(context->custom);
#endif
    return pThis;
}

int CClipboardFreeRDP::Init(CliprdrClientContext *context, bool bEnable)
{
    if(!bEnable) return 0;

    m_pCliprdrClientContext = context;
    context->custom = this;

    // See: [MS_RDPECLIP] 1.3.2.1 Initialization Sequence
    // https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpeclip/a5cae3c9-170c-4154-992d-9ac8a149cc7e
    context->ServerCapabilities = cbServerCapabilities;
    context->MonitorReady = cbMonitorReady;
    context->ServerFormatList = cbServerFormatList;
    context->ServerFormatListResponse = cbServerFormatListResponse;
    context->ServerFormatDataRequest = cbServerFormatDataRequest;
    context->ServerFormatDataResponse = cbServerFormatDataResponse;

#if HAVE_FILE_INTERFACE
    // set context->custom = m_pFileContext in cliprdr_file_context_init()
    return cliprdr_file_context_init(m_pFileContext, m_pCliprdrClientContext);
#else
    context->ServerFileContentsRequest = cb_cliprdr_server_file_contents_request;
    //context->ServerFileContentsResponse = cb_cliprdr_server_file_contents_response;
#endif

    return 0;
}

int CClipboardFreeRDP::UnInit(CliprdrClientContext *context, bool bEnable)
{
#if HAVE_FILE_INTERFACE
    cliprdr_file_context_uninit(m_pFileContext, m_pCliprdrClientContext);
#endif
    context->custom = nullptr;
    m_pCliprdrClientContext = nullptr;
    return 0;
}

void CClipboardFreeRDP::slotClipBoardChanged()
{
    qDebug(log) << "CClipboardFreeRdp::slotClipBoardChanged";
    // Whether it is the clipboard's QMimeData set by this connection
    const QMimeData* pMimeType = QApplication::clipboard()->mimeData();
    if(!pMimeType) return;

    if(pMimeType->hasFormat(MIME_TYPE_RABBITREMOTECONTROL_PLUGINS_FREERDP)) {
        qint32 data = 0;
        QByteArray d = pMimeType->data(MIME_TYPE_RABBITREMOTECONTROL_PLUGINS_FREERDP);
        if(!d.isNull()) {
            bool ok = false;
            data = d.toInt(&ok);
            if(!m_lstClipboardMimeDataId.isEmpty() && ok
                && m_lstClipboardMimeDataId.contains(data))
            {//*
                qDebug(log)
                    << "CClipboardFreeRdp::slotClipBoardChanged: clipboard is this owner"
                    << data << m_lstClipboardMimeDataId;//*/
                return;
            }
        }
    }

    m_lstClipboardMimeDataId.clear();
    SendClientFormatList(m_pCliprdrClientContext);
}

UINT CClipboardFreeRDP::cbServerCapabilities(
    CliprdrClientContext* context,
    const CLIPRDR_CAPABILITIES* capabilities)
{
    qDebug(log) << Q_FUNC_INFO;

    int nRet = CHANNEL_RC_OK;
    if(!context || !capabilities) return CHANNEL_RC_NOT_INITIALIZED;
    CClipboardFreeRDP* pThis = GetThis(context);
    const BYTE* capsPtr = (const BYTE*)capabilities->capabilitySets;

#ifdef HAVE_FILE_INTERFACE
    if (!cliprdr_file_context_remote_set_flags(pThis->m_pFileContext, 0))
        return ERROR_INTERNAL_ERROR;
#else
    pThis->m_bFileSupported = FALSE;
#endif

    for (UINT32 i = 0; i < capabilities->cCapabilitiesSets; i++)
    {
        const CLIPRDR_CAPABILITY_SET* caps = (const CLIPRDR_CAPABILITY_SET*)capsPtr;
        if (caps->capabilitySetType == CB_CAPSTYPE_GENERAL)
        {
            const CLIPRDR_GENERAL_CAPABILITY_SET* generalCaps = (const CLIPRDR_GENERAL_CAPABILITY_SET*)caps;
#ifdef HAVE_FILE_INTERFACE
            if (!cliprdr_file_context_remote_set_flags(
                    pThis->m_pFileContext, generalCaps->generalFlags)) {
                qCritical(log) << "Set file flags failed";
                return ERROR_INTERNAL_ERROR;
            }
#else
            if (generalCaps->generalFlags & CB_STREAM_FILECLIP_ENABLED)
            {
                // Support file clipboard
                pThis->m_bFileSupported = TRUE;
            }
#endif
        }
        capsPtr += caps->capabilitySetLength;
    }
    return nRet;
}

///////// Send format list from client to server ///////////

// 1.3.2.1 Initialization Sequence:
//    https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpeclip/a5cae3c9-170c-4154-992d-9ac8a149cc7e
UINT CClipboardFreeRDP::cbMonitorReady(
    CliprdrClientContext *context,
    const CLIPRDR_MONITOR_READY *monitorReady)
{
    qDebug(log) << Q_FUNC_INFO;
    UINT nRet = CHANNEL_RC_OK;
    if (!context || !context->ClientCapabilities || !monitorReady)
    {
        Q_ASSERT(false);
        return ERROR_INTERNAL_ERROR;
    }
    CClipboardFreeRDP* pThis = GetThis(context);

    // Send client capabilities
    CLIPRDR_CAPABILITIES capabilities;
    memset(&capabilities, 0, sizeof(CLIPRDR_CAPABILITIES));
    CLIPRDR_GENERAL_CAPABILITY_SET generalCapabilitySet;
    capabilities.cCapabilitiesSets = 1;
    capabilities.capabilitySets = (CLIPRDR_CAPABILITY_SET*)&(generalCapabilitySet);
    generalCapabilitySet.capabilitySetType = CB_CAPSTYPE_GENERAL;
    generalCapabilitySet.capabilitySetLength = sizeof(CLIPRDR_GENERAL_CAPABILITY_SET);
    generalCapabilitySet.version = CB_CAPS_VERSION_2;
    generalCapabilitySet.generalFlags = CB_USE_LONG_FORMAT_NAMES;
#if HAVE_FILE_INTERFACE
    generalCapabilitySet.generalFlags |=
        cliprdr_file_context_current_flags(pThis->m_pFileContext);
#else
    if (pThis->m_bFileSupported && pThis->m_bFileFormatsRegistered)
    {
        generalCapabilitySet.generalFlags |=
            CB_STREAM_FILECLIP_ENABLED | CB_FILECLIP_NO_FILE_PATHS
#if FREERDP_VERSION_MAJOR > 2 || (FREERDP_VERSION_MAJOR == 2 && FREERDP_VERSION_MINOR > 7)
            | CB_HUGE_FILE_SUPPORT_ENABLED
#endif
            ;
    }

    pThis->m_FileCapabilityFlags = generalCapabilitySet.generalFlags;
#endif

    if((nRet = context->ClientCapabilities(context, &capabilities)) != CHANNEL_RC_OK) {
        qCritical(log) << "Send Client Capabilities fail";
        return nRet;
    }

    // Send client formats
    return SendClientFormatList(context);
}

// See: [MS_RDPECLIP] 1.3.2.2 Data Transfer Sequences
//      https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpeclip/395bc830-f2c2-40e5-a3f3-23e41183b777
// Clipboard Formats: https://docs.microsoft.com/en-us/windows/win32/dataxchg/clipboard-formats
UINT CClipboardFreeRDP::SendClientFormatList(CliprdrClientContext *context)
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = CHANNEL_RC_OK;

    if(!context) return CHANNEL_RC_NULL_DATA;
    
    CClipboardFreeRDP* pThis = GetThis(context);
    QClipboard *clipboard = QApplication::clipboard();
    if(!clipboard)
    {
        qDebug(log) << "clipboard is null";
        return CHANNEL_RC_NULL_DATA;
    }

    const QMimeData* pData = clipboard->mimeData();
    if(!pData || pData->formats().isEmpty())
    {
        qDebug(log) << "clipboard->mimeData is null";
        return CHANNEL_RC_NULL_DATA;
    }
    auto formartNames = pData->formats();
    QVector<UINT32> formatIds;

    if(pData->hasUrls())
    {
        formartNames << CClipboardMimeData::TypeFileGroupDescriptorW;
        formartNames << "FileContents";
        formartNames << CClipboardMimeData::MimeUriList;
    }
    if(pData->hasImage())
    {
        if(!formatIds.contains(CF_BITMAP))
        {
            formatIds.push_back(CF_BITMAP);
        }
        if(!formatIds.contains(CF_DIB))
        {
            formatIds.push_back(CF_DIB);
        }
        if(!formatIds.contains(CF_DIBV5))
        {
            formatIds.push_back(CF_DIBV5);
        }
#if FREERDP_VERSION_MAJOR > 3 || (FREERDP_VERSION_MAJOR == 3 && FREERDP_VERSION_MINOR >=3)
        if (winpr_image_format_is_supported(WINPR_IMAGE_BITMAP))
        {
            formartNames << CClipboardMimeData::MimeBmp
                         << CClipboardMimeData::MimeXBmp
                         << CClipboardMimeData::MimeXMsBmp
                         << CClipboardMimeData::MimeXWinBitmap;
        }
        if (winpr_image_format_is_supported(WINPR_IMAGE_JPEG)) {
            formartNames << CClipboardMimeData::MimeJpeg;
        }
        if (winpr_image_format_is_supported(WINPR_IMAGE_WEBP)) {
            formartNames << CClipboardMimeData::MimeWebp;
        }
        if (winpr_image_format_is_supported(WINPR_IMAGE_PNG)) {
            formartNames << CClipboardMimeData::MimePng;
        }
#endif
    }
    if(pData->hasHtml())
    {
        formartNames << CClipboardMimeData::MimeHtml
                     << CClipboardMimeData::TypeHtmlFormat;
    }
    if(pData->hasText())
    {
        if(!formatIds.contains(CF_TEXT))
        {
            formatIds.push_back(CF_TEXT);
        }
        if(!formatIds.contains(CF_OEMTEXT))
        {
            formatIds.push_back(CF_OEMTEXT);
        }
        if(!formatIds.contains(CF_UNICODETEXT))
        {
            formatIds.push_back(CF_UNICODETEXT);
        }
        if(!formatIds.contains(CF_LOCALE))
        {
            formatIds.push_back(CF_LOCALE);
        }
    }

#if HAVE_FILE_INTERFACE
    if (cliprdr_file_context_notify_new_client_format_list(pThis->m_pFileContext) != CHANNEL_RC_OK) {
        return ERROR_INTERNAL_ERROR;
    }
#endif

    CLIPRDR_FORMAT* pFormats = nullptr;
    CLIPRDR_FORMAT_LIST formatList;
    memset(&formatList, 0, sizeof(CLIPRDR_FORMAT_LIST));
    int nLen = formartNames.length() + formatIds.length();
    if(nLen > 0) {
        pFormats = new CLIPRDR_FORMAT[nLen];
    } else {
        qWarning(log) << "Don't include any format in clipboard";
        return CHANNEL_RC_NULL_DATA;
    }
    if(!pFormats)
    {
        qCritical(log) << "Failed to allocate"
                       << nLen << "CLIPRDR_FORMAT structs";
        return CHANNEL_RC_NO_MEMORY;
    }
    memset(pFormats, 0, sizeof(CLIPRDR_FORMAT) * nLen);

    UINT32 nNumFormats = 0;
    foreach(auto id, formatIds) {
        pFormats[nNumFormats].formatId = id;
        nNumFormats++;
    }
    qDebug(log) << "Clipboard formats:" << formartNames;
    foreach(auto f, formartNames) {
        if(f.isEmpty()) continue;
        UINT32 id = ClipboardRegisterFormat(
            pThis->m_pClipboard, f.toStdString().c_str());
        if(!formatIds.contains(id))
        {
            pFormats[nNumFormats].formatName = _strdup(f.toStdString().c_str());
            pFormats[nNumFormats].formatId = id;
            nNumFormats++;
        }
    }

    //*
    QString szFormats;
    for(int i = 0; i < nNumFormats; i++)
    {
        szFormats += " id:" + QString::number(pFormats[i].formatId);
        if(pFormats[i].formatName)
        {
            szFormats += "(";
            szFormats += pFormats[i].formatName;
            szFormats += ");";
        }
        szFormats += "\n";
    }
    qDebug(log, "SendClientFormatList formats: %d:%s",
           nNumFormats,
           szFormats.toStdString().c_str());//*/
#if FREERDP_VERSION_MAJOR >= 3
    formatList.common.msgFlags = CB_RESPONSE_OK;
    formatList.common.msgType = CB_FORMAT_LIST;
#else
    formatList.msgFlags = CB_RESPONSE_OK;
    formatList.msgType = CB_FORMAT_LIST;
#endif
    formatList.numFormats = nNumFormats;
    formatList.formats = pFormats;

    /* Ensure all pending requests are answered. */
    std::ignore = SendFormatDataResponse(context, NULL, 0);

    Q_ASSERT(context->ClientFormatList);
    nRet = context->ClientFormatList(context, &formatList);
    qDebug(log) << "SendClientFormatList nRet:" << nRet;

    for(UINT32 i = 0; i < nNumFormats; i++)
        if(pFormats[i].formatName)
        {
            //qDebug(log) << pFormats[i].formatName;
            free(pFormats[i].formatName);
        }
    delete []pFormats;

    return nRet;
}

///////// Send format data from client to server ///////////
UINT CClipboardFreeRDP::cbServerFormatDataRequest(
    CliprdrClientContext* context,
    const CLIPRDR_FORMAT_DATA_REQUEST* formatDataRequest)
{
    qDebug(log) << Q_FUNC_INFO;

    int nRet = CHANNEL_RC_OK;
    if(!context || !formatDataRequest)
        return CHANNEL_RC_NOT_INITIALIZED;

    bool bRet = false;
    QString mimeType;
    BYTE* pDstData = NULL;
    UINT32 dstSize = 0;

    CClipboardFreeRDP* pThis = GetThis(context);
    if(!pThis) return CHANNEL_RC_NOT_INITIALIZED;

    QClipboard *clipboard = QApplication::clipboard();
    if(!clipboard) return CHANNEL_RC_NULL_DATA;
    
    const UINT32 fileFormatId = ClipboardGetFormatId(
        pThis->m_pClipboard,
        CClipboardMimeData::TypeFileGroupDescriptorW.toStdString().c_str());
    const UINT32 htmlFormatId = ClipboardGetFormatId(
        pThis->m_pClipboard,
        CClipboardMimeData::TypeHtmlFormat.toStdString().c_str());
    UINT32 dstFormatId = formatDataRequest->requestedFormatId;
    UINT32 srcFormatID = 0;
    qDebug(log) << "server format date request formatID:" << dstFormatId;
    switch(dstFormatId)
    {
    case CF_TEXT:
    case CF_OEMTEXT:
    case CF_UNICODETEXT:
    case CF_LOCALE:
    {
        if(clipboard->mimeData()->hasText())
        {
            QString szData = clipboard->text();
            bRet = ClipboardSetData(
                pThis->m_pClipboard, CF_UNICODETEXT,
                szData.data(), (szData.size() + 1) * sizeof(QChar));
        }
        break;
    }
    case CF_DIB:
    case CF_DIBV5:
    //case CF_BITMAP:
        if(clipboard->mimeData()->hasImage())
        {
            QImage img = clipboard->image();
            if(img.isNull())
                break;
            QByteArray d;
            QBuffer buffer(&d);
            if(buffer.open(QIODevice::WriteOnly))
            {
                img.save(&buffer, "BMP");
                buffer.close();
            }
            if(!d.isEmpty())
                bRet = ClipboardSetData(
                    pThis->m_pClipboard,
                    ClipboardGetFormatId(
                        pThis->m_pClipboard,
                        CClipboardMimeData::MimeBmp.toStdString().c_str()),
                    (BYTE*)d.data(), d.length());
        }
        break;
    default:
        if(htmlFormatId == dstFormatId)
            mimeType = CClipboardMimeData::MimeHtml;
        else if(fileFormatId == dstFormatId)
            mimeType = CClipboardMimeData::MimeUriList;
        else
            mimeType = ClipboardGetFormatName(pThis->m_pClipboard, dstFormatId);

        if(!mimeType.isEmpty())
        {
            srcFormatID = ClipboardGetFormatId(
                pThis->m_pClipboard, mimeType.toStdString().c_str());
            if(srcFormatID > 0) {
                QByteArray data = clipboard->mimeData()->data(mimeType);
                qDebug(log) << "mimeData:" << data << data.length();
                if(!data.isEmpty())
                    bRet = ClipboardSetData(pThis->m_pClipboard, srcFormatID,
                                            data.data(), data.size());
            }
        }
        break;
    }

    if(bRet)
        pDstData = (BYTE*)ClipboardGetData(pThis->m_pClipboard, dstFormatId, &dstSize);

    if(!pDstData)
    {
        qCritical(log) << "ClipboardGetData fail: dstFormatId:" << dstFormatId
                       << ", srcFormatID:" << srcFormatID;
        nRet = SendFormatDataResponse(context, NULL, 0);
        return nRet;
    }

    /*
	 * File lists require a bit of postprocessing to convert them from WinPR's FILDESCRIPTOR
	 * format to CLIPRDR_FILELIST expected by the server.
	 *
	 * We check for "FileGroupDescriptorW" format being registered (i.e., nonzero) in order
	 * to not process CF_RAW as a file list in case WinPR does not support file transfers.
     *
     * See: [MS-RDPECLIP] 2.2.5.2.3 Packed File List (CLIPRDR_FILELIST)
     * See: https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpeclip/3570c2e4-cdd7-4460-8a7e-1a4595f5ebdc
	 */
    if(fileFormatId == dstFormatId)
    {
        UINT32 flags = 0;
#if HAVE_FILE_INTERFACE
        flags = cliprdr_file_context_remote_get_flags(pThis->m_pFileContext);
        if (cliprdr_file_context_update_client_data(
                pThis->m_pFileContext, (const char*)pDstData, (size_t)dstSize))
#else
        flags = pThis->m_FileCapabilityFlags;
#endif
        {
            UINT error = NO_ERROR;
            FILEDESCRIPTORW* file_array = (FILEDESCRIPTORW*)pDstData;
            UINT32 file_count = dstSize / sizeof(FILEDESCRIPTORW);
            error = cliprdr_serialize_file_list_ex(
                flags, file_array,
                file_count, &pDstData, &dstSize);
            if (error)
                qCritical(log) << "failed to serialize CLIPRDR_FILELIST:" << error;
            free(file_array);
        }
    }

    nRet = SendFormatDataResponse(context, pDstData, dstSize);

    if(pDstData)
        free(pDstData);

    /*
    qDebug(log) << "cb_cliprdr_server_format_data_request end. nRet:" << nRet;//*/

    return nRet;
}

UINT CClipboardFreeRDP::SendFormatDataResponse(CliprdrClientContext *context,
                                               const BYTE *data, size_t size)
{
    qDebug(log) << Q_FUNC_INFO;
    CLIPRDR_FORMAT_DATA_RESPONSE response = { 0 };
#if FREERDP_VERSION_MAJOR >= 3
    response.common.msgFlags = (data) ? CB_RESPONSE_OK : CB_RESPONSE_FAIL;
    response.common.dataLen = size;
#else
    response.msgFlags = (data) ? CB_RESPONSE_OK : CB_RESPONSE_FAIL;
    response.dataLen = size;
#endif
    response.requestedFormatData = data;
    return context->ClientFormatDataResponse(context, &response);
}

///////// Server to client ///////////
UINT CClipboardFreeRDP::cbServerFormatList(
    CliprdrClientContext* context,
    const CLIPRDR_FORMAT_LIST* formatList)
{
    qDebug(log) << Q_FUNC_INFO;
    UINT nRet = CHANNEL_RC_OK;
    if(!context || !formatList) return CHANNEL_RC_NOT_INITIALIZED;
    CClipboardFreeRDP* pThis = GetThis(context);
    if(formatList->numFormats < 0)
    {
        return nRet;
    }

#if HAVE_FILE_INTERFACE
    nRet = cliprdr_file_context_notify_new_server_format_list(pThis->m_pFileContext);
    if (nRet != CHANNEL_RC_OK)
        return nRet;
#endif

    // The pMimeData is freed by QApplication::clipboard()
    auto pMimeData = new CClipboardMimeData(context);
    if(!pMimeData) return CHANNEL_RC_NO_BUFFER;
    nRet = pMimeData->SetFormat(formatList);
    if(nRet)
        pMimeData->deleteLater();
    else {
        bool check = false;
        check = connect(pThis,
                        SIGNAL(sigServerFormatData(const BYTE*, UINT32, UINT32, QString)),
                        pMimeData,
                        SLOT(slotServerFormatData(const BYTE*, UINT32, UINT32, QString)),
                        Qt::DirectConnection);
        Q_ASSERT(check);
        check = connect(pThis,
                        SIGNAL(sigServerFileContentsRespose(UINT32, QByteArray&)),
                        pMimeData,
                        SLOT(slotServerFileContentsRespose(UINT32, QByteArray&)),
                        Qt::DirectConnection);
        Q_ASSERT(check);
        check = connect(pMimeData,
                        SIGNAL(sigSendDataRequest(CliprdrClientContext*, UINT32, QString)),
                        pThis,
                        SLOT(slotSendFormatDataRequest(CliprdrClientContext*, UINT32, QString)));
        Q_ASSERT(check);
    }

    pThis->m_lstClipboardMimeDataId.push_back(pMimeData->GetId());
    emit pThis->m_pConnect->sigSetClipboard(pMimeData);

    nRet = pThis->SendServerFormatListResponse(0 == nRet);
    return nRet;
}

UINT CClipboardFreeRDP::SendServerFormatListResponse(BOOL status)
{
    CLIPRDR_FORMAT_LIST_RESPONSE formatListResponse;
#if FREERDP_VERSION_MAJOR >= 3
    formatListResponse.common.msgType = CB_FORMAT_LIST_RESPONSE;
    formatListResponse.common.msgFlags = status ? CB_RESPONSE_OK : CB_RESPONSE_FAIL;
    formatListResponse.common.dataLen = 0;
#else
    formatListResponse.msgType = CB_FORMAT_LIST_RESPONSE;
    formatListResponse.msgFlags = status ? CB_RESPONSE_OK : CB_RESPONSE_FAIL;
    formatListResponse.dataLen = 0;
#endif
    Q_ASSERT(m_pCliprdrClientContext);
    Q_ASSERT(m_pCliprdrClientContext->ClientFormatListResponse);
    return m_pCliprdrClientContext->ClientFormatListResponse(m_pCliprdrClientContext, &formatListResponse);
}

UINT CClipboardFreeRDP::cbServerFormatListResponse(
    CliprdrClientContext* context,
    const CLIPRDR_FORMAT_LIST_RESPONSE* pformatListResponse)
{
#if FREERDP_VERSION_MAJOR >= 3
    qDebug(log)
        << "CClipboardFreeRdp::cb_cliprdr_server_format_list_response:type:"
        << pformatListResponse->common.msgType
        << ";flag:" << pformatListResponse->common.msgFlags
        << ";datalen:" << pformatListResponse->common.dataLen;
#else
    qDebug(log)
        << "CClipboardFreeRdp::cb_cliprdr_server_format_list_response:type:"
        << pformatListResponse->msgType
        << ";flag:" << pformatListResponse->msgFlags
        << ";datalen:" << pformatListResponse->dataLen;
#endif

    if (
#if FREERDP_VERSION_MAJOR >= 3
        pformatListResponse->common.msgFlags
#else
        pformatListResponse->msgFlags
#endif
        & CB_RESPONSE_FAIL)
        qCritical(log) << "The server is not support the format";
    return CHANNEL_RC_OK;
}

void CClipboardFreeRDP::slotSendFormatDataRequest(
    CliprdrClientContext* context,
    UINT32 formatId, QString szMimeType)
{
    qDebug(log) << Q_FUNC_INFO;

    if (!context || !context->ClientFormatDataRequest)
        return;

    CClipboardFreeRDP* pThis = GetThis(context);
    if(!pThis) return;

    CLIPRDR_FORMAT_DATA_REQUEST formatDataRequest;
    pThis->m_RequestFormatId = formatId;
    pThis->m_szRequestMime = szMimeType;
    formatDataRequest.requestedFormatId = formatId;
    context->ClientFormatDataRequest(context, &formatDataRequest);

    return;
}

UINT CClipboardFreeRDP::cbServerFormatDataResponse(
    CliprdrClientContext* context,
    const CLIPRDR_FORMAT_DATA_RESPONSE* formatDataResponse)
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = CHANNEL_RC_OK;
    if(!context || !formatDataResponse) return CHANNEL_RC_NOT_INITIALIZED;

    CClipboardFreeRDP* pThis = GetThis(context);
    if(!pThis) return CHANNEL_RC_NOT_INITIALIZED;
    emit pThis->sigServerFormatData(formatDataResponse->requestedFormatData,
#if FREERDP_VERSION_MAJOR >= 3
                                    formatDataResponse->common.dataLen,
#else
                                    formatDataResponse->dataLen,
#endif
                                    pThis->m_RequestFormatId, pThis->m_szRequestMime);
    return nRet;
}

#ifndef HAVE_FILE_INTERFACE
///////// Send file from client to server ///////////
UINT CClipboardFreeRDP::cb_clipboard_file_size_success(
    wClipboardDelegate* delegate,
    const wClipboardFileSizeRequest* request,
    UINT64 fileSize)
{
    qDebug(log) << "CClipboardFreeRDP::cb_clipboard_file_size_success";
    CLIPRDR_FILE_CONTENTS_RESPONSE response = { 0 };
    CClipboardFreeRDP* pThis = (CClipboardFreeRDP*)delegate->custom;
#if FREERDP_VERSION_MAJOR >= 3
    response.common.msgFlags = CB_RESPONSE_OK;
#else
    response.msgFlags = CB_RESPONSE_OK;
#endif
    response.streamId = request->streamId;
    response.cbRequested = sizeof(UINT64);
    response.requestedData = (BYTE*)&fileSize;
    return pThis->m_pCliprdrClientContext->ClientFileContentsResponse(
        pThis->m_pCliprdrClientContext, &response);
}

UINT CClipboardFreeRDP::cb_clipboard_file_size_failure(
    wClipboardDelegate* delegate,
    const wClipboardFileSizeRequest* request,
    UINT errorCode)
{
    qDebug(log) << "CClipboardFreeRDP::cb_clipboard_file_size_failure";
    CLIPRDR_FILE_CONTENTS_RESPONSE response = { 0 };
    CClipboardFreeRDP* pThis = (CClipboardFreeRDP*)delegate->custom;
    WINPR_UNUSED(errorCode);
#if FREERDP_VERSION_MAJOR >= 3
    response.common.msgFlags = CB_RESPONSE_FAIL;
#else
    response.msgFlags = CB_RESPONSE_FAIL;
#endif
    response.streamId = request->streamId;
    return pThis->m_pCliprdrClientContext->ClientFileContentsResponse(
        pThis->m_pCliprdrClientContext, &response);
}

UINT CClipboardFreeRDP::cb_clipboard_file_range_success(
    wClipboardDelegate* delegate,
    const wClipboardFileRangeRequest* request,
    const BYTE* data, UINT32 size)
{
    qDebug(log) << "CClipboardFreeRDP::cb_clipboard_file_range_success";
    CLIPRDR_FILE_CONTENTS_RESPONSE response = { 0 };
    CClipboardFreeRDP* pThis = (CClipboardFreeRDP*)delegate->custom;
#if FREERDP_VERSION_MAJOR >= 3
    response.common.msgFlags = CB_RESPONSE_OK;
#else
    response.msgFlags = CB_RESPONSE_OK;
#endif
    response.streamId = request->streamId;
    response.cbRequested = size;
    response.requestedData = (BYTE*)data;
    return pThis->m_pCliprdrClientContext->ClientFileContentsResponse(
        pThis->m_pCliprdrClientContext, &response);
}

UINT CClipboardFreeRDP::cb_clipboard_file_range_failure(
    wClipboardDelegate* delegate,
    const wClipboardFileRangeRequest* request,
    UINT errorCode)
{
    qDebug(log) << "CClipboardFreeRDP::cb_clipboard_file_range_failure";
    CLIPRDR_FILE_CONTENTS_RESPONSE response = { 0 };
    CClipboardFreeRDP* pThis = (CClipboardFreeRDP*)delegate->custom;
    WINPR_UNUSED(errorCode);
#if FREERDP_VERSION_MAJOR >= 3
    response.common.msgFlags = CB_RESPONSE_FAIL;
#else
    response.msgFlags = CB_RESPONSE_FAIL;
#endif
    response.streamId = request->streamId;
    return pThis->m_pCliprdrClientContext->ClientFileContentsResponse(
        pThis->m_pCliprdrClientContext, &response);
}

BOOL CClipboardFreeRDP::cbIsFileNameComponentValid(LPCWSTR lpFileName)
{
    qDebug(log) << "CClipboardFreeRDP::cbIsFileNameComponentValid:" << lpFileName;
    LPCWSTR c;

    if (!lpFileName)
        return FALSE;

    if (lpFileName[0] == L'\0')
        return FALSE;

    /* Reserved characters */
    for (c = lpFileName; *c; ++c)
    {
        if (*c == L'/')
            return FALSE;
    }

    return TRUE;
}

UINT CClipboardFreeRDP::SendFileContentsFailure(
    CliprdrClientContext* context,
    const CLIPRDR_FILE_CONTENTS_REQUEST* fileContentsRequest)
{
    qDebug(log) << Q_FUNC_INFO;
    CLIPRDR_FILE_CONTENTS_RESPONSE response = { 0 };
#if FREERDP_VERSION_MAJOR >= 3
    response.common.msgFlags = CB_RESPONSE_FAIL;
#else
    response.msgFlags = CB_RESPONSE_FAIL;
#endif
    response.streamId = fileContentsRequest->streamId;
    return context->ClientFileContentsResponse(context, &response);
}

UINT CClipboardFreeRDP::ServerFileRangeRequest(
    const CLIPRDR_FILE_CONTENTS_REQUEST* fileContentsRequest)
{
    qDebug(log) << Q_FUNC_INFO;
    wClipboardFileRangeRequest request = { 0 };
    request.streamId = fileContentsRequest->streamId;
    request.listIndex = fileContentsRequest->listIndex;
    request.nPositionLow = fileContentsRequest->nPositionLow;
    request.nPositionHigh = fileContentsRequest->nPositionHigh;
    request.cbRequested = fileContentsRequest->cbRequested;
    wClipboardDelegate* pDelegate = ClipboardGetDelegate(m_pClipboard);
    if(pDelegate)
        return pDelegate->ClientRequestFileRange(pDelegate, &request);
    return E_FAIL;
}

UINT CClipboardFreeRDP::ServerFileSizeRequest(
    const CLIPRDR_FILE_CONTENTS_REQUEST* fileContentsRequest)
{
    qDebug(log) << Q_FUNC_INFO;
    wClipboardFileSizeRequest request = { 0 };
    request.streamId = fileContentsRequest->streamId;
    request.listIndex = fileContentsRequest->listIndex;

    wClipboardDelegate* pDelegate = ClipboardGetDelegate(m_pClipboard);
    if(!pDelegate) return E_FAIL;

    if (fileContentsRequest->cbRequested != sizeof(UINT64))
    {
        qWarning(log) << "unexpected FILECONTENTS_SIZE request:"
                      << fileContentsRequest->cbRequested << " bytes";
    }

    return pDelegate->ClientRequestFileSize(pDelegate, &request);
}

UINT CClipboardFreeRDP::cb_cliprdr_server_file_contents_request(
    CliprdrClientContext* context,
    const CLIPRDR_FILE_CONTENTS_REQUEST* fileContentsRequest)
{
    qDebug(log) <<
        "CClipboardFreeRdp::cb_cliprdr_server_file_contents_request";
    int nRet = CHANNEL_RC_OK;

    UINT error = NO_ERROR;

    if(!context) return nRet;

    CClipboardFreeRDP* pThis = GetThis(context);

    /*
     * MS-RDPECLIP 2.2.5.3 File Contents Request PDU (CLIPRDR_FILECONTENTS_REQUEST):
     * The FILECONTENTS_SIZE and FILECONTENTS_RANGE flags MUST NOT be set at the same time.
     */
    if ((fileContentsRequest->dwFlags
         & (FILECONTENTS_SIZE | FILECONTENTS_RANGE))
        == (FILECONTENTS_SIZE | FILECONTENTS_RANGE))
    {
        qCritical(log) << "invalid CLIPRDR_FILECONTENTS_REQUEST.dwFlags";
        return SendFileContentsFailure(context, fileContentsRequest);
    }

    if (fileContentsRequest->dwFlags & FILECONTENTS_SIZE)
        error = pThis->ServerFileSizeRequest(fileContentsRequest);

    if (fileContentsRequest->dwFlags & FILECONTENTS_RANGE)
        error = pThis->ServerFileRangeRequest(fileContentsRequest);

    if (error)
    {
        qCritical(log)
        << "failed to handle CLIPRDR_FILECONTENTS_REQUEST:" << error;
        return SendFileContentsFailure(context, fileContentsRequest);
    }

    return nRet;
}

UINT CClipboardFreeRDP::cb_cliprdr_server_file_contents_response(
    CliprdrClientContext* context,
    const CLIPRDR_FILE_CONTENTS_RESPONSE* fileContentsResponse)
{
    qDebug(log) << "CClipboardFreeRdp::cb_cliprdr_server_file_contents_response";
    int nRet = CHANNEL_RC_OK;

    if (!context || !fileContentsResponse)
        return ERROR_INTERNAL_ERROR;

    if (
#if FREERDP_VERSION_MAJOR >= 3
        fileContentsResponse->common.msgFlags
#else
        fileContentsResponse->msgFlags
#endif
        != CB_RESPONSE_OK)
    {
        qDebug(log) << "File contents response error";
        return nRet;
    }

    CClipboardFreeRDP* pThis = GetThis(context);
    if(0 == fileContentsResponse->cbRequested)
    {
        qDebug(log) << "CClipboardFreeRdp::cb_cliprdr_server_file_contents_response size is zero.";
        QByteArray data;
        emit pThis->sigServerFileContentsRespose(
            fileContentsResponse->streamId,
            data);
    } else {
        QByteArray data((char*)fileContentsResponse->requestedData,
                        fileContentsResponse->cbRequested);
        emit pThis->sigServerFileContentsRespose(
            fileContentsResponse->streamId,
            data);
    }
    return nRet;
}
#endif