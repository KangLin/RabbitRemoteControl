#ifndef CHOOKUNIXNATIVEEVENTFILTER_H
#define CHOOKUNIXNATIVEEVENTFILTER_H

#include <QAbstractNativeEventFilter>

class CHookUnixNativeEventFilter : public QAbstractNativeEventFilter
{
public:
    CHookUnixNativeEventFilter();
    
    // QAbstractNativeEventFilter interface    
public:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;
#else
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
#endif

};

#endif // CHOOKUNIXNATIVEEVENTFILTER_H
