// Author: Kang Lin <kl222@126.com>
// See: https://github.com/KangLin/Documents/blob/master/qt/clipboard.md

#include "ClipboardFreeRDP.h"
#include <QClipboard>
#include <QApplication>
#include <QMimeData>
#include <QtDebug>
#include <QImage>
#include <QBuffer>
#include <QStandardPaths>
#include <QDir>

#include "ConnectFreeRDP.h"
#include "ClipboardMimeData.h"
#include "winpr/wlog.h"

#define WINPR_TAG(tag) "com.winpr." tag
#define TAG WINPR_TAG("clipboard")

Q_LOGGING_CATEGORY(FreeRDPClipboard, "FreeRDP.Clipboard")

CClipboardFreeRDP::CClipboardFreeRDP(CConnectFreeRDP *parent) : QObject(parent),
    m_pConnect(parent),
    m_pCliprdrClientContext(nullptr),
    m_pClipboard(nullptr),
    m_FileCapabilityFlags(0),
    m_bFileSupported(false),
    m_bFileFormatsRegistered(false)
{
    /*
    auto log = WLog_Get(TAG);
    WLog_SetLogLevel(log, WLOG_DEBUG);
    qDebug(FreeRDPClipboard) << "log level:" << WLog_GetLogLevel(log)
                             << ";IsLevelActive:"
                             << WLog_IsLevelActive(log, WLOG_DEBUG);//*/

    m_pClipboard = ClipboardCreate();
    if (ClipboardGetFormatId(m_pClipboard, "text/uri-list"))
        m_bFileFormatsRegistered = true;
    wClipboardDelegate* pDelegate = ClipboardGetDelegate(m_pClipboard);
    pDelegate->custom = this;
    /* Set up a filesystem base path for local URI */
    QString szPath = QStandardPaths::writableLocation(
                QStandardPaths::TempLocation)
            + QDir::separator() + "Rabbit"
            + QDir::separator() + "RabbitRemoteControl";
    qDebug(FreeRDPClipboard) << "Delegate base path:" << szPath;

	pDelegate->basePath = _strdup(szPath.toStdString().c_str());
    pDelegate->ClipboardFileSizeSuccess = cb_clipboard_file_size_success;
    pDelegate->ClipboardFileSizeFailure = cb_clipboard_file_size_failure;
    pDelegate->ClipboardFileRangeSuccess = cb_clipboard_file_range_success;
    pDelegate->ClipboardFileRangeFailure = cb_clipboard_file_range_failure;
    
#if FreeRDP_VERSION_MAJOR > 2 || (FreeRDP_VERSION_MAJOR == 2 && FreeRDP_VERSION_MINOR > 7)
    pDelegate->IsFileNameComponentValid = cbIsFileNameComponentValid;
#endif
}

CClipboardFreeRDP::~CClipboardFreeRDP()
{
    qDebug(FreeRDPClipboard) << "CClipboardFreeRdp::~CClipboardFreeRdp()";
    
    // Notify clipboard program has exited
    emit sigServerFormatData(nullptr, 0, 0);
    QByteArray data;
    emit sigServerFileContentsRespose(-1, data);
    ClipboardDestroy(m_pClipboard);
}

int CClipboardFreeRDP::Init(CliprdrClientContext *context, bool bEnable)
{
    m_pCliprdrClientContext = context;
    context->custom = this;
    if(!bEnable) return 0;
    // See: [MS_RDPECLIP] 1.3.2.1 Initialization Sequence
    // https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpeclip/a5cae3c9-170c-4154-992d-9ac8a149cc7e
    context->ServerCapabilities = cb_cliprdr_server_capabilities;
    context->MonitorReady = cb_cliprdr_monitor_ready;
	context->ServerFormatList = cb_cliprdr_server_format_list;
	context->ServerFormatListResponse = cb_cliprdr_server_format_list_response;
	context->ServerFormatDataRequest = cb_cliprdr_server_format_data_request;
	context->ServerFormatDataResponse = cb_cliprdr_server_format_data_response;
	context->ServerFileContentsRequest = cb_cliprdr_server_file_contents_request;
    context->ServerFileContentsResponse = cb_cliprdr_server_file_contents_response;
    return 0;
}

