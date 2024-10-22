#include "ParameterScreenCapture.h"

CParameterScreenCapture::CParameterScreenCapture(QObject *parent)
    : CParameterBase{parent}
    , m_Target(TARGET::Screen)
    , m_Operate(OPERATE::Shot)
    , m_nScreen(-1)
{}

CParameterScreenCapture::~CParameterScreenCapture()
{}

int CParameterScreenCapture::OnLoad(QSettings &set)
{
    return 0;
}

int CParameterScreenCapture::OnSave(QSettings &set)
{
    return 0;
}

const CParameterScreenCapture::TARGET CParameterScreenCapture::GetTarget() const
{
    return m_Target;
}

int CParameterScreenCapture::SetTarget(TARGET tg)
{
    if(m_Target == tg)
        return 0;
    m_Target = tg;
    SetModified(true);
    return 0;
}

const CParameterScreenCapture::OPERATE CParameterScreenCapture::GetOperate() const
{
    return m_Operate;
}

int CParameterScreenCapture::SetOperate(OPERATE opt)
{
    if(m_Operate == opt)
        return 0;
    m_Operate = opt;
    SetModified(true);
    return 0;
}

const int CParameterScreenCapture::GetScreen() const
{
    return m_nScreen;
}

int CParameterScreenCapture::SetScreen(int screen)
{
    if(m_nScreen == screen)
        return 0;
    m_nScreen = screen;
    SetModified(true);
    return 0;
}

const QCapturableWindow CParameterScreenCapture::GetWindow() const
{
    return m_WindowCapture;
}

int CParameterScreenCapture::SetWindow(QCapturableWindow w)
{
    if(m_WindowCapture == w)
        return 0;
    m_WindowCapture = w;
    SetModified(true);
    return 0;
}
