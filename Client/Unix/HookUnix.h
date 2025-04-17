#ifndef CHOOKUNIX_H
#define CHOOKUNIX_H

#include "Hook.h"

class CHookUnix : public CHook
{
public:
    explicit CHookUnix(QObject *parent = nullptr);
    virtual ~CHookUnix();

    // CHook interface
public:
    virtual int RegisterKeyboard() override;
    virtual int UnRegisterKeyboard() override;
};

#endif // CHOOKUNIX_H
