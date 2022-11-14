#ifndef CHOOKUNIX_H
#define CHOOKUNIX_H

#include "Hook.h"

class CHookUnix : public CHook
{
public:
    explicit CHookUnix(QObject *parent = nullptr);
    virtual ~CHookUnix();

    // QAbstractNativeEventFilter interface
public:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;
#else
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
#endif
    
    // CHook interface
public:
    virtual int RegisterKeyboard() override;
    virtual int UnRegisterKeyboard() override;
};

#endif // CHOOKUNIX_H
