#include "ClipboardFreeRdp.h"
#include "RabbitCommonLog.h"
#include <QClipboard>
#include <QApplication>
#include <QMimeData>
#include <QtDebug>
#include <QImage>
#include <QBuffer>

extern const char* CF_STANDARD_STRINGS[CF_MAX];
static const char* mime_text[] = { "text/plain",  "text/plain;charset=utf-8",
	                               "UTF8_STRING", "COMPOUND_TEXT",
	                               "TEXT",        "STRING" };

static const char* mime_image[] = {
	"image/png",       "image/bmp",   "image/x-bmp",        "image/x-MS-bmp",
	"image/x-icon",    "image/x-ico", "image/x-win-bitmap", "image/vmd.microsoft.icon",
	"application/ico", "image/ico",   "image/icon",         "image/jpeg",
	"image/tiff"
};

static const char* mime_html[] = { "text/html" };

static BOOL mime_is_text(const char* mime)
{
	size_t x;

	for (x = 0; x < ARRAYSIZE(mime_text); x++)
	{
		if (strcmp(mime, mime_text[x]) == 0)
			return TRUE;
	}

	return FALSE;
}

static BOOL mime_is_image(const char* mime)
{
	size_t x;

	for (x = 0; x < ARRAYSIZE(mime_image); x++)
	{
		if (strcmp(mime, mime_image[x]) == 0)
			return TRUE;
	}

	return FALSE;
}

static BOOL mime_is_html(const char* mime)
{
	size_t x;

	for (x = 0; x < ARRAYSIZE(mime_html); x++)
	{
		if (strcmp(mime, mime_html[x]) == 0)
			return TRUE;
	}

	return FALSE;
}

CClipboardFreeRdp::CClipboardFreeRdp(QObject *parent) : QObject(parent),
    m_pClipboard(nullptr)
{}

int CClipboardFreeRdp::Init(CliprdrClientContext *cliprdr)
{
    m_pClipboard = cliprdr;
    cliprdr->custom = this;
    // See: [MS_RDPECLIP] 1.3.2.1
    cliprdr->MonitorReady = cb_cliprdr_monitor_ready;
	cliprdr->ServerCapabilities = cb_cliprdr_server_capabilities;
	cliprdr->ServerFormatList = cb_cliprdr_server_format_list;
	cliprdr->ServerFormatListResponse = cb_cliprdr_server_format_list_response;
	cliprdr->ServerFormatDataRequest = cb_cliprdr_server_format_data_request;
	cliprdr->ServerFormatDataResponse = cb_cliprdr_server_format_data_response;
	cliprdr->ServerFileContentsRequest = cb_cliprdr_server_file_contents_request;
    return 0;
}

int CClipboardFreeRdp::UnInit(CliprdrClientContext *cliprdr)
{
    cliprdr->custom = nullptr;
    m_pClipboard = nullptr;
    return 0;
}

void CClipboardFreeRdp::slotClipBoardChange()
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::slotClipBoardChange");
    SendClientFormatList(m_pClipboard);
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
	generalCapabilitySet.generalFlags =
	    CB_USE_LONG_FORMAT_NAMES | CB_STREAM_FILECLIP_ENABLED | CB_FILECLIP_NO_FILE_PATHS;
	if((nRet = context->ClientCapabilities(context, &capabilities)) != CHANNEL_RC_OK)
    {
        LOG_MODEL_ERROR("FreeRdp", "Send Client Capabilities fail");
        return nRet;
    }
    
    // Send client formats
    return SendClientFormatList(context);
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

