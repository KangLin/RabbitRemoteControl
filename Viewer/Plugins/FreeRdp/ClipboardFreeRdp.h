#ifndef CCLIPBOARDFREERDP_H
#define CCLIPBOARDFREERDP_H

#include <QObject>
#include "freerdp/freerdp.h"
#include "freerdp/client/cliprdr.h"
#include "freerdp/client/rdpgfx.h"

class CClipboardFreeRdp : public QObject
{
    Q_OBJECT
public:
    explicit CClipboardFreeRdp(QObject *parent = nullptr);
    
    int Init(CliprdrClientContext *context);
    int UnInit(CliprdrClientContext* context);
    
    static UINT cb_cliprdr_monitor_ready(CliprdrClientContext* context,
                                     const CLIPRDR_MONITOR_READY* monitorReady);
    static UINT cb_cliprdr_server_capabilities(CliprdrClientContext* context,
                                      const CLIPRDR_CAPABILITIES* capabilities);
    static UINT cb_cliprdr_server_format_list(CliprdrClientContext* context,
                                         const CLIPRDR_FORMAT_LIST* formatList);
    static UINT
    cb_cliprdr_server_format_list_response(CliprdrClientContext* context,
                        const CLIPRDR_FORMAT_LIST_RESPONSE* formatListResponse);
    static UINT
    cb_cliprdr_server_format_data_request(CliprdrClientContext* context,
                          const CLIPRDR_FORMAT_DATA_REQUEST* formatDataRequest);
    static UINT
    cb_cliprdr_server_format_data_response(CliprdrClientContext* context,
                        const CLIPRDR_FORMAT_DATA_RESPONSE* formatDataResponse);
    static UINT
    cb_cliprdr_server_file_contents_request(CliprdrClientContext* context,
                      const CLIPRDR_FILE_CONTENTS_REQUEST* fileContentsRequest);
    
    static UINT cb_cliprdr_send_data_response(CliprdrClientContext *context, BYTE* data, int size);
    
    static UINT SendClientFormatList(CliprdrClientContext *context);

public Q_SLOTS:
    virtual void slotClipBoardChange();
    
private:
    CliprdrClientContext* m_pClipboard;
    struct _FORMAT
    {
        UINT32 id;
        std::string name;
    };
    std::vector<_FORMAT> m_ServerClipboardFormats;
};

#endif // CCLIPBOARDFREERDP_H
