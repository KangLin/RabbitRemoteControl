// Author: Kang Lin <kl222@126.com>

#ifndef CHOOK_H
#define CHOOK_H

#include <QObject>

/*!
 * \brief The class is the HOOK abstract class.
 *        Call CHook::GetHook get a instance of the class.
 * \note The class is only used by Client.
 */
class CHook : public QObject
{
    
public:
    explicit CHook(QObject *parent = nullptr);
    virtual ~CHook();

    static CHook* GetHook(QObject *parent = nullptr);

    virtual int RegisterKeyboard() = 0;
    virtual int UnRegisterKeyboard() = 0;   
};

#endif // CHOOK_H
