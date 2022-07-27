#ifndef CPARAMETERVIEWER_H
#define CPARAMETERVIEWER_H

#pragma once

#include "Parameter.h"

/*!
 * \brief The CParameterViewer class
 *        The parameters is valid in the viewer.
 */
class VIEWER_EXPORT CParameterViewer : public CParameter
{
    Q_OBJECT
public:
    explicit CParameterViewer(QObject *parent = nullptr);
    virtual ~CParameterViewer();

    virtual int Load(QSettings &set) override;
    virtual int Save(QSettings &set) override;

public:
    bool GetHookKeyboard() const;
    void SetHookKeyboard(bool newHookKeyboard);
Q_SIGNALS:
    void sigHookKeyboardChanged();
private:
    bool m_bHookKeyboard;
    Q_PROPERTY(bool HookKeyboard READ GetHookKeyboard WRITE SetHookKeyboard
               NOTIFY sigHookKeyboardChanged)
};

#endif // CPARAMETERVIEWER_H
