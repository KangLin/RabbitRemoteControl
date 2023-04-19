#include "HookUnixNativeEventFilter.h"
#include "xcb/xcb.h"

#include <QApplication>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Client)

CHookUnixNativeEventFilter::CHookUnixNativeEventFilter()
{
    
}

void
print_modifiers (uint32_t mask)
{
    const char **mod, *mods[] = {
                          "Shift", "Lock", "Ctrl", "Alt",
                          "Mod2", "Mod3", "Mod4", "Mod5",
                          "Button1", "Button2", "Button3", "Button4", "Button5"
                      };
    qDebug() << "Modifier mask: ";
    for (mod = mods ; mask; mask >>= 1, mod++)
        if (mask & 1)
            qDebug() << *mod;
    putchar ('\n');
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
bool CHookUnixNativeEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
#else
bool CHookUnixNativeEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
#endif
{
    qDebug(Client) << "CClient::nativeEventFilter:" << eventType;
    if (eventType == "xcb_generic_event_t") {
        xcb_generic_event_t* e = static_cast<xcb_generic_event_t *>(message);
        switch (e->response_type & ~0x80) {
        case XCB_KEY_PRESS: {
            xcb_key_press_event_t *ev = (xcb_key_press_event_t *)e;
            print_modifiers(ev->state);
                        
            qDebug("Key [%ld] pressed in window %ld\n",
                   ev->detail,
                   ev->event);
            break;
        }
        case XCB_KEY_RELEASE: {
            xcb_key_release_event_t *ev = (xcb_key_release_event_t *)e;
            print_modifiers(ev->state);
            
            qDebug ("Key released in window %ld\n",
                   ev->event);
            break;
        }
        }
    }
    return false;
}