int CClipboardFreeRDP::UnInit(CliprdrClientContext *context, bool bEnable)
{
    context->custom = nullptr;
    m_pCliprdrClientContext = nullptr;
    return 0;
}

void CClipboardFreeRDP::slotClipBoardChanged()
{
    qDebug(FreeRDPClipboard) << "CClipboardFreeRdp::slotClipBoardChanged";
    // Whether it is the clipboard's QMimeData set by this connection
    const QMimeData* pMimeType = QApplication::clipboard()->mimeData();
    if(!pMimeType) return;

    qint32 data = 0;
    QVariant d = pMimeType->data(MIME_TYPE_RABBITREMOTECONTROL_PLUGINS_FREERDP);
    if(d.isValid()) {
        data = d.toInt();
        if(!m_lstClipboardMimeDataId.isEmpty()
                && m_lstClipboardMimeDataId.contains(data))
        {//*
            qDebug(FreeRDPClipboard)
                    << "CClipboardFreeRdp::slotClipBoardChanged: clipboard is this owner"
                    << data << m_lstClipboardMimeDataId;//*/
            return;
        }
    }

    //*
    qDebug(FreeRDPClipboard) << "CClipboardFreeRdp::slotClipBoardChanged:"
                             << data << m_lstClipboardMimeDataId;//*/
    m_lstClipboardMimeDataId.clear();
    SendClientFormatList(m_pCliprdrClientContext);
}

UINT CClipboardFreeRDP::cb_cliprdr_server_capabilities(
        CliprdrClientContext* context,
        const CLIPRDR_CAPABILITIES* capabilities)
{
    qDebug(FreeRDPClipboard) << "CClipboardFreeRdp::cb_cliprdr_server_capabilities";

    int nRet = CHANNEL_RC_OK;

    CClipboardFreeRDP* pThis = (CClipboardFreeRDP*)context->custom;
    pThis->m_bFileSupported = FALSE;
    const CLIPRDR_CAPABILITY_SET* caps;
	const CLIPRDR_GENERAL_CAPABILITY_SET* generalCaps;
	const BYTE* capsPtr = (const BYTE*)capabilities->capabilitySets;

	for (UINT32 i = 0; i < capabilities->cCapabilitiesSets; i++)
	{
		caps = (const CLIPRDR_CAPABILITY_SET*)capsPtr;

		if (caps->capabilitySetType == CB_CAPSTYPE_GENERAL)
		{
			generalCaps = (const CLIPRDR_GENERAL_CAPABILITY_SET*)caps;

			if (generalCaps->generalFlags & CB_STREAM_FILECLIP_ENABLED)
			{
				// Support file clipboard
                pThis->m_bFileSupported = TRUE;
			}
		}

		capsPtr += caps->capabilitySetLength;
	}
    return nRet;
}

///////// Send format list from client to server ///////////

