// Author: Kang Lin <kl222@126.com>

#ifndef CNATIVEEVENTFILTER_H
#define CNATIVEEVENTFILTER_H

#pragma once

#include <QAbstractNativeEventFilter>
#include <QEvent>
#include <xcb/xcb_keysyms.h>

#include "ParameterPlugin.h"
#include "DesktopShortcuts.h"

class CNativeEventFilterUnix : public QAbstractNativeEventFilter
{
public:
    CNativeEventFilterUnix(CParameterPlugin *pPara);
    virtual ~CNativeEventFilterUnix();

    // QAbstractNativeEventFilter interface    
public:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;
#else
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
#endif

    xcb_connection_t* m_pConnect;
    xcb_key_symbols_t * m_pKeySymbols;
    int GetKeySym(xcb_key_press_event_t* event, xcb_keysym_t& keysym);
    bool HandleEvent(xcb_generic_event_t* event);
    bool HandleKey(xcb_keysym_t keysym, QEvent::Type type, Qt::KeyboardModifiers modifiers);
    
    static void RestoreSuperKeyShortcuts();
    static void DisableSuperKeyShortcuts();
private:
    CParameterPlugin *m_pParameterPlugin;
};

#endif // CNATIVEEVENTFILTER_H
