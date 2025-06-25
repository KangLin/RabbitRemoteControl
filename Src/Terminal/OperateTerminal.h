// Author: Kang Lin <kl222@126.com>

#pragma once

#include "Operate.h"
#include "Backend.h"
#include "ParameterTerminalBase.h"

class CBackendThread;
/*!
 * \~chinese 终端接口
 * \~english Terminal interface
 * \~
 * \ingroup PLUGIN_API
 */
class PLUGIN_EXPORT COperateTerminal : public COperate
{
    Q_OBJECT
public:
    explicit COperateTerminal(CPlugin *parent);
    virtual ~COperateTerminal() override;
    
    /*!
     * \~chinese
     * 新建后端实例。它的所有者是调用者，
     * 如果调用者不再使用它，调用者必须负责释放它。
     *
     * \~english New CBackend. the ownership is caller.
     *        if don't use, the caller must delete it.
     * \~see CBackendThread
     */
    [[nodiscard]] virtual CBackend* InstanceBackend() = 0;
    //! Get parameters
    [[nodiscard]] virtual CParameterTerminalBase* GetParameter();
    /*!
     * \~chinese 设置参数指针
     * \note 先建立参数对象，然后构造函数或 Initial() 中调用此函数设置参数指针
     * \~english Set parameter pointer
     * \note Establish the parameter object first, and then calls this function
     *       in the constructor or Initial() to set the parameter pointer
     */
    virtual int SetParameter(CParameterTerminalBase *pPara);

    [[nodiscard]] QWidget* GetViewer() override;
    [[nodiscard]] virtual const qint16 Version() const override;

public Q_SLOTS:
    virtual int Start() override;
    virtual int Stop() override;

protected:
    virtual int Initial() override;
    virtual int Clean() override;
    virtual int SetGlobalParameters(CParameterPlugin *pPara) override;

private Q_SLOTS:
    void slotTerminalTitleChanged();
    void slotZoomReset();
    void slotCustomContextMenu(const QPoint & pos);
    void slotActivateUrl(const QUrl& url, bool fromContextMenu);
    void slotTermKeyPressed(QKeyEvent* e);
    void slotFocusIn();
protected Q_SLOTS:
    void slotUpdateParameter(COperate *pOperate);

protected:
    virtual int Load(QSettings &set) override;
    virtual int Save(QSettings &set) override;

    QTermWidget* m_pConsole;
    CBackendThread* m_pThread;
private:
    CParameterTerminalBase* m_pParameters;
};
