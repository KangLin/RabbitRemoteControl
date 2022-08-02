// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTFREERDP_H
#define CCONNECTFREERDP_H

#include "Connect.h"
#include "freerdp/freerdp.h"
#include "ClipboardFreeRDP.h"
#include "ConnecterFreeRDP.h"
#include "CursorFreeRDP.h"
#include <QSharedPointer>

class CConnectFreeRDP : public CConnect
{
    Q_OBJECT

public:
    explicit CConnectFreeRDP(CConnecterFreeRDP* pConnecter = nullptr,
                             QObject *parent = nullptr);
    virtual ~CConnectFreeRDP() override;

    static BOOL Client_global_init();
    static void Client_global_uninit();
    static BOOL Client_new(freerdp* instance, rdpContext* context);
    static void Client_free(freerdp* instance, rdpContext* context);
    static int Client_start(rdpContext* context);
    static int Client_stop(rdpContext* context);
    
    /**
     * Callback given to freerdp_connect() to process the pre-connect operations.
     * It will fill the rdp_freerdp structure (instance) with the appropriate options to use for the
     * connection.
     *
     * @param instance - pointer to the rdp_freerdp structure that contains the connection's parameters,
     * and will be filled with the appropriate informations.
     *
     * @return TRUE if successful. FALSE otherwise.
     * Can exit with error code XF_EXIT_PARSE_ARGUMENTS if there is an error in the parameters.
     */
    static BOOL cb_pre_connect(freerdp* instance);
    /**
     * Callback given to freerdp_connect() to perform post-connection operations.
     * It will be called only if the connection was initialized properly, and will continue the
     * initialization based on the newly created connection.
     */
    static BOOL cb_post_connect(freerdp* instance);
    static void cb_post_disconnect(freerdp* instance);
    static int cb_logon_error_info(freerdp* instance, UINT32 data, UINT32 type);
    
    static void OnChannelConnectedEventHandler(void* context, ChannelConnectedEventArgs* e);
    static void OnChannelDisconnectedEventHandler(void* context, ChannelDisconnectedEventArgs* e);
    
	static BOOL cb_authenticate(freerdp* instance, char** username, char** password, char** domain);
	static BOOL cb_GatewayAuthenticate(freerdp* instance, char** username, char** password, char** domain);
    static DWORD cb_verify_certificate_ex(freerdp* instance, const char* host, UINT16 port,
                                                 const char* common_name, const char* subject,
                                                 const char* issuer, const char* fingerprint, DWORD flags);
    static DWORD cb_verify_changed_certificate_ex(freerdp* instance, const char* host, UINT16 port,
                                                  const char* common_name, const char* subject,
                                                  const char* issuer, const char* fingerprint,
                                                  const char* old_subject, const char* old_issuer,
                                                  const char* old_fingerprint, DWORD flags);
    static BOOL cb_present_gateway_message(freerdp* instance, UINT32 type, BOOL isDisplayMandatory,
                                           BOOL isConsentMandatory, size_t length, const WCHAR* message);
    
    static BOOL cb_begin_paint(rdpContext* context);
    static BOOL cb_end_paint(rdpContext* context);
    static BOOL cb_desktop_resize(rdpContext* context);

    static UINT32 GetImageFormat(QImage::Format format);

private:
    UINT32 GetImageFormat();
    BOOL UpdateBuffer(INT32 x, INT32 y, INT32 w, INT32 h);

    // CConnect interface
public Q_SLOTS:
    virtual void slotClipBoardChange() override;
    
public Q_SLOTS:
    virtual void slotMousePressEvent(Qt::MouseButtons buttons, QPoint pos) override;
    virtual void slotMouseReleaseEvent(Qt::MouseButton button, QPoint pos) override;
    virtual void slotMouseMoveEvent(Qt::MouseButtons buttons, QPoint pos) override;
    virtual void slotWheelEvent(Qt::MouseButtons buttons, QPoint pos, QPoint angleDelta) override;
    virtual void slotKeyPressEvent(int key, Qt::KeyboardModifiers modifiers) override;
    virtual void slotKeyReleaseEvent(int key, Qt::KeyboardModifiers modifiers) override;
    
private:
    int RdpClientEntry(RDP_CLIENT_ENTRY_POINTS* pEntryPoints);
   
    // CConnect interface
    virtual int OnInit() override;
    virtual int OnClean() override;
    virtual int OnProcess() override;
    
private:
    struct ClientContext{
        rdpContext Context;
        CConnectFreeRDP* pThis;
    };
    ClientContext* m_pContext;
    CParameterFreeRDP* m_pParamter;
    CConnecterFreeRDP* m_pConnecter;
	RDP_CLIENT_ENTRY_POINTS m_ClientEntryPoints;
        
    QImage m_Image;
    QSharedPointer<CImage> m_Desktop;

    CClipboardFreeRDP m_ClipBoard;
    CCursorFreeRDP m_Cursor;
    friend CCursorFreeRDP;
};

#endif // CCONNECTFREERDP_H
