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

CClipboardFreeRdp::CClipboardFreeRdp(CConnectFreeRdp *parent) : QObject(parent),
    m_pConnect(parent),
    m_pCliprdrClientContext(nullptr),
    m_pMimeData(nullptr),
    m_pClipboard(nullptr)
{
    m_pClipboard = ClipboardCreate();
}

CClipboardFreeRdp::~CClipboardFreeRdp()
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::~CClipboardFreeRdp()");
    QClipboard* pClipboard = QApplication::clipboard();
    if(pClipboard && pClipboard->ownsClipboard())
        pClipboard->clear();
    
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
    SendClientFormatList(m_pCliprdrClientContext);
}

UINT CClipboardFreeRdp::cb_cliprdr_server_capabilities(CliprdrClientContext* context,
                                           const CLIPRDR_CAPABILITIES* capabilities)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::cb_cliprdr_server_capabilities");
    int nRet = CHANNEL_RC_OK;

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
				//TODO: support file clipboard
			}
		}

		capsPtr += caps->capabilitySetLength;
	}
    return nRet;
}

UINT CClipboardFreeRdp::cb_cliprdr_monitor_ready(CliprdrClientContext *context, const CLIPRDR_MONITOR_READY *monitorReady)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::cb_cliprdr_monitor_ready");
    UINT nRet = CHANNEL_RC_OK;

    if (!context || !context->ClientCapabilities)
	{
        Q_ASSERT(false);
        return ERROR_INTERNAL_ERROR;
    }
    
    // Send client capabilities
    CLIPRDR_CAPABILITIES capabilities;
	CLIPRDR_GENERAL_CAPABILITY_SET generalCapabilitySet;
	capabilities.cCapabilitiesSets = 1;
	capabilities.capabilitySets = (CLIPRDR_CAPABILITY_SET*)&(generalCapabilitySet);
	generalCapabilitySet.capabilitySetType = CB_CAPSTYPE_GENERAL;
	generalCapabilitySet.capabilitySetLength = 12;
	generalCapabilitySet.version = CB_CAPS_VERSION_2;
	generalCapabilitySet.generalFlags = CB_USE_LONG_FORMAT_NAMES
            | CB_STREAM_FILECLIP_ENABLED | CB_FILECLIP_NO_FILE_PATHS
            | CB_HUGE_FILE_SUPPORT_ENABLED;
	if((nRet = context->ClientCapabilities(context, &capabilities)) != CHANNEL_RC_OK)
    {
        LOG_MODEL_ERROR("FreeRdp", "Send Client Capabilities fail");
        return nRet;
    }
    
    // Send client formats
    return SendClientFormatList(context);
}

UINT CClipboardFreeRdp::cb_cliprdr_server_format_list(CliprdrClientContext* context,
                                              const CLIPRDR_FORMAT_LIST* formatList)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::cb_cliprdr_server_format_list");
    int nRet = CHANNEL_RC_OK;
    
    CClipboardFreeRdp* pThis = (CClipboardFreeRdp*)context->custom;
    
    if(formatList->numFormats > 0)
    {   
        pThis->m_pMimeData = new CClipboardMimeData(context);
        if(!pThis->m_pMimeData) return nRet;
    }
    
    for (UINT32 i = 0; i < formatList->numFormats; i++)
	{
		CLIPRDR_FORMAT* pFormat = &formatList->formats[i];
        //TODO: Delete it
        LOG_MODEL_DEBUG("FreeRdp", "cb_cliprdr_server_format_list FormatId: 0x%X; name: %s; get name:%s",
                        pFormat->formatId,
                        pFormat->formatName,
                        ClipboardGetFormatName(pThis->m_pClipboard, pFormat->formatId));
        //Set clipboard format
        pThis->m_pMimeData->AddFormat(pFormat->formatId, pFormat->formatName);
	}
    emit pThis->m_pConnect->sigSetClipboard(pThis->m_pMimeData);
    return nRet;
}

UINT CClipboardFreeRdp::cb_cliprdr_server_format_list_response(CliprdrClientContext* context,
                                      const CLIPRDR_FORMAT_LIST_RESPONSE* pformatListResponse)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::cb_cliprdr_server_format_list_response");
    int nRet = CHANNEL_RC_OK;
    return nRet;
}

