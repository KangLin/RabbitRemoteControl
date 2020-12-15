#include "ClipboardFreeRdp.h"
#include "RabbitCommonLog.h"
#include <QClipboard>
#include <QApplication>
#include <QMimeData>
#include <QtDebug>

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

CClipboardFreeRdp::CClipboardFreeRdp(QObject *parent) : QObject(parent)
{}

int CClipboardFreeRdp::Init(CliprdrClientContext *cliprdr)
{
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
    return 0;
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
        LOG_MODEL_DEBUG("FreeRdp", "FormatId: %d; name: %s",
                        format->formatId,
                        format->formatName);
        //TODO: Set clipboard format
	}
    return nRet;
}

UINT CClipboardFreeRdp::cb_cliprdr_server_format_list_response(CliprdrClientContext* context,
                                      const CLIPRDR_FORMAT_LIST_RESPONSE* formatListResponse)
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
    //CLIPRDR_FORMAT* format = cliprdr_
    //TODO: format invalie
    cb_cliprdr_send_data_response(context, NULL, 0);
    
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

UINT CClipboardFreeRdp::SendClientFormatList(CliprdrClientContext *context)
{
    int nRet = CHANNEL_RC_OK;
    CLIPRDR_FORMAT* pFormats = nullptr;
	CLIPRDR_FORMAT_LIST formatList = { 0 };
    UINT32 numFormats = 0;

    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData* pData = clipboard->mimeData();
    auto clipFormats = pData->formats();
    qDebug() << "Clipboard formats:" << clipFormats;
    for(UINT32 i = 0; i < clipFormats.size(); i++)
    {
        CLIPRDR_FORMAT format = {0, NULL};
        if("text/plain" == clipFormats.at(i)) {
            format.formatId = CF_TEXT;
            format.formatName = _strdup(clipFormats.at(i).toStdString().c_str());
        } else if("TEXT" == clipFormats.at(i)) {
            format.formatId = CF_TEXT;
            format.formatName = _strdup(clipFormats.at(i).toStdString().c_str());
        } else if("STRING" == clipFormats.at(i)) {
            format.formatId = CF_TEXT;
            format.formatName = _strdup(clipFormats.at(i).toStdString().c_str());
        }else if("text/html" == clipFormats.at(i)) {
            format.formatId = CB_FORMAT_HTML;
            format.formatName = _strdup(clipFormats.at(i).toStdString().c_str());
        } else if("image/png" == clipFormats.at(i)) {
            format.formatId = CB_FORMAT_PNG;
            format.formatName = _strdup(clipFormats.at(i).toStdString().c_str());
        } else if("image/jpeg" == clipFormats.at(i)) {
            format.formatId = CB_FORMAT_JPEG;
            format.formatName = _strdup(clipFormats.at(i).toStdString().c_str());
        } else if("image/gif" == clipFormats.at(i)) {
            format.formatId = CB_FORMAT_GIF;
            format.formatName = _strdup(clipFormats.at(i).toStdString().c_str());
        } else if("image/bmp" == clipFormats.at(i)) {
            format.formatId = CF_DIB;
            format.formatName = _strdup(clipFormats.at(i).toStdString().c_str());
        } else
            continue;
        if(nullptr == pFormats)
            pFormats = (CLIPRDR_FORMAT*)calloc(++numFormats, sizeof(CLIPRDR_FORMAT));
        else 
            pFormats = (CLIPRDR_FORMAT*)realloc(pFormats, ++numFormats * sizeof(CLIPRDR_FORMAT));
        pFormats[i+1] = format;
    }
    LOG_MODEL_DEBUG("FreeRdp", "formats: %d", numFormats);
    formatList.msgFlags = CB_RESPONSE_OK;
	formatList.numFormats = numFormats;
	formatList.formats = pFormats;
	formatList.msgType = CB_FORMAT_LIST;
	nRet = context->ClientFormatList(context, &formatList);
    if(pFormats)
        free(pFormats);
    return nRet;
}
