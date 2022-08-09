#include "HookUnix.h"
#include <QDebug>
#include <QApplication>
#include <QWidget>
#include <QAbstractEventDispatcher>

#include "xcb/xcb.h"

CHook* CHook::GetHook(QObject *parent)
{
    return new CHookUnix(parent);
}

CHookUnix::CHookUnix(QObject *parent)
    : CHook(parent)
{
    //TODO:
    //RegisterKeyboard();
}

CHookUnix::~CHookUnix()
{
    UnRegisterKeyboard();
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

bool CHookUnix::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    if (eventType == "xcb_generic_event_t") {
        xcb_generic_event_t* e = static_cast<xcb_generic_event_t *>(message);
        switch (e->response_type & ~0x80) {
        case XCB_KEY_PRESS: {
              xcb_key_press_event_t *ev = (xcb_key_press_event_t *)e;
              print_modifiers(ev->state);
         
              WId id = QApplication::focusWidget()->parentWidget()->window()->winId();
                                
              qDebug("Key [%ld] pressed in window %ld : %ld\n",
                     ev->detail,
                      ev->event, id);
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

        //qDebug() << "CClient::nativeEventFilter" << message << *result;
    }
    return false;
}

int CHookUnix::RegisterKeyboard()
{
    QCoreApplication::eventDispatcher()->installNativeEventFilter(this);
    return 0;
}

int CHookUnix::UnRegisterKeyboard()
{
    QCoreApplication::eventDispatcher()->removeNativeEventFilter(this);
    return 0;
}
