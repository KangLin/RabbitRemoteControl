// Author: Kang Lin <kl222@126.com>

#include "ClipboardFreeRdp.h"
#include "RabbitCommonLog.h"
#include <QClipboard>
#include <QApplication>
#include <QMimeData>
#include <QtDebug>
#include <QImage>
#include <QBuffer>
#include "ConnectFreeRdp.h"
#include "ClipboardMimeData.h"

CClipboardFreeRdp::CClipboardFreeRdp(CConnectFreeRdp *parent) : QObject(parent),
    m_pConnect(parent),
    m_pCliprdrClientContext(nullptr),
    m_pClipboard(nullptr),
    m_bOwns(false),
    m_FileCapabilityFlags(0),
    m_bFileSupported(false)
{
    m_pClipboard = ClipboardCreate();
    wClipboardDelegate* pDelegate = ClipboardGetDelegate(m_pClipboard);
    pDelegate->custom = this;
    pDelegate->ClipboardFileSizeSuccess = cb_clipboard_file_size_success;
    pDelegate->ClipboardFileSizeFailure = cb_clipboard_file_size_failure;
    pDelegate->ClipboardFileRangeSuccess = cb_clipboard_file_range_success;
    pDelegate->ClipboardFileRangeFailure = cb_clipboard_file_range_failure;
}

CClipboardFreeRdp::~CClipboardFreeRdp()
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::~CClipboardFreeRdp()");
    
    // Notify clipboard program has exited
    emit sigServerFormatData(nullptr, 0, 0, "");
    ClipboardDestroy(m_pClipboard);
}

int CClipboardFreeRdp::Init(CliprdrClientContext *context, bool bEnable)
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

int CClipboardFreeRdp::UnInit(CliprdrClientContext *context, bool bEnable)
{
    context->custom = nullptr;
    m_pCliprdrClientContext = nullptr;
    return 0;
}

void CClipboardFreeRdp::slotClipBoardChange()
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::slotClipBoardChange");
    if(m_bOwns)
    {
        m_bOwns = false;
        return;
    }
    SendClientFormatList(m_pCliprdrClientContext);
}

UINT CClipboardFreeRdp::cb_cliprdr_server_capabilities(CliprdrClientContext* context,
                                           const CLIPRDR_CAPABILITIES* capabilities)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::cb_cliprdr_server_capabilities");
    int nRet = CHANNEL_RC_OK;

    CClipboardFreeRdp* pThis = (CClipboardFreeRdp*)context->custom;
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

UINT CClipboardFreeRdp::cb_cliprdr_monitor_ready(CliprdrClientContext *context,
                                      const CLIPRDR_MONITOR_READY *monitorReady)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::cb_cliprdr_monitor_ready");
    UINT nRet = CHANNEL_RC_OK;

    if (!context || !context->ClientCapabilities)
	{
        Q_ASSERT(false);
        return ERROR_INTERNAL_ERROR;
    }
    CClipboardFreeRdp* pThis = (CClipboardFreeRdp*)context->custom;

    // Send client capabilities
    CLIPRDR_CAPABILITIES capabilities;
	CLIPRDR_GENERAL_CAPABILITY_SET generalCapabilitySet;
	capabilities.cCapabilitiesSets = 1;
	capabilities.capabilitySets = (CLIPRDR_CAPABILITY_SET*)&(generalCapabilitySet);
	generalCapabilitySet.capabilitySetType = CB_CAPSTYPE_GENERAL;
	generalCapabilitySet.capabilitySetLength = 12;
	generalCapabilitySet.version = CB_CAPS_VERSION_2;
	generalCapabilitySet.generalFlags = CB_USE_LONG_FORMAT_NAMES;

	if (pThis->m_bFileSupported)
    {
		generalCapabilitySet.generalFlags |=
		    CB_STREAM_FILECLIP_ENABLED | CB_FILECLIP_NO_FILE_PATHS;
        #if QT_VERSION_CHECK(WINPR_VERSION_MAJOR, WINPR_VERSION_MINOR, WINPR_VERSION_REVISION) > QT_VERSION_CHECK(2, 4, 2)
                generalCapabilitySet.generalFlags |= CB_HUGE_FILE_SUPPORT_ENABLED;
        #endif
    }
    
    pThis->m_FileCapabilityFlags = generalCapabilitySet.generalFlags;
	if((nRet = context->ClientCapabilities(context, &capabilities)) != CHANNEL_RC_OK)
    {
        LOG_MODEL_ERROR("FreeRdp", "Send Client Capabilities fail");
        return nRet;
    }
    
    // Send client formats
    return SendClientFormatList(context);
}