UINT CClipboardFreeRdp::cb_cliprdr_server_format_list(CliprdrClientContext* context,
                                              const CLIPRDR_FORMAT_LIST* formatList)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::cb_cliprdr_server_format_list");
    int nRet = CHANNEL_RC_OK;
    for (UINT32 i = 0; i < formatList->numFormats; i++)
	{
		CLIPRDR_FORMAT* format = &formatList->formats[i];
        //TODO: Delete it
        LOG_MODEL_DEBUG("FreeRdp", "cb_cliprdr_server_format_list FormatId: %d; name: %s",
                        format->formatId,
                        format->formatName);
        //TODO: Set clipboard format
	}
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
    UINT32 formatId = formatDataRequest->requestedFormatId;
    LOG_MODEL_DEBUG("FreeRdp", "server format date request: %d", formatId);
    QClipboard *clipboard = QApplication::clipboard();
    switch(formatId)
    {
    case CF_TEXT:
    {
        if(clipboard->mimeData()->hasText())
        {
            QString szData = clipboard->text();
            std::string d = szData.toStdString();
            cb_cliprdr_send_data_response(context, (BYTE*)d.c_str(), d.length());
        }
        break;
    }
    case CB_FORMAT_HTML:
        if(clipboard->mimeData()->hasHtml())
        {
            QString szData = clipboard->mimeData()->html();
            std::string d = szData.toStdString();
            cb_cliprdr_send_data_response(context, (BYTE*)d.c_str(), d.length());
        }
        break;
    case CB_FORMAT_PNG:
    case CB_FORMAT_JPEG:
    case CB_FORMAT_GIF:
    case CF_DIB:
        if(clipboard->mimeData()->hasImage())
        {
            QImage img = clipboard->image();
            QByteArray d;
            QBuffer buffer(&d);
            buffer.open(QIODevice::WriteOnly);
            img.save(&buffer, "BMP");
            buffer.close();
            cb_cliprdr_send_data_response(context, (BYTE*)d.data(), d.length());
        }
    default:
        // format invalie
        cb_cliprdr_send_data_response(context, NULL, 0);
    }

    return nRet;
}

UINT CClipboardFreeRdp::cb_cliprdr_server_format_data_response(CliprdrClientContext* context,
                                      const CLIPRDR_FORMAT_DATA_RESPONSE* formatDataResponse)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::cb_cliprdr_server_format_data_response");
    int nRet = CHANNEL_RC_OK;
    
    return nRet;
}

UINT CClipboardFreeRdp::cb_cliprdr_server_file_contents_request(CliprdrClientContext* context,
                                     const CLIPRDR_FILE_CONTENTS_REQUEST* fileContentsRequest)
{
    LOG_MODEL_DEBUG("FreeRdp", "CClipboardFreeRdp::cb_cliprdr_server_file_contents_request");
    int nRet = CHANNEL_RC_OK;
    
    return nRet;
}

UINT CClipboardFreeRdp::cb_cliprdr_send_data_response(CliprdrClientContext* context, BYTE* data, int size)
{
    CLIPRDR_FORMAT_DATA_RESPONSE response = { 0 };
	response.msgFlags = (data) ? CB_RESPONSE_OK : CB_RESPONSE_FAIL;
	response.dataLen = size;
	response.requestedFormatData = data;
	return context->ClientFormatDataResponse(context, &response);
}

// See: https://docs.microsoft.com/en-us/windows/win32/dataxchg/clipboard-formats
UINT CClipboardFreeRdp::SendClientFormatList(CliprdrClientContext *context)
{
    int nRet = CHANNEL_RC_OK;
    CLIPRDR_FORMAT* pFormats = new CLIPRDR_FORMAT[10];
	CLIPRDR_FORMAT_LIST formatList = { 0 };
    UINT32 numFormats = 0;

    if(!context) return nRet;
    
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData* pData = clipboard->mimeData();
    auto clipFormats = pData->formats();
    qDebug() << "Clipboard formats:" << clipFormats;
    
    if(pData->hasImage())
    {
        pFormats[numFormats].formatId = CF_DIB;
        pFormats[numFormats].formatName = _strdup("image/bmp");
        numFormats++;
    }
    if(pData->hasHtml())
    {
        pFormats[numFormats].formatId = CB_FORMAT_HTML;
        pFormats[numFormats].formatName = _strdup("text/html");
        numFormats++;
    }
    if(pData->hasText())
    {
        pFormats[numFormats].formatId = CF_TEXT;
        pFormats[numFormats].formatName = _strdup("text/plain");
        numFormats++;
    }
    
    LOG_MODEL_DEBUG("FreeRdp", "SendClientFormatList formats: %d", numFormats);
    formatList.msgFlags = CB_RESPONSE_OK;
	formatList.numFormats = numFormats;
	formatList.formats = pFormats;
	formatList.msgType = CB_FORMAT_LIST;
	nRet = context->ClientFormatList(context, &formatList);
    if(pFormats)
    {
        for(int i = 0; i < numFormats; i++)
            free(pFormats[i].formatName);
        free(pFormats);
    }
    return nRet;
}
