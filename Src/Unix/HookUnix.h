// Author: Kang Lin <kl222@126.com>

#ifndef CHOOKUNIX_H
#define CHOOKUNIX_H

#include "Hook.h"
class CParameterClient;
class CHookUnix : public CHook
{
public:
    explicit CHookUnix(CParameterPlugin* para, QObject *parent = nullptr);
    virtual ~CHookUnix();

    // CHook interface
public:
    virtual int RegisterKeyboard() override;
    virtual int UnRegisterKeyboard() override;
};

#endif // CHOOKUNIX_H
