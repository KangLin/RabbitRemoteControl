// Author: Kang Lin <kl222@126.com>

#ifndef CCLIPBOARDFREERDP_H
#define CCLIPBOARDFREERDP_H

#include <QObject>
#include <QVector>
#include "freerdp/freerdp.h"
#include "freerdp/client/cliprdr.h"
#include "freerdp/client/rdpgfx.h"
#include "winpr/clipboard.h"
#include "ClipboardMimeData.h"

class CConnectFreeRdp;
class CClipboardFreeRdp : public QObject
{
    Q_OBJECT
public:
    explicit CClipboardFreeRdp(CConnectFreeRdp *parent = nullptr);
    virtual ~CClipboardFreeRdp();
    
    int Init(CliprdrClientContext *context, bool bEnable);
    int UnInit(CliprdrClientContext* context, bool bEnable);
    
    static UINT cb_cliprdr_server_capabilities(CliprdrClientContext* context,
                                      const CLIPRDR_CAPABILITIES* capabilities);
    
    static UINT cb_cliprdr_server_format_list(CliprdrClientContext* context,
                                         const CLIPRDR_FORMAT_LIST* formatList);
    static UINT
    cb_cliprdr_server_format_list_response(CliprdrClientContext* context,
                        const CLIPRDR_FORMAT_LIST_RESPONSE* formatListResponse);
    
    static UINT
    cb_cliprdr_server_format_data_response(CliprdrClientContext* context,
                        const CLIPRDR_FORMAT_DATA_RESPONSE* formatDataResponse);
    static UINT
    cb_cliprdr_server_file_contents_request(CliprdrClientContext* context,
                      const CLIPRDR_FILE_CONTENTS_REQUEST* fileContentsRequest);
    static UINT
    cb_cliprdr_server_file_contents_response(CliprdrClientContext* context,
                    const CLIPRDR_FILE_CONTENTS_RESPONSE* fileContentsResponse);
    
    ///////// Send format list from client to server ///////////
    static UINT cb_cliprdr_monitor_ready(CliprdrClientContext* context,
                                     const CLIPRDR_MONITOR_READY* monitorReady);
    static UINT SendClientFormatList(CliprdrClientContext *context);
    
    ///////// Send format data from client to server ///////////
    static UINT
    cb_cliprdr_server_format_data_request(CliprdrClientContext* context,
                          const CLIPRDR_FORMAT_DATA_REQUEST* formatDataRequest);
    static UINT SendFormatDataResponse(CliprdrClientContext* context,
                                 const BYTE* data, size_t size);
    
    static UINT SendDataRequest(CliprdrClientContext* context, UINT32 formatId);
    
    ///////// Send file from client to server ///////////
    static UINT SendFileContentsFailure(CliprdrClientContext* context,
                      const CLIPRDR_FILE_CONTENTS_REQUEST* fileContentsRequest);
    UINT ServerFileSizeRequest(
            const CLIPRDR_FILE_CONTENTS_REQUEST* fileContentsRequest);
    UINT ServerFileRangeRequest(
            const CLIPRDR_FILE_CONTENTS_REQUEST* fileContentsRequest);
    static UINT cb_clipboard_file_size_success(wClipboardDelegate* delegate,
                                   const wClipboardFileSizeRequest* request,
                                                           UINT64 fileSize);
    static UINT cb_clipboard_file_size_failure(
            wClipboardDelegate* delegate,
            const wClipboardFileSizeRequest* request,
            UINT errorCode);
    static UINT cb_clipboard_file_range_success(
            wClipboardDelegate* delegate,
            const wClipboardFileRangeRequest* request,
            const BYTE* data, UINT32 size);
    static UINT cb_clipboard_file_range_failure(
            wClipboardDelegate* delegate,
            const wClipboardFileRangeRequest* request,
            UINT errorCode);
    
public Q_SLOTS:
    virtual void slotClipBoardChange();
    
private:
    CConnectFreeRdp* m_pConnect;
    CliprdrClientContext* m_pCliprdrClientContext;
    CClipboardMimeData* m_pMimeData;
    wClipboard* m_pClipboard;
    QVector<UINT32> m_FormatIds;
    UINT32 m_FileCapabilityFlags;
};

#endif // CCLIPBOARDFREERDP_H
