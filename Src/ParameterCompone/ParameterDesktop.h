// Author: Kang Lin <kl222@126.com>

#pragma once
#include "ParameterNet.h"
#include "ParameterProxy.h"
#include "ParameterRecord.h"
#include "FrmViewer.h"

/*!
 * \~chinese
 * \brief
 * 远程桌面的基础参数接口。包括基本参数（网络参数等）。
 * 所有远程桌面插件的参数都应从此类派生。
 *
 * \section section_Use_CParameterDesktop 使用远程桌面基础参数
 *  - 因为插件都有一些公共参数（例如：网络地址等），所以插件参数请从 CParameterDesktop 派生。
 *    \snippet Plugins/FreeRDP/Client/ParameterFreeRDP.h Declare CParameterFreeRDP
 *  - 请在 COperate 派生类的构造函数中实例化参数。
 *    \snippet Plugins/FreeRDP/Client/OperateFreeRDP.h Initialize parameter
 *  - 调用 COperate::SetParameter 设置参数指针。
 *    \snippet Plugins/FreeRDP/Client/OperateFreeRDP.cpp Set the parameter
 *  - 因为 COperate 实例运行在主线程中（UI线程），所以插件参数实例也在主线程中。
 *
 * \~english
 * \brief
 *  The interface of remote desktop base parameters.
 *  include base parameters(network etc).
 *  All plug-in remote desktop parameters should be derived from this class.
 * \section section_Use_CParameterDesktop Use remote desktop parameter
 *  - The plugins have some common parameters (e.g., network address, etc.),
 *    please derive the plugin parameters from CParameterDesktop.
 *    \snippet Plugins/FreeRDP/Client/ParameterFreeRDP.h Declare CParameterFreeRDP
 *  - Instantiate the parameters in the constructor of
 *    the derived class of COperate.
 *    \snippet Plugins/FreeRDP/Client/OperateFreeRDP.h Initialize parameter
 *  - Call COperate::SetParameter to set
 *    \snippet Plugins/FreeRDP/Client/OperateFreeRDP.cpp Set the parameter
 *  - Because COperate is running main thread(UI thread),
 *    so that the parameter is running main thread.
 *
 * \~
 * \ingroup CLIENT_PARAMETER_COMPONE 
 * \ingroup gOperateDesktop
 */
class PLUGIN_EXPORT CParameterDesktop : public CParameterOperate
{
    Q_OBJECT
    Q_PROPERTY(QString Name READ GetName WRITE SetName NOTIFY sigNameChanged)
    Q_PROPERTY(QString ServerName READ GetServerName WRITE SetServerName)
    Q_PROPERTY(bool ShowServerName READ GetShowServerName
                   WRITE SetShowServerName NOTIFY sigShowServerNameChanged)
    Q_PROPERTY(bool OnlyView READ GetOnlyView WRITE SetOnlyView)
    Q_PROPERTY(bool LocalCursor READ GetLocalCursor WRITE SetLocalCursor)

public:
    explicit CParameterDesktop(QObject* parent = nullptr);
    explicit CParameterDesktop(CParameterOperate* parent,
                            const QString& szPrefix = QString());

    const QString GetServerName() const;
    void SetServerName(const QString& szName);

    bool GetShowServerName() const;
    void SetShowServerName(bool NewShowServerName);

    CParameterNet m_Net;
    CParameterProxy m_Proxy;
    CParameterRecord m_Record;

    bool GetOnlyView() const;
    void SetOnlyView(bool only);
    
    const bool GetLocalCursor() const;
    void SetLocalCursor(bool cursor);
    
    const bool GetCursorPosition() const;
    void SetCursorPosition(bool pos);
    
    virtual const bool GetClipboard() const;
    virtual void SetClipboard(bool c);
    
    bool GetSupportsDesktopResize() const;
    void SetSupportsDesktopResize(bool newSupportsDesktopResize);
    
    bool GetLedState() const;
    void SetLedState(bool state);

    bool GetEnableLocalInputMethod() const;
    void SetEnableLocalInputMethod(bool enable);

Q_SIGNALS:
    void sigShowServerNameChanged();
    void sigEnableInputMethod(bool bEnable);

private:
    int Init();
    
    QString m_szServerName;
    bool m_bShowServerName;
    bool m_bOnlyView;
    bool m_bLocalCursor;
    bool m_bCursorPosition;
    bool m_bClipboard;
    bool m_bSupportsDesktopResize;
    bool m_bLedState;
    bool m_bEnableLocalInputMethod;

    // Viewer
public:
    CFrmViewer::ADAPT_WINDOWS GetAdaptWindows();
    void SetAdaptWindows(CFrmViewer::ADAPT_WINDOWS aw);
Q_SIGNALS:
    void sigAdaptWindowsChanged(CFrmViewer::ADAPT_WINDOWS aw);
private:
    CFrmViewer::ADAPT_WINDOWS m_AdaptWindows;
    Q_PROPERTY(CFrmViewer::ADAPT_WINDOWS AdaptWindows READ GetAdaptWindows WRITE SetAdaptWindows NOTIFY sigAdaptWindowsChanged)
public:
    double GetZoomFactor() const;
    void SetZoomFactor(double newZoomFactor);
Q_SIGNALS:
    void sigZoomFactorChanged(double newZoomFactor);
private:
    double m_dbZoomFactor;
    Q_PROPERTY(double ZoomFactor READ GetZoomFactor WRITE SetZoomFactor NOTIFY sigZoomFactorChanged)

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
    
    // CParameterOperate interface
    virtual void slotSetPluginParameters() override;
};