// See: [MS_RDPECLIP] 1.3.2.2 Data Transfer Sequences
//      https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpeclip/395bc830-f2c2-40e5-a3f3-23e41183b777
// Clipboard Formats: https://docs.microsoft.com/en-us/windows/win32/dataxchg/clipboard-formats
UINT CClipboardFreeRdp::SendClientFormatList(CliprdrClientContext *context)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::SendClientFormatList");
    int nRet = CHANNEL_RC_OK;
    CLIPRDR_FORMAT* pFormats = nullptr;
	CLIPRDR_FORMAT_LIST formatList = { 0 };
    UINT32 numFormats = 0;

    if(!context) return nRet;
    
    CClipboardFreeRdp* pThis = (CClipboardFreeRdp*)context->custom;
    QClipboard *clipboard = QApplication::clipboard();
    if(!clipboard)
    {
        LOG_MODEL_DEBUG("FreeRdp", "clipboard is null");
        return nRet;
    }

    pThis->m_FormatIds.clear();
    const QMimeData* pData = clipboard->mimeData();
    if(!pData || pData->formats().isEmpty())
    {
        LOG_MODEL_DEBUG("FreeRdp", "clipboard->mimeData is null");
        return nRet;
    }
    auto clipFormats = pData->formats();
    if(!clipFormats.isEmpty())
        pFormats = new CLIPRDR_FORMAT[clipFormats.length() + CF_MAX];
    if(!pFormats)
    {
        LOG_MODEL_ERROR("FreeRdp", "failed to allocate %d CLIPRDR_FORMAT structs", clipFormats.length());
        return CHANNEL_RC_NO_MEMORY;
    }
    memset(pFormats, 0, sizeof(CLIPRDR_FORMAT) * (clipFormats.length() + CF_MAX));
    qDebug() << "Clipboard formats:" << clipFormats;
    foreach(auto f, pData->formats())
    {
        pFormats[numFormats].formatName = _strdup(f.toStdString().c_str());
        pFormats[numFormats].formatId = ClipboardRegisterFormat(
                    pThis->m_pClipboard, f.toStdString().c_str());
        pThis->m_FormatIds.push_back(pFormats[numFormats].formatId);
        numFormats++;
    }

    if(pData->hasUrls())
    {
        pFormats[numFormats].formatName = _strdup("FileGroupDescriptorW");
        pFormats[numFormats].formatId = ClipboardRegisterFormat(
                    pThis->m_pClipboard, "FileGroupDescriptorW");
        pThis->m_FormatIds.push_back(pFormats[numFormats].formatId);
        numFormats++;
    }
    if(pData->hasImage())
    {
        pFormats[numFormats].formatId = CF_BITMAP;
        pFormats[numFormats].formatName = nullptr;
        pThis->m_FormatIds.push_back(pFormats[numFormats].formatId);
        numFormats++;
        pFormats[numFormats].formatId = CF_DIB;
        pFormats[numFormats].formatName = nullptr;
        pThis->m_FormatIds.push_back(pFormats[numFormats].formatId);
        numFormats++;
        pFormats[numFormats].formatId = CF_DIBV5;
        pFormats[numFormats].formatName = nullptr;
        pThis->m_FormatIds.push_back(pFormats[numFormats].formatId);
        numFormats++;
    }
