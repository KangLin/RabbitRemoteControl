// Author: Kang Lin <kl222@126.com>

#ifndef CHOOK_H
#define CHOOK_H

#include <QObject>
#include "ParameterPlugin.h"

/*!
 * \brief The class is the HOOK abstract class.
 *        Call CHook::GetHook get a instance of the class.
 * \note The class is only used by Client.
 */
class CHook : public QObject
{
public:
    static CHook* GetHook(CParameterPlugin* pPara, QObject *parent = nullptr);

    virtual int RegisterKeyboard();
    virtual int UnRegisterKeyboard();
    
public:
    // QObject interface
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

protected:
    CParameterPlugin* m_pParameterPlugin;

protected:
    explicit CHook(CParameterPlugin* pPara, QObject *parent = nullptr);
    virtual ~CHook();
};

#endif // CHOOK_H
