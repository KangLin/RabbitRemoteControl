#ifndef PARAMETERSCREENCAPTURE_H
#define PARAMETERSCREENCAPTURE_H

#pragma once

#include <QCapturableWindow>

#include "ParameterOperate.h"

class CParameterScreenCapture : public CParameterOperate
{
    Q_OBJECT
public:
    explicit CParameterScreenCapture(QObject *parent = nullptr);
    virtual ~CParameterScreenCapture();
    
    CParameterRecord m_Record;
    enum class TARGET {
        Screen,
        Window,
        CustomRectangle,
        Curstomize
    };
    Q_ENUM(TARGET)
    const TARGET GetTarget() const;
    int SetTarget(TARGET tg);

    enum class OPERATE {
        Shot,
        Record
    };
    Q_ENUM(OPERATE)
    const OPERATE GetOperate() const;
    int SetOperate(OPERATE opt);

    const int GetScreen() const;
    int SetScreen(int screen);

    const QCapturableWindow GetWindow() const;
    int SetWindow(QCapturableWindow w);

private:
    TARGET m_Target;
    OPERATE m_Operate;
    int m_nScreen;
    QCapturableWindow m_WindowCapture;

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
    
    // CParameterOperate interface
protected slots:
    virtual void slotSetGlobalParameters() override;
};

#endif // PARAMETERSCREENCAPTURE_H