//    if(pData->hasHtml())
//    {
//        pFormats[numFormats].formatId = CB_FORMAT_HTML;
//        pFormats[numFormats].formatName = _strdup("text/html");
//        pThis->m_FormatIds.push_back(pFormats[numFormats].formatId);
//        numFormats++;
//    }
    if(pData->hasText())
    {
        pFormats[numFormats].formatId = CF_TEXT;
        pFormats[numFormats].formatName = nullptr;
        pThis->m_FormatIds.push_back(pFormats[numFormats].formatId);
        numFormats++;
        pFormats[numFormats].formatId = CF_OEMTEXT;
        pFormats[numFormats].formatName = nullptr;
        pThis->m_FormatIds.push_back(pFormats[numFormats].formatId);
        numFormats++;
        pFormats[numFormats].formatId = CF_UNICODETEXT;
        pFormats[numFormats].formatName = nullptr;
        pThis->m_FormatIds.push_back(pFormats[numFormats].formatId);
        numFormats++;
    }

    LOG_MODEL_DEBUG("FreeRdp", "SendClientFormatList formats: %d", numFormats);
    formatList.msgFlags = CB_RESPONSE_OK;
	formatList.numFormats = numFormats;
	formatList.formats = pFormats;
	formatList.msgType = CB_FORMAT_LIST;

    /* Ensure all pending requests are answered. */
	SendFormatDataResponse(context, NULL, 0);

    Q_ASSERT(context->ClientFormatList);
	nRet = context->ClientFormatList(context, &formatList);
    LOG_MODEL_DEBUG("FreeRdp", "SendClientFormatList nRet: %d", nRet);
    for(UINT32 i = 0; i < numFormats; i++)
        if(pFormats[i].formatName)
            free(pFormats[i].formatName);
    delete []pFormats;
    return nRet;
}

///////// Send format data from client to server ///////////

UINT CClipboardFreeRdp::cb_cliprdr_server_format_data_request(CliprdrClientContext* context,
                                       const CLIPRDR_FORMAT_DATA_REQUEST* formatDataRequest)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::cb_cliprdr_server_format_data_request");
    int nRet = CHANNEL_RC_OK;
    bool bSuucess = false;
    QString mimeType;
    BYTE* pDstData = NULL;
    UINT32 dstSize = 0;
    UINT32 dstFormatId = formatDataRequest->requestedFormatId;
    UINT32 scrFormatID = 0;
    if(!context) return nRet;
    
    CClipboardFreeRdp* pThis = (CClipboardFreeRdp*)context->custom;
    if(!pThis->m_FormatIds.contains(dstFormatId))
    {
        LOG_MODEL_ERROR("FreeRdp", "The formatid[%d] is support", dstFormatId);
        return nRet;
    }
    LOG_MODEL_DEBUG("FreeRdp", "server format date request formatID: %d", dstFormatId);
    QClipboard *clipboard = QApplication::clipboard();
    //if(!clipboard->ownsClipboard()) return nRet;
    
    switch(dstFormatId)
    {
    case CF_TEXT:
    case CF_OEMTEXT:
    case CF_UNICODETEXT:
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
    case CF_BITMAP:
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
        if(ClipboardGetFormatId(pThis->m_pClipboard, "FileGroupDescriptorW")
                == dstFormatId)
        {
            mimeType = "text/uri-list";
            scrFormatID = ClipboardGetFormatId(pThis->m_pClipboard, "text/uri-list");
        } else {
            mimeType = ClipboardGetFormatName(pThis->m_pClipboard, dstFormatId);
            scrFormatID = dstFormatId;
            if("text/uri-list" == mimeType)
            {
                dstFormatId = ClipboardGetFormatId(pThis->m_pClipboard, "FileGroupDescriptorW");
            }
        }
        if(!mimeType.isEmpty() && scrFormatID > 0)
        {
            QByteArray data = clipboard->mimeData()->data(mimeType);
            qDebug() << data << data.length();
            if(!data.isEmpty())
                bSuucess = ClipboardSetData(pThis->m_pClipboard, scrFormatID, data.data(), data.size());
        }
        break;
    }

    if(bSuucess)
        pDstData = (BYTE*)ClipboardGetData(pThis->m_pClipboard, dstFormatId, &dstSize);
    
    if(!pDstData)
    {
        nRet = SendFormatDataResponse(context, NULL, 0);
        return nRet;
    }
    
