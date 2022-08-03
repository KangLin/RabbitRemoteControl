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
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
    
    // CHook interface
public:
    virtual int RegisterKeyboard() override;
    virtual int UnRegisterKeyboard() override;
};

#endif // CHOOKUNIX_H