UINT CClipboardFreeRDP::cb_cliprdr_monitor_ready(CliprdrClientContext *context,
                                      const CLIPRDR_MONITOR_READY *monitorReady)
{
    qDebug(FreeRDPClipboard) << "CClipboardFreeRdp::cb_cliprdr_monitor_ready";
    UINT nRet = CHANNEL_RC_OK;

    if (!context || !context->ClientCapabilities)
	{
        Q_ASSERT(false);
        return ERROR_INTERNAL_ERROR;
    }
    CClipboardFreeRDP* pThis = (CClipboardFreeRDP*)context->custom;

    // Send client capabilities
    CLIPRDR_CAPABILITIES capabilities;
	CLIPRDR_GENERAL_CAPABILITY_SET generalCapabilitySet;
	capabilities.cCapabilitiesSets = 1;
	capabilities.capabilitySets = (CLIPRDR_CAPABILITY_SET*)&(generalCapabilitySet);
	generalCapabilitySet.capabilitySetType = CB_CAPSTYPE_GENERAL;
	generalCapabilitySet.capabilitySetLength = 12;
	generalCapabilitySet.version = CB_CAPS_VERSION_2;
	generalCapabilitySet.generalFlags = CB_USE_LONG_FORMAT_NAMES;

	if (pThis->m_bFileSupported && pThis->m_bFileFormatsRegistered)
    {
        generalCapabilitySet.generalFlags |=
                CB_STREAM_FILECLIP_ENABLED | CB_FILECLIP_NO_FILE_PATHS
        #if FreeRDP_VERSION_MAJOR > 2 || (FreeRDP_VERSION_MAJOR == 2 && FreeRDP_VERSION_MINOR > 7)
                | CB_HUGE_FILE_SUPPORT_ENABLED
        #endif
                ;
    }

    pThis->m_FileCapabilityFlags = generalCapabilitySet.generalFlags;
	if((nRet = context->ClientCapabilities(context, &capabilities))
            != CHANNEL_RC_OK)
    {
        qCritical(FreeRDPClipboard) << "Send Client Capabilities fail";
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
    qDebug(FreeRDPClipboard) << "CClipboardFreeRdp::SendClientFormatList";
    int nRet = CHANNEL_RC_OK;
    int nLen = 0;
    CLIPRDR_FORMAT* pFormats = nullptr;
	CLIPRDR_FORMAT_LIST formatList = { 0 };
    UINT32 numFormats = 0;

    if(!context) return nRet;
    
    CClipboardFreeRDP* pThis = (CClipboardFreeRDP*)context->custom;
    QClipboard *clipboard = QApplication::clipboard();
    if(!clipboard)
    {
        qDebug(FreeRDPClipboard) << "clipboard is null";
        return nRet;
    }

    QVector<UINT32> formatIds;
    const QMimeData* pData = clipboard->mimeData();
    if(!pData || pData->formats().isEmpty())
    {
        qDebug(FreeRDPClipboard) << "clipboard->mimeData is null";
        return nRet;
    }
    auto clipFormats = pData->formats();
    if(!clipFormats.isEmpty())
    {
        nLen = clipFormats.length()
                + ClipboardCountRegisteredFormats(pThis->m_pClipboard);
        pFormats = new CLIPRDR_FORMAT[nLen];
    }
    if(!pFormats)
    {
        qCritical(FreeRDPClipboard) << "Failed to allocate"
                                  << nLen << "CLIPRDR_FORMAT structs";
        return CHANNEL_RC_NO_MEMORY;
    }
    memset(pFormats, 0, sizeof(CLIPRDR_FORMAT) * nLen);
    qDebug(FreeRDPClipboard) << "Clipboard formats:" << clipFormats;
    foreach(auto f, pData->formats())
    {
        UINT32 id = ClipboardRegisterFormat(
                    pThis->m_pClipboard, f.toStdString().c_str());
        if(!formatIds.contains(id))
        {
            pFormats[numFormats].formatName = _strdup(f.toStdString().c_str());
            pFormats[numFormats].formatId = id;
            formatIds.push_back(pFormats[numFormats].formatId);
            numFormats++;
        }
    }

    if(pData->hasUrls())
    {
        UINT32 id = ClipboardRegisterFormat(
                    pThis->m_pClipboard, "FileGroupDescriptorW");
        if(!formatIds.contains(id))
        {
            pFormats[numFormats].formatName = _strdup("FileGroupDescriptorW");
            pFormats[numFormats].formatId = id;
            formatIds.push_back(pFormats[numFormats].formatId);
            numFormats++;
        }
        id = ClipboardRegisterFormat(pThis->m_pClipboard, "FileContents");
        if(!formatIds.contains(id))
        {
            pFormats[numFormats].formatName = _strdup("FileContents");
            pFormats[numFormats].formatId = id;
            formatIds.push_back(pFormats[numFormats].formatId);
            numFormats++;
        }
        id = ClipboardRegisterFormat(pThis->m_pClipboard, "text/uri-list");
        if(!formatIds.contains(id))
        {
            pFormats[numFormats].formatName = _strdup("text/uri-list");
            pFormats[numFormats].formatId = id;
            formatIds.push_back(pFormats[numFormats].formatId);
            numFormats++;
        }
    }
    if(pData->hasImage())
    {
//        if(!formatIds.contains(CF_BITMAP))
//        {
//            pFormats[numFormats].formatId = CF_BITMAP;
//            pFormats[numFormats].formatName = nullptr;
//            formatIds.push_back(pFormats[numFormats].formatId);
//            numFormats++;
//        }
        if(!formatIds.contains(CF_DIB))
        {
            pFormats[numFormats].formatId = CF_DIB;
            pFormats[numFormats].formatName = nullptr;
            formatIds.push_back(pFormats[numFormats].formatId);
            numFormats++;
        }
        if(!formatIds.contains(CF_DIBV5))
        {
            pFormats[numFormats].formatId = CF_DIBV5;
            pFormats[numFormats].formatName = nullptr;
            formatIds.push_back(pFormats[numFormats].formatId);
            numFormats++;
        }
    }
    if(pData->hasHtml())
    {
        UINT32 id = ClipboardRegisterFormat(
                    pThis->m_pClipboard, "text/html");
        if(!formatIds.contains(id))
        {
            pFormats[numFormats].formatId = id;
            pFormats[numFormats].formatName = _strdup("text/html");
            formatIds.push_back(pFormats[numFormats].formatId);
            numFormats++;
        }
        id = ClipboardRegisterFormat(
                    pThis->m_pClipboard, "HTML Format");
        if(!formatIds.contains(id))
        {
            pFormats[numFormats].formatId = id;
            pFormats[numFormats].formatName = _strdup("HTML Format");
            formatIds.push_back(pFormats[numFormats].formatId);
            numFormats++;
        }
    }
    if(pData->hasText())
    {
        if(!formatIds.contains(CF_TEXT))
        {
            pFormats[numFormats].formatId = CF_TEXT;
            pFormats[numFormats].formatName = nullptr;
            formatIds.push_back(pFormats[numFormats].formatId);
            numFormats++;
        }
        if(!formatIds.contains(CF_OEMTEXT))
        {
            pFormats[numFormats].formatId = CF_OEMTEXT;
            pFormats[numFormats].formatName = nullptr;
            formatIds.push_back(pFormats[numFormats].formatId);
            numFormats++;
        }
        if(!formatIds.contains(CF_UNICODETEXT))
        {
            pFormats[numFormats].formatId = CF_UNICODETEXT;
            pFormats[numFormats].formatName = nullptr;
            formatIds.push_back(pFormats[numFormats].formatId);
            numFormats++;
        }
        if(!formatIds.contains(CF_LOCALE))
        {
            pFormats[numFormats].formatId = CF_LOCALE;
            pFormats[numFormats].formatName = nullptr;
            formatIds.push_back(pFormats[numFormats].formatId);
            numFormats++;
        }
    }
    /*
    QString szFormats;
    for(int i = 0; i < numFormats; i++)
    {
        szFormats += " id:" + QString::number(pFormats[i].formatId);
        if(pFormats[i].formatName)
        {
            szFormats += "(";
            szFormats += pFormats[i].formatName;
            szFormats += ");";
        }
    }
    qDebug(FreeRDPClipboard, "SendClientFormatList formats: %d:%s",
                    numFormats,
                    szFormats.toStdString().c_str());//*/
#if FreeRDP_VERSION_MAJOR >= 3
    formatList.common.msgFlags = CB_RESPONSE_OK;
    formatList.common.msgType = CB_FORMAT_LIST;
#else
    formatList.msgFlags = CB_RESPONSE_OK;
    formatList.msgType = CB_FORMAT_LIST;
#endif
	formatList.numFormats = numFormats;
	formatList.formats = pFormats;
	

    /* Ensure all pending requests are answered. */
	SendFormatDataResponse(context, NULL, 0);

    Q_ASSERT(context->ClientFormatList);
	nRet = context->ClientFormatList(context, &formatList);
    qDebug(FreeRDPClipboard) << "SendClientFormatList nRet:" << nRet;
    for(UINT32 i = 0; i < numFormats; i++)
        if(pFormats[i].formatName)
        {
            //qDebug() << pFormats[i].formatName;
            free(pFormats[i].formatName);
        }
    delete []pFormats;
    return nRet;
}

///////// Send format data from client to server ///////////

UINT CClipboardFreeRDP::cb_cliprdr_server_format_data_request(
        CliprdrClientContext* context,
        const CLIPRDR_FORMAT_DATA_REQUEST* formatDataRequest)
{
    qDebug(FreeRDPClipboard)
            << "CClipboardFreeRdp::cb_cliprdr_server_format_data_request";

    int nRet = CHANNEL_RC_OK;
    bool bSuucess = false;
    QString mimeType;
    BYTE* pDstData = NULL;
    UINT32 dstSize = 0;
    UINT32 dstFormatId = formatDataRequest->requestedFormatId;
    UINT32 scrFormatID = 0;
    if(!context) return ERROR_INTERNAL_ERROR;
    
    CClipboardFreeRDP* pThis = (CClipboardFreeRDP*)context->custom;
    qDebug(FreeRDPClipboard) << "server format date request formatID:"
                    << dstFormatId;
    QClipboard *clipboard = QApplication::clipboard();
    if(!clipboard) return ERROR_INTERNAL_ERROR;
    
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
            bSuucess = ClipboardSetData(pThis->m_pClipboard, CF_UNICODETEXT,
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
                bSuucess = ClipboardSetData(pThis->m_pClipboard,
                         ClipboardGetFormatId(pThis->m_pClipboard, "image/bmp"),
                                            (BYTE*)d.data(), d.length());
        }
        break;
    default:
        if(ClipboardGetFormatId(pThis->m_pClipboard, "HTML Format")
                == dstFormatId)
        {
            mimeType = "text/html";
            scrFormatID = ClipboardGetFormatId(pThis->m_pClipboard,
                                               "text/html");
        } else if(ClipboardGetFormatId(pThis->m_pClipboard,
                                       "FileGroupDescriptorW")
                == dstFormatId)
        {
            mimeType = "text/uri-list";
            scrFormatID = ClipboardGetFormatId(pThis->m_pClipboard,
                                               "text/uri-list");
        } else {
            mimeType = ClipboardGetFormatName(pThis->m_pClipboard, dstFormatId);
            scrFormatID = dstFormatId;
        }
        if(!mimeType.isEmpty() && scrFormatID > 0)
        {
            QByteArray data = clipboard->mimeData()->data(mimeType);
            qDebug(FreeRDPClipboard) << "mimeData:" << data << data.length();
            if(!data.isEmpty())
                bSuucess = ClipboardSetData(pThis->m_pClipboard, scrFormatID,
                                            data.data(), data.size());
        }
        break;
    }

    if(bSuucess)
        pDstData = (BYTE*)ClipboardGetData(pThis->m_pClipboard,
                                           dstFormatId, &dstSize);

    if(!pDstData)
    {
        qCritical(FreeRDPClipboard)
                << "ClipboardGetData fail: dstFormatId:" << dstFormatId
                << ", srcFormatId:" << scrFormatID;
        nRet = SendFormatDataResponse(context, NULL, 0);
        return nRet;
    }

    /*
	 * File lists require a bit of postprocessing to convert them from WinPR's FILDESCRIPTOR
	 * format to CLIPRDR_FILELIST expected by the server.
	 *
	 * We check for "FileGroupDescriptorW" format being registered (i.e., nonzero) in order
	 * to not process CF_RAW as a file list in case WinPR does not support file transfers.
	 */
    if(dstFormatId
       && (ClipboardGetFormatId(pThis->m_pClipboard, "FileGroupDescriptorW")
           == dstFormatId))
    {
        UINT error = NO_ERROR;
        FILEDESCRIPTORW* file_array = (FILEDESCRIPTORW*)pDstData;
        UINT32 file_count = dstSize / sizeof(FILEDESCRIPTORW);

        error = cliprdr_serialize_file_list_ex(
                    pThis->m_FileCapabilityFlags, file_array,
                    file_count, &pDstData, &dstSize);
        if (error)
            qCritical(FreeRDPClipboard)
                    << "failed to serialize CLIPRDR_FILELIST:" << error;
        free(file_array);
    }

    nRet = SendFormatDataResponse(context, pDstData, dstSize);

    if(pDstData)
        free(pDstData);

    /*
    qDebug(FreeRDPClipboard) <<
               "cb_cliprdr_server_format_data_request end. nRet:" << nRet;//*/

    return nRet;
}