#if QT_VERSION_CHECK(WINPR_VERSION_MAJOR, WINPR_VERSION_MINOR, WINPR_VERSION_REVISION) > QT_VERSION_CHECK(2, 4, 2)
    /*
	 * File lists require a bit of postprocessing to convert them from WinPR's FILDESCRIPTOR
	 * format to CLIPRDR_FILELIST expected by the server.
	 *
	 * We check for "FileGroupDescriptorW" format being registered (i.e., nonzero) in order
	 * to not process CF_RAW as a file list in case WinPR does not support file transfers.
	 */
    if(dstFormatId && ClipboardGetFormatId(pThis->m_pClipboard, "FileGroupDescriptorW") == dstFormatId)
    {
        UINT error = NO_ERROR;
        FILEDESCRIPTORW* file_array = (FILEDESCRIPTORW*)pDstData;
        UINT32 file_count = dstSize / sizeof(FILEDESCRIPTORW);

        error = cliprdr_serialize_file_list_ex(
                    pThis->m_FileCapabilityFlags, file_array,
                    file_count, &pDstData, &dstSize);

        if (error)
            LOG_MODEL_ERROR("FreeRdp", "failed to serialize CLIPRDR_FILELIST: 0x%08X", error);

        free(file_array);
    }
#endif
    
    nRet = SendFormatDataResponse(context, pDstData, dstSize);

    if(pDstData)
        free(pDstData);

    return nRet;
}

UINT CClipboardFreeRdp::SendFormatDataResponse(CliprdrClientContext *context,
                                               const BYTE *data, size_t size)
{
    CLIPRDR_FORMAT_DATA_RESPONSE response = { 0 };
	response.msgFlags = (data) ? CB_RESPONSE_OK : CB_RESPONSE_FAIL;
	response.dataLen = size;
	response.requestedFormatData = data;
	return context->ClientFormatDataResponse(context, &response);
}

///////// Send file from client to server ///////////

UINT CClipboardFreeRdp::cb_clipboard_file_size_success(
        wClipboardDelegate* delegate,
        const wClipboardFileSizeRequest* request,
        UINT64 fileSize)
{
	CLIPRDR_FILE_CONTENTS_RESPONSE response = { 0 };
	CClipboardFreeRdp* pThis = (CClipboardFreeRdp*)delegate->custom;
	response.msgFlags = CB_RESPONSE_OK;
	response.streamId = request->streamId;
	response.cbRequested = sizeof(UINT64);
	response.requestedData = (BYTE*)&fileSize;
	return pThis->m_pCliprdrClientContext->ClientFileContentsResponse(
                pThis->m_pCliprdrClientContext, &response);
}

UINT CClipboardFreeRdp::cb_clipboard_file_size_failure(
        wClipboardDelegate* delegate,
        const wClipboardFileSizeRequest* request,
        UINT errorCode)
{
	CLIPRDR_FILE_CONTENTS_RESPONSE response = { 0 };
	CClipboardFreeRdp* pThis = (CClipboardFreeRdp*)delegate->custom;
	WINPR_UNUSED(errorCode);

	response.msgFlags = CB_RESPONSE_FAIL;
	response.streamId = request->streamId;
	return pThis->m_pCliprdrClientContext->ClientFileContentsResponse(
                pThis->m_pCliprdrClientContext, &response);
}

UINT CClipboardFreeRdp::cb_clipboard_file_range_success(
        wClipboardDelegate* delegate,
        const wClipboardFileRangeRequest* request,
        const BYTE* data, UINT32 size)
{
	CLIPRDR_FILE_CONTENTS_RESPONSE response = { 0 };
	CClipboardFreeRdp* pThis = (CClipboardFreeRdp*)delegate->custom;
	response.msgFlags = CB_RESPONSE_OK;
	response.streamId = request->streamId;
	response.cbRequested = size;
	response.requestedData = (BYTE*)data;
	return pThis->m_pCliprdrClientContext->ClientFileContentsResponse(
                pThis->m_pCliprdrClientContext, &response);
}

