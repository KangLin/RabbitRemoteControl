#ifndef CNATIVEEVENTFILTER_H
#define CNATIVEEVENTFILTER_H

#pragma once

#include <QAbstractNativeEventFilter>
#include <QEvent>
#if defined(Q_OS_LINUX)
#include <xcb/xcb_keysyms.h>
#endif

#include "ParameterClient.h"

class CNativeEventFilter : public QAbstractNativeEventFilter
{
public:
    CNativeEventFilter(CParameterClient *pParaClient);
    virtual ~CNativeEventFilter();

    // QAbstractNativeEventFilter interface    
public:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;
#else
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
#endif

#if defined(Q_OS_LINUX)
    xcb_connection_t* m_pConnect;
    xcb_key_symbols_t * m_pKeySymbols;
    int GetKeySym(xcb_key_press_event_t* event, xcb_keysym_t& keysym);
    bool HandleEvent(xcb_generic_event_t* event);
    bool HandleKey(xcb_keysym_t keysym, QEvent::Type type, Qt::KeyboardModifiers modifiers);
#endif

private:
    CParameterClient *m_pParameterClient;
};

#endif // CNATIVEEVENTFILTER_H
