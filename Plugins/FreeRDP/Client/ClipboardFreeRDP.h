// Author: Kang Lin <kl222@126.com>

#ifndef CCLIPBOARDFREERDP_H
#define CCLIPBOARDFREERDP_H

#include <QObject>
#include <QVector>
#include <QLoggingCategory>
#include <QList>
#include "ClipboardMimeData.h"
#include "freerdp/client/cliprdr.h"
#include "winpr/clipboard.h"

class CConnectFreeRDP;
class CClipboardFreeRDP : public QObject
{
    Q_OBJECT
public:
    explicit CClipboardFreeRDP(CConnectFreeRDP *parent = nullptr);
    virtual ~CClipboardFreeRDP();

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
public Q_SLOTS:
    static UINT slotSendFormatDataRequest(CliprdrClientContext* context,
                                    UINT32 formatId);

public:
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
    static BOOL cbIsFileNameComponentValid(LPCWSTR lpFileName);
    
public Q_SLOTS:
    virtual void slotClipBoardChanged();

Q_SIGNALS:
    //!
    //! \brief Notify clipboard get data from server
    //! \param pData: data pointer
    //! \param nLen: data length
    //! \param formatId: format id
    //! if(pData == nullptr && nLen == 0) is Notify clipboard program has exited
    void sigServerFormatData(const BYTE* pData, UINT32 nLen,
                             UINT32 formatId);
    void sigServerFileContentsRespose(UINT32 streamId, QByteArray& data);
    
private:
    CConnectFreeRDP* m_pConnect;
    CliprdrClientContext* m_pCliprdrClientContext;
    friend CClipboardMimeData;
    QList<qint32> m_lstClipboardMimeDataId;
    wClipboard* m_pClipboard; // Clipboard interface provided by winpr
    // Client request format data
    UINT32 m_RequestFormatId;

    // File
    UINT32 m_FileCapabilityFlags;
    bool m_bFileSupported; // Whether is server support file
    BOOL m_bFileFormatsRegistered;
};

#endif // CCLIPBOARDFREERDP_H