UINT CClipboardFreeRdp::cb_clipboard_file_range_failure(
        wClipboardDelegate* delegate,
        const wClipboardFileRangeRequest* request,
        UINT errorCode)
{
	CLIPRDR_FILE_CONTENTS_RESPONSE response = { 0 };
	CClipboardFreeRdp* pThis = (CClipboardFreeRdp*)delegate->custom;
	WINPR_UNUSED(errorCode);

	response.msgFlags = CB_RESPONSE_FAIL;
	response.streamId = request->streamId;
	return pThis->m_pCliprdrClientContext->ClientFileContentsResponse(
                pThis->m_pCliprdrClientContext, &response);
}

UINT CClipboardFreeRdp::SendFileContentsFailure(CliprdrClientContext* context,
                  const CLIPRDR_FILE_CONTENTS_REQUEST* fileContentsRequest)
{
	CLIPRDR_FILE_CONTENTS_RESPONSE response = { 0 };
	response.msgFlags = CB_RESPONSE_FAIL;
	response.streamId = fileContentsRequest->streamId;
	return context->ClientFileContentsResponse(context, &response);
}

UINT CClipboardFreeRdp::ServerFileRangeRequest(
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

UINT CClipboardFreeRdp::ServerFileSizeRequest(
        const CLIPRDR_FILE_CONTENTS_REQUEST* fileContentsRequest)
{
	wClipboardFileSizeRequest request = { 0 };
	request.streamId = fileContentsRequest->streamId;
	request.listIndex = fileContentsRequest->listIndex;

    wClipboardDelegate* pDelegate = ClipboardGetDelegate(m_pClipboard);
    if(!pDelegate) return E_FAIL;

	if (fileContentsRequest->cbRequested != sizeof(UINT64))
	{
		LOG_MODEL_ERROR("FreeRdp", "unexpected FILECONTENTS_SIZE request: %" PRIu32 " bytes",
		          fileContentsRequest->cbRequested);
	}

	return pDelegate->ClientRequestFileSize(pDelegate, &request);
}

UINT CClipboardFreeRdp::cb_cliprdr_server_file_contents_request(
        CliprdrClientContext* context,
        const CLIPRDR_FILE_CONTENTS_REQUEST* fileContentsRequest)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::cb_cliprdr_server_file_contents_request");
    int nRet = CHANNEL_RC_OK;

    UINT error = NO_ERROR;
    
    if(!context) return nRet;

    CClipboardFreeRdp* pThis = (CClipboardFreeRdp*)context->custom;
    
    /*
	 * MS-RDPECLIP 2.2.5.3 File Contents Request PDU (CLIPRDR_FILECONTENTS_REQUEST):
	 * The FILECONTENTS_SIZE and FILECONTENTS_RANGE flags MUST NOT be set at the same time.
	 */
	if ((fileContentsRequest->dwFlags & (FILECONTENTS_SIZE | FILECONTENTS_RANGE)) ==
	    (FILECONTENTS_SIZE | FILECONTENTS_RANGE))
	{
		LOG_MODEL_ERROR("FreeRdp", "invalid CLIPRDR_FILECONTENTS_REQUEST.dwFlags");
		return SendFileContentsFailure(context, fileContentsRequest);
	}

	if (fileContentsRequest->dwFlags & FILECONTENTS_SIZE)
		error = pThis->ServerFileSizeRequest(fileContentsRequest);

	if (fileContentsRequest->dwFlags & FILECONTENTS_RANGE)
		error = pThis->ServerFileRangeRequest(fileContentsRequest);

	if (error)
	{
		LOG_MODEL_ERROR("FreeRdp", "failed to handle CLIPRDR_FILECONTENTS_REQUEST: 0x%08X", error);
		return SendFileContentsFailure(context, fileContentsRequest);
	}

    return nRet;
}

