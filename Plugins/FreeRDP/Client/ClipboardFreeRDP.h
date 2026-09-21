// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QObject>
#include <QVector>
#include <QLoggingCategory>
#include <QList>
#include "ClipboardMimeData.h"
#include "freerdp/client/cliprdr.h"
#ifdef HAVE_FILE_INTERFACE
#include "freerdp/client/client_cliprdr_file.h"
#endif
#include "winpr/clipboard.h"
#include "freerdp/version.h"

class CBackendFreeRDP;

class CClipboardFreeRDP : public QObject
{
    Q_OBJECT

public:
    explicit CClipboardFreeRDP(CBackendFreeRDP *parent = nullptr);
    virtual ~CClipboardFreeRDP();

    int Init(CliprdrClientContext *context, bool bEnable);
    int UnInit(CliprdrClientContext* context, bool bEnable);

private:
    //! Receive server capabilities from server
    [[nodiscard]] static UINT cbServerCapabilities(
        CliprdrClientContext* context,
        const CLIPRDR_CAPABILITIES* capabilities);
    //! Send client capabilities to server
    [[nodiscard]] static UINT cbMonitorReady(
        CliprdrClientContext* context,
        const CLIPRDR_MONITOR_READY* monitorReady);

    //! Receive server format list from server
    [[nodiscard]] static UINT cbServerFormatList(
        CliprdrClientContext* context,
        const CLIPRDR_FORMAT_LIST* formatList);
    //! Send server format list response from client
    [[nodiscard]] UINT SendServerFormatListResponse(BOOL status);

    //! Send client format list to server
    [[nodiscard]] static UINT SendClientFormatList(CliprdrClientContext *context);
    //! Receive client format list response from server
    [[nodiscard]] static UINT cbServerFormatListResponse(
        CliprdrClientContext* context,
        const CLIPRDR_FORMAT_LIST_RESPONSE* formatListResponse);

    //! Receive server format data request from server
    [[nodiscard]] static UINT cbServerFormatDataRequest(
        CliprdrClientContext* context,
        const CLIPRDR_FORMAT_DATA_REQUEST* formatDataRequest);
    //! Send server format data response from client
    [[nodiscard]] static UINT SendFormatDataResponse(
        CliprdrClientContext* context,
        const BYTE* data, size_t size);

private Q_SLOTS:
    //! Send client format data request to server
    void slotSendFormatDataRequest(
        CliprdrClientContext* context, UINT32 formatId, QString szMimeType);
private:
    //! Receive client format data response from server
    [[nodiscard]] static UINT cbServerFormatDataResponse(
        CliprdrClientContext* context,
        const CLIPRDR_FORMAT_DATA_RESPONSE* formatDataResponse);

#ifndef HAVE_FILE_INTERFACE
public:
    static UINT
    cb_cliprdr_server_file_contents_request(CliprdrClientContext* context,
                                            const CLIPRDR_FILE_CONTENTS_REQUEST* fileContentsRequest);
    static UINT
    cb_cliprdr_server_file_contents_response(CliprdrClientContext* context,
                                             const CLIPRDR_FILE_CONTENTS_RESPONSE* fileContentsResponse);

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
#endif

public Q_SLOTS:
    virtual void slotClipBoardChanged();

Q_SIGNALS:
    //!
    //! \brief Notify clipboard get data from server
    //! \param pData: data pointer
    //! \param nLen: data length
    //! \param formatId: format id
    //! \param szMimeType: source format mime type
    //! if(pData == nullptr && nLen == 0) is Notify clipboard program has exited
    void sigServerFormatData(const BYTE* pData, UINT32 nLen,
                             UINT32 formatId, QString szMimeType);
    void sigServerFileContentsRespose(UINT32 streamId, QByteArray& data);

private:
    static CClipboardFreeRDP* GetThis(CliprdrClientContext* context);

private:
    CBackendFreeRDP* m_pConnect;
    CliprdrClientContext* m_pCliprdrClientContext;

    friend CClipboardMimeData;
    QList<qint32> m_lstClipboardMimeDataId;
    // Clipboard interface provided by winpr
    // 用于剪切板格式的转换
    wClipboard* m_pClipboard;
    // Client request format data
    UINT32 m_RequestFormatId;
    QString m_szRequestMime;

    // File
#if HAVE_FILE_INTERFACE
    CliprdrFileContext* m_pFileContext;
#else
    UINT32 m_FileCapabilityFlags;
    bool m_bFileSupported; // Whether is server support file
    BOOL m_bFileFormatsRegistered;
#endif
};
