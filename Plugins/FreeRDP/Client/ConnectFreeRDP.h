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

    static BOOL cbGlobalInit();
    static void cbGlobalUninit();
    static BOOL cbClientNew(freerdp* instance, rdpContext* context);
    static void cbClientFree(freerdp* instance, rdpContext* context);
    static int cbClientStart(rdpContext* context);
    static int cbClientStop(rdpContext* context);

    /**
     * Callback given to freerdp_connect() to process the pre-connect operations.
     * It will fill the rdp_freerdp structure (instance) with the appropriate options to use for the
     * connection.
     *
     * @param instance - pointer to the rdp_freerdp structure that contains the connection's parameters,
     * and will be filled with the appropriate information.
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

    static void OnChannelConnectedEventHandler(void* context,
                                           #if FreeRDP_VERSION_MAJOR >= 3
                                               const
                                           #endif
                                               ChannelConnectedEventArgs* e);
    static void OnChannelDisconnectedEventHandler(void* context,
                                              #if FreeRDP_VERSION_MAJOR >= 3
                                                  const
                                              #endif
                                                  ChannelDisconnectedEventArgs* e);
    
	static BOOL cb_authenticate(freerdp* instance,
                               char** username, char** password, char** domain);
	static BOOL cb_GatewayAuthenticate(freerdp* instance,
                               char** username, char** password, char** domain);
#if FreeRDP_VERSION_MAJOR >= 3
    static BOOL cb_authenticate_ex(freerdp* instance,
                                   char** username, char** password,
                                   char** domain, rdp_auth_reason reason);
    static BOOL cb_choose_smartcard(freerdp* instance,
                                    SmartcardCertInfo** cert_list,
                                    DWORD count,
                                    DWORD* choice, BOOL gateway);
#endif
    /** @brief Callback used if user interaction is required to accept
	 *         a certificate.
	 *
	 *  @param instance         Pointer to the freerdp instance.
	 *  @param data             Pointer to certificate data (full chain) in PEM format.
	 *  @param length           The length of the certificate data.
	 *  @param hostname         The hostname connecting to.
	 *  @param port             The port connecting to.
	 *  @param flags            Flags of type VERIFY_CERT_FLAG*
	 *
	 *  @return 1 to accept and store a certificate, 2 to accept
	 *          a certificate only for this session, 0 otherwise.
	 */
    static int cb_verify_x509_certificate(freerdp* instance,
                                          const BYTE* data, size_t length,
                                const char* hostname, UINT16 port, DWORD flags);
    static DWORD cb_verify_certificate_ex(freerdp* instance,
                                                  const char* host, UINT16 port,
                                   const char* common_name, const char* subject,
                      const char* issuer, const char* fingerprint, DWORD flags);
    static DWORD cb_verify_changed_certificate_ex(freerdp* instance,
                                                  const char* host, UINT16 port,
                                   const char* common_name, const char* subject,
                                    const char* issuer, const char* fingerprint,
                                const char* old_subject, const char* old_issuer,
                                      const char* old_fingerprint, DWORD flags);
    static BOOL cb_present_gateway_message(freerdp* instance,
                                           UINT32 type, BOOL isDisplayMandatory,
                  BOOL isConsentMandatory, size_t length, const WCHAR* message);

    static BOOL cb_begin_paint(rdpContext* context);
    static BOOL cb_end_paint(rdpContext* context);
    static BOOL cb_desktop_resize(rdpContext* context);
    static BOOL cb_play_bell_sound(rdpContext* context,
                                   const PLAY_SOUND_UPDATE* play_sound);
    static BOOL cb_keyboard_set_indicators(rdpContext* context, UINT16 led_flags);
    static BOOL cb_keyboard_set_ime_status(rdpContext* context, UINT16 imeId, UINT32 imeState,
                                        UINT32 imeConvMode);

    static UINT32 GetImageFormat(QImage::Format format);
    static const char* GetTitle(freerdp *instance);

private:
    UINT32 GetImageFormat();
    BOOL UpdateBuffer(INT32 x, INT32 y, INT32 w, INT32 h);

    int RedirectionSound();
    int RedirectionMicrophone();
    int RedirectionDriver();
    int RedirectionPrinter();
    int RedirectionSerial();

    bool SendMouseEvent(UINT16 flags, QPoint pos);

    // CConnect interface
public Q_SLOTS:
    virtual void slotClipBoardChanged() override;
    
public Q_SLOTS:
    virtual void slotMousePressEvent(Qt::MouseButtons buttons, QPoint pos) override;
    virtual void slotMouseReleaseEvent(Qt::MouseButton button, QPoint pos) override;
    virtual void slotMouseMoveEvent(Qt::MouseButtons buttons, QPoint pos) override;
    virtual void slotWheelEvent(Qt::MouseButtons buttons, QPoint pos, QPoint angleDelta) override;
    virtual void slotKeyPressEvent(int key, Qt::KeyboardModifiers modifiers) override;
    virtual void slotKeyReleaseEvent(int key, Qt::KeyboardModifiers modifiers) override;

private:
    // CConnect interface
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;
    virtual int OnProcess() override;
    
private:
    struct ClientContext{
        #if FreeRDP_VERSION_MAJOR >= 3
            rdpClientContext Context;
        #else
            rdpContext Context;
        #endif
        CConnectFreeRDP* pThis;
    };
    ClientContext* m_pContext;
    CParameterFreeRDP* m_pParameter;
    CConnecterFreeRDP* m_pConnecter;
	RDP_CLIENT_ENTRY_POINTS m_ClientEntryPoints;
        
    QImage m_Image;

    CClipboardFreeRDP m_ClipBoard;
    CCursorFreeRDP m_Cursor;
    friend class CCursorFreeRDP;
};

#endif // CCONNECTFREERDP_H