///////// Server to client ///////////
UINT CClipboardFreeRdp::cb_cliprdr_server_format_list(CliprdrClientContext* context,
                                              const CLIPRDR_FORMAT_LIST* formatList)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::cb_cliprdr_server_format_list");
    int nRet = CHANNEL_RC_OK;
    
    CClipboardFreeRdp* pThis = (CClipboardFreeRdp*)context->custom;
    CClipboardMimeData* pMimeData = nullptr;
    if(formatList->numFormats < 0)
    {
        return nRet;
    }
    pMimeData = new CClipboardMimeData(context);
    if(!pMimeData) return nRet;
    
    if(pMimeData->SetFormat(formatList))
    {
        pMimeData->deleteLater();
        return nRet;
    }

    pThis->m_ServerFormatDataRequest.clear();
    bool check = false;
    check = connect(pThis, SIGNAL(sigServerFormatData(const BYTE*, UINT32, UINT32, QString)),
                    pMimeData, SLOT(slotServerFormatData(const BYTE*, UINT32, UINT32, QString)),
                    Qt::DirectConnection);
    Q_ASSERT(check);
    check = connect(pMimeData, SIGNAL(sigSendDataRequest(CliprdrClientContext*, UINT32, QString)),
                    pThis, SLOT(slotSendFormatDataRequest(CliprdrClientContext*, UINT32, QString)));
    Q_ASSERT(check);
    
    pThis->m_bOwns = true;
    emit pThis->m_pConnect->sigSetClipboard(pMimeData);
    return nRet;
}

UINT CClipboardFreeRdp::cb_cliprdr_server_format_list_response(CliprdrClientContext* context,
                                      const CLIPRDR_FORMAT_LIST_RESPONSE* pformatListResponse)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::cb_cliprdr_server_format_list_response:type:%d;flag:%d;datalen:%d",
                    pformatListResponse->msgType, pformatListResponse->msgFlags, pformatListResponse->dataLen);
    int nRet = CHANNEL_RC_OK;
    return nRet;
}

UINT CClipboardFreeRdp::slotSendFormatDataRequest(CliprdrClientContext* context,
                                            UINT32 formatId, QString formatName)
{
	UINT rc = CHANNEL_RC_OK;
	CLIPRDR_FORMAT_DATA_REQUEST formatDataRequest;

	if (!context || !context->ClientFormatDataRequest)
		return ERROR_INTERNAL_ERROR;

    CClipboardFreeRdp* pThis = (CClipboardFreeRdp*)context->custom;
    CClipboardMimeData::_FORMAT f = {formatId, formatName};
    pThis->m_ServerFormatDataRequest.push_back(f);
	formatDataRequest.requestedFormatId = formatId;
	rc = context->ClientFormatDataRequest(context, &formatDataRequest);

	return rc;
}

UINT CClipboardFreeRdp::cb_cliprdr_server_format_data_response(
        CliprdrClientContext* context,
        const CLIPRDR_FORMAT_DATA_RESPONSE* formatDataResponse)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::cb_cliprdr_server_format_data_response");
    int nRet = CHANNEL_RC_OK;
    if(!context) return nRet;

    CClipboardFreeRdp* pThis = (CClipboardFreeRdp*)context->custom;
   
    auto it = pThis->m_ServerFormatDataRequest.begin();
    pThis->m_ServerFormatDataRequest.pop_front();
    emit pThis->sigServerFormatData(formatDataResponse->requestedFormatData,
                                    formatDataResponse->dataLen,
                                    it->id,
                                    it->name.toStdString().c_str());
    return nRet;
}

UINT CClipboardFreeRdp::cb_cliprdr_server_file_contents_response(CliprdrClientContext* context,
                     const CLIPRDR_FILE_CONTENTS_RESPONSE* fileContentsResponse)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::cb_cliprdr_server_file_contents_response");
    int nRet = CHANNEL_RC_OK;
    return nRet;
}