UINT CClipboardFreeRdp::cb_cliprdr_server_format_data_request(CliprdrClientContext* context,
                                       const CLIPRDR_FORMAT_DATA_REQUEST* formatDataRequest)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::cb_cliprdr_server_format_data_request");
    int nRet = CHANNEL_RC_OK;
    bool bSuucess = false;
    QString mimeType;
    BYTE* pDstData = NULL;
    UINT32 dstSize = 0;
    UINT32 formatId = formatDataRequest->requestedFormatId;
    UINT32 scrFormatID = 0;
    if(!context) return nRet;
    
    CClipboardFreeRdp* pThis = (CClipboardFreeRdp*)context->custom;
    if(!pThis->m_FormatIds.contains(formatId))
    {
        LOG_MODEL_ERROR("FreeRdp", "The formatid[%d] is support", formatId);
        return nRet;
    }
    LOG_MODEL_DEBUG("FreeRdp", "server format date request formatID: %d", formatId);
    QClipboard *clipboard = QApplication::clipboard();
    //if(!clipboard->ownsClipboard()) return nRet;
    
    switch(formatId)
    {
    case CF_TEXT:
    case CF_OEMTEXT:
    case CF_UNICODETEXT:
    {
        if(clipboard->mimeData()->hasText())
        {
            QString szData = clipboard->text();
            //qDebug() << szData << szData.size();
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
            //qDebug() << d << d.length();
            if(!d.isEmpty())
                bSuucess = ClipboardSetData(pThis->m_pClipboard,
                         ClipboardGetFormatId(pThis->m_pClipboard, "image/bmp"),
                                            (BYTE*)d.data(), d.length());
        }
        break;
    default:
        mimeType = ClipboardGetFormatName(pThis->m_pClipboard, formatId);
        scrFormatID = formatId;
        if(!mimeType.isEmpty() && scrFormatID > 0)
        {
            QByteArray data = clipboard->mimeData()->data(mimeType);
            //qDebug() << data << data.length();
            if(!data.isEmpty())
                bSuucess = ClipboardSetData(pThis->m_pClipboard, scrFormatID, data.data(), data.size());
        }
        break;
    }

    if(bSuucess)
        pDstData = (BYTE*)ClipboardGetData(pThis->m_pClipboard, formatId, &dstSize);
    if(pDstData)
        SendDataResponse(context, pDstData, dstSize);
    else
        SendDataResponse(context, NULL, 0);
    if(pDstData)
        free(pDstData);
    
    return nRet;
}

UINT CClipboardFreeRdp::cb_cliprdr_server_format_data_response(CliprdrClientContext* context,
                                      const CLIPRDR_FORMAT_DATA_RESPONSE* formatDataResponse)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::cb_cliprdr_server_format_data_response");
    int nRet = CHANNEL_RC_OK;
    if(!context) return nRet;
    
    CClipboardFreeRdp* pThis = (CClipboardFreeRdp*)context->custom;
    if(!pThis->m_pMimeData)
        return nRet;
    
    pThis->m_pMimeData->SetData((const char*)formatDataResponse->requestedFormatData,
                                formatDataResponse->dataLen);
    return nRet;
}

UINT CClipboardFreeRdp::cb_cliprdr_server_file_contents_request(CliprdrClientContext* context,
                                     const CLIPRDR_FILE_CONTENTS_REQUEST* fileContentsRequest)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::cb_cliprdr_server_file_contents_request");
    int nRet = CHANNEL_RC_OK;
    
    return nRet;
}

UINT CClipboardFreeRdp::cb_cliprdr_server_file_contents_response(CliprdrClientContext* context,
                     const CLIPRDR_FILE_CONTENTS_RESPONSE* fileContentsResponse)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::cb_cliprdr_server_file_contents_response");
    int nRet = CHANNEL_RC_OK;
    
    return nRet;
}

// See: [MS_RDPECLIP] 1.3.2.2 Data Transfer Sequences
// See: https://docs.microsoft.com/en-us/windows/win32/dataxchg/clipboard-formats
UINT CClipboardFreeRdp::SendClientFormatList(CliprdrClientContext *context)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::SendClientFormatList");
    int nRet = CHANNEL_RC_OK;
    CLIPRDR_FORMAT* pFormats = nullptr;
	CLIPRDR_FORMAT_LIST formatList = { 0 };
    UINT32 numFormats = 0;

    if(!context) return nRet;
    QClipboard *clipboard = QApplication::clipboard();
    if(!clipboard || clipboard->ownsClipboard())
    {
        LOG_MODEL_DEBUG("FreeRdp", "ownsClipboard");
        return nRet;
    }

    CClipboardFreeRdp* pThis = (CClipboardFreeRdp*)context->custom;
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
        if("text/uri-list" == f)
        {
            pFormats[numFormats].formatName = _strdup("FileGroupDescriptorW");
            pFormats[numFormats].formatId = CB_FORMAT_TEXTURILIST;
            pThis->m_FormatIds.push_back(pFormats[numFormats].formatId);
        }
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
	//SendDataResponse(context, NULL, 0);
    
	nRet = context->ClientFormatList(context, &formatList);
    LOG_MODEL_DEBUG("FreeRdp", "SendClientFormatList nRet: %d", nRet);
    for(int i = 0; i < numFormats; i++)
        if(pFormats[i].formatName)
            free(pFormats[i].formatName);
    delete []pFormats;
    return nRet;
}

UINT CClipboardFreeRdp::SendDataRequest(CliprdrClientContext* context, UINT32 formatId)
{
	UINT rc = CHANNEL_RC_OK;
	CLIPRDR_FORMAT_DATA_REQUEST formatDataRequest;

	if (!context || !context->ClientFormatDataRequest)
		return ERROR_INTERNAL_ERROR;

	formatDataRequest.requestedFormatId = formatId;
	rc = context->ClientFormatDataRequest(context, &formatDataRequest);

	return rc;
}

UINT CClipboardFreeRdp::SendDataResponse(CliprdrClientContext *context, const BYTE *data, size_t size)
{
    CLIPRDR_FORMAT_DATA_RESPONSE response = { 0 };

    //TODO: check request format id
    
	response.msgFlags = (data) ? CB_RESPONSE_OK : CB_RESPONSE_FAIL;
	response.dataLen = size;
	response.requestedFormatData = data;
	return context->ClientFormatDataResponse(context, &response);
}