UINT CClipboardFreeRDP::SendFormatDataResponse(CliprdrClientContext *context,
                                               const BYTE *data, size_t size)
{
    CLIPRDR_FORMAT_DATA_RESPONSE response = { 0 };
#if FreeRDP_VERSION_MAJOR >= 3
    response.common.msgFlags = (data) ? CB_RESPONSE_OK : CB_RESPONSE_FAIL;
    response.common.dataLen = size;
#else
	response.msgFlags = (data) ? CB_RESPONSE_OK : CB_RESPONSE_FAIL;
	response.dataLen = size;
#endif
	response.requestedFormatData = data;
	return context->ClientFormatDataResponse(context, &response);
}

///////// Send file from client to server ///////////

UINT CClipboardFreeRDP::cb_clipboard_file_size_success(
        wClipboardDelegate* delegate,
        const wClipboardFileSizeRequest* request,
        UINT64 fileSize)
{
    qDebug(FreeRDPClipboard) << "CClipboardFreeRDP::cb_clipboard_file_size_success";
	CLIPRDR_FILE_CONTENTS_RESPONSE response = { 0 };
	CClipboardFreeRDP* pThis = (CClipboardFreeRDP*)delegate->custom;
#if FreeRDP_VERSION_MAJOR >= 3
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
    qDebug(FreeRDPClipboard) << "CClipboardFreeRDP::cb_clipboard_file_size_failure";
	CLIPRDR_FILE_CONTENTS_RESPONSE response = { 0 };
	CClipboardFreeRDP* pThis = (CClipboardFreeRDP*)delegate->custom;
	WINPR_UNUSED(errorCode);
#if FreeRDP_VERSION_MAJOR >= 3
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
    qDebug(FreeRDPClipboard) << "CClipboardFreeRDP::cb_clipboard_file_range_success";
	CLIPRDR_FILE_CONTENTS_RESPONSE response = { 0 };
	CClipboardFreeRDP* pThis = (CClipboardFreeRDP*)delegate->custom;
#if FreeRDP_VERSION_MAJOR >= 3
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
    qDebug(FreeRDPClipboard) << "CClipboardFreeRDP::cb_clipboard_file_range_failure";
	CLIPRDR_FILE_CONTENTS_RESPONSE response = { 0 };
	CClipboardFreeRDP* pThis = (CClipboardFreeRDP*)delegate->custom;
	WINPR_UNUSED(errorCode);
#if FreeRDP_VERSION_MAJOR >= 3
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
    qDebug(FreeRDPClipboard) << "CClipboardFreeRDP::cbIsFileNameComponentValid: " << lpFileName;
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

UINT CClipboardFreeRDP::SendFileContentsFailure(CliprdrClientContext* context,
                  const CLIPRDR_FILE_CONTENTS_REQUEST* fileContentsRequest)
{
	CLIPRDR_FILE_CONTENTS_RESPONSE response = { 0 };
#if FreeRDP_VERSION_MAJOR >= 3
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
	wClipboardFileSizeRequest request = { 0 };
	request.streamId = fileContentsRequest->streamId;
	request.listIndex = fileContentsRequest->listIndex;

    wClipboardDelegate* pDelegate = ClipboardGetDelegate(m_pClipboard);
    if(!pDelegate) return E_FAIL;

	if (fileContentsRequest->cbRequested != sizeof(UINT64))
	{
		qWarning(FreeRDPClipboard) << "unexpected FILECONTENTS_SIZE request:"
                              << fileContentsRequest->cbRequested << " bytes";
	}

	return pDelegate->ClientRequestFileSize(pDelegate, &request);
}

UINT CClipboardFreeRDP::cb_cliprdr_server_file_contents_request(
        CliprdrClientContext* context,
        const CLIPRDR_FILE_CONTENTS_REQUEST* fileContentsRequest)
{
    qDebug(FreeRDPClipboard) <<
                  "CClipboardFreeRdp::cb_cliprdr_server_file_contents_request";
    int nRet = CHANNEL_RC_OK;

    UINT error = NO_ERROR;
    
    if(!context) return nRet;

    CClipboardFreeRDP* pThis = (CClipboardFreeRDP*)context->custom;
    
    /*
	 * MS-RDPECLIP 2.2.5.3 File Contents Request PDU (CLIPRDR_FILECONTENTS_REQUEST):
	 * The FILECONTENTS_SIZE and FILECONTENTS_RANGE flags MUST NOT be set at the same time.
	 */
	if ((fileContentsRequest->dwFlags
         & (FILECONTENTS_SIZE | FILECONTENTS_RANGE))
            == (FILECONTENTS_SIZE | FILECONTENTS_RANGE))
	{
		qCritical(FreeRDPClipboard) << "invalid CLIPRDR_FILECONTENTS_REQUEST.dwFlags";
		return SendFileContentsFailure(context, fileContentsRequest);
	}

	if (fileContentsRequest->dwFlags & FILECONTENTS_SIZE)
		error = pThis->ServerFileSizeRequest(fileContentsRequest);

	if (fileContentsRequest->dwFlags & FILECONTENTS_RANGE)
		error = pThis->ServerFileRangeRequest(fileContentsRequest);

	if (error)
	{
		qCritical(FreeRDPClipboard)
                << "failed to handle CLIPRDR_FILECONTENTS_REQUEST:" << error;
		return SendFileContentsFailure(context, fileContentsRequest);
	}

    return nRet;
}

///////// Server to client ///////////
UINT CClipboardFreeRDP::cb_cliprdr_server_format_list(
        CliprdrClientContext* context,
        const CLIPRDR_FORMAT_LIST* formatList)
{
    qDebug(FreeRDPClipboard) << "CClipboardFreeRdp::cb_cliprdr_server_format_list";
    int nRet = CHANNEL_RC_OK;
    
    CClipboardFreeRDP* pThis = (CClipboardFreeRDP*)context->custom;
    CClipboardMimeData* pMimeData = nullptr;
    if(formatList->numFormats < 0)
    {
        return nRet;
    }
    // The pMimeData is freed by QApplication::clipboard()
    pMimeData = new CClipboardMimeData(context);
    if(!pMimeData) return nRet;
    if(pMimeData->SetFormat(formatList))
    {
        pMimeData->deleteLater();
        return nRet;
    }

    bool check = false;
    check = connect(pThis,
                    SIGNAL(sigServerFormatData(const BYTE*, UINT32, UINT32)),
                    pMimeData,
                    SLOT(slotServerFormatData(const BYTE*, UINT32, UINT32)),
                    Qt::DirectConnection);
    Q_ASSERT(check);
    check = connect(pThis,
                    SIGNAL(sigServerFileContentsRespose(UINT32, QByteArray&)),
                    pMimeData,
                    SLOT(slotServerFileContentsRespose(UINT32, QByteArray&)),
                    Qt::DirectConnection);
    Q_ASSERT(check);
    check = connect(pMimeData,
                    SIGNAL(sigSendDataRequest(CliprdrClientContext*, UINT32)),
                    pThis,
                SLOT(slotSendFormatDataRequest(CliprdrClientContext*, UINT32)));
    Q_ASSERT(check);

    pThis->m_lstClipboardMimeDataId.push_back(pMimeData->GetId());
    emit pThis->m_pConnect->sigSetClipboard(pMimeData);
    return nRet;
}

UINT CClipboardFreeRDP::cb_cliprdr_server_format_list_response(
        CliprdrClientContext* context,
        const CLIPRDR_FORMAT_LIST_RESPONSE* pformatListResponse)
{
#if FreeRDP_VERSION_MAJOR >= 3
    qDebug(FreeRDPClipboard)
            << "CClipboardFreeRdp::cb_cliprdr_server_format_list_response:type:"
            << pformatListResponse->common.msgType
            << ";flag:" << pformatListResponse->common.msgFlags
            << ";datalen:" << pformatListResponse->common.dataLen;
#else
    qDebug(FreeRDPClipboard)
            << "CClipboardFreeRdp::cb_cliprdr_server_format_list_response:type:"
            << pformatListResponse->msgType
            << ";flag:" << pformatListResponse->msgFlags
            << ";datalen:" << pformatListResponse->dataLen;
#endif

    if (
        #if FreeRDP_VERSION_MAJOR >= 3
            pformatListResponse->common.msgFlags
        #else
            pformatListResponse->msgFlags
        #endif
            != CB_RESPONSE_OK)
        return E_FAIL;
    return CHANNEL_RC_OK;
}

UINT CClipboardFreeRDP::slotSendFormatDataRequest(CliprdrClientContext* context,
                                            UINT32 formatId)
{
	UINT rc = CHANNEL_RC_OK;
	CLIPRDR_FORMAT_DATA_REQUEST formatDataRequest;

	if (!context || !context->ClientFormatDataRequest)
		return ERROR_INTERNAL_ERROR;

    CClipboardFreeRDP* pThis = (CClipboardFreeRDP*)context->custom;
    pThis->m_RequestFormatId = formatId;
	formatDataRequest.requestedFormatId = formatId;
	rc = context->ClientFormatDataRequest(context, &formatDataRequest);

	return rc;
}

UINT CClipboardFreeRDP::cb_cliprdr_server_format_data_response(
        CliprdrClientContext* context,
        const CLIPRDR_FORMAT_DATA_RESPONSE* formatDataResponse)
{
    qDebug(FreeRDPClipboard) << "CClipboardFreeRdp::cb_cliprdr_server_format_data_response";
    int nRet = CHANNEL_RC_OK;
    if(!context) return nRet;

    CClipboardFreeRDP* pThis = (CClipboardFreeRDP*)context->custom;
   
    emit pThis->sigServerFormatData(formatDataResponse->requestedFormatData,
                                #if FreeRDP_VERSION_MAJOR >= 3
                                    formatDataResponse->common.dataLen,
                                #else
                                    formatDataResponse->dataLen,
                                #endif
                                    pThis->m_RequestFormatId);
    return nRet;
}

UINT CClipboardFreeRDP::cb_cliprdr_server_file_contents_response(
        CliprdrClientContext* context,
        const CLIPRDR_FILE_CONTENTS_RESPONSE* fileContentsResponse)
{
    qDebug(FreeRDPClipboard) << "CClipboardFreeRdp::cb_cliprdr_server_file_contents_response";
    int nRet = CHANNEL_RC_OK;

    if (!context || !fileContentsResponse)
		return ERROR_INTERNAL_ERROR;

	if (
        #if FreeRDP_VERSION_MAJOR >= 3
            fileContentsResponse->common.msgFlags
        #else
            fileContentsResponse->msgFlags
        #endif
            != CB_RESPONSE_OK)
		return E_FAIL;

    CClipboardFreeRDP* pThis = (CClipboardFreeRDP*)context->custom;
    if(0 == fileContentsResponse->cbRequested)
    {
        qDebug(FreeRDPClipboard) << "CClipboardFreeRdp::cb_cliprdr_server_file_contents_response size is zero.";
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
