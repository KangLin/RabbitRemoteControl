// Author: Kang Lin <kl222@126.com>
#pragma once

#include <QSpinBox>
#include "Operate.h"
#include "Backend.h"
#include "ParameterBase.h"
#include "FrmScroll.h"

class CBackendThread;
/*!
 * \~chinese
 * \brief 默认启动一个后台线程。实现一个后台线程处理一个连接。
 *        可与插件接口从 CPlugin 派生的插件一起使用，用于连接是阻塞模型的。
 * \details
 * 原理：在 Start() 中启动一个后台线程 CBackendThread 。
 *      在线程中调用 InstanceBackend() 实例化 CBackend ，
 *      并在 CBackend::Start() 启动定时器，
 *      此定时器在后台线程中调用，通过对定时器的操作，实现一个非 Qt 事件循环(可能会阻塞)，详见 CBackend 。
 *      并且 CBackend 仍然支持 Qt 事件（QObject 的 信号 － 槽 机制）(非阻塞）。
 *
 * 序列图：\image html docs/Image/PluginClientBlockSequenceDiagram.svg
 *
 * \note 此接口仅由插件实现。 \n
 *      此接口实例运行在主线程(UI线程)中。 \n
 *      具体的插件需要实现下面接口：
 *         1. 实现 InstanceBackend() ，生成连接对象。连接对象运行在后台线程中。
 *
 * \~english
 * \brief It starts a background thread by default.
 *        It implements a background thread to handle a connection.
 *        Can be used with plugins whose plugin interface
 *        derives from CPluginClient for connection is blocking model.
 * \details 
 *     Principle: Start a background thread (CBackendThread) in Start() .
 *     Call InstanceBackend() in the thread to instantiate CBackend,
 *     and start the timer in CBackend::Start().
 *     This timer is called in the background thread.
 *     Through the operation of the timer,
 *     start a non-Qt event loop (that is, normal loop processing. May block), See CBackend.
 *     And CBackend supports the Qt event (the signal-slot mechanism of QObject. no-block).
 *
 *     Sequen diagram: \image html docs/Image/PluginClientBlockSequenceDiagram.svg
 *
 * \note The interface only is implemented by plug-in \n
 *     The specific plug-in needs to implement the following interface. 
 *         1. Implement InstanceConnect() . Generate a connection object.
 *            The connection object runs in a background thread.
 *
 * \~
 * \see CBackend CBackendThread COperate CPlugin CFrmViewer
 * \ingroup LIBAPI
 */
class COperateDesktop : public COperate
{
    Q_OBJECT

public:
    /*!
     * \~chinese
     * \param plugin: 此指针必须是相应的 CPlugin 派生类的实例指针
     * \note 如果需要插件的全局参数(CParameterPlugin)
     *       请在其派生类的构造函数（或者　Initial() ）中实例化参数，
     *       并在　Initial()　调用 COperateDesktop::SetParameter 设置参数指针。
     *       如果参数不需要 CParameterClient ，
     *       那请在其派生类重载 COperate::SetParameterPlugin 并忽略它。
     *
     * \~english
     * \param plugin: The plugin pointer must be specified as
     *        the corresponding CPlugin derived class
     * \note If request the global parameters of the plugin,
     *       Please instantiate the parameters and call COperateDesktop::SetParameter
     *       in the derived class( or Initial() ) to set the parameters pointer.
     *       If you are sure the parameter does not need CParameterClient.
     *       please overload the COperate::SetParameterPlugin in the derived class. don't set it. Add commentMore actions
     *
     * \~
     * \see CManager::CreateConnecter SetParameterPlugin SetParameter
     *      CParameterConnecter CParameterClient
     */
    explicit COperateDesktop(CPlugin *plugin);
    virtual ~COperateDesktop();

    /*!
     * \~chinese
     * 新建后端实例。它的所有者是调用者，
     * 如果调用者不再使用它，调用者必须负责释放它。
     *
     * \~english New CBackend. the ownership is caller.
     *        if don't use, the caller must delete it.
     * \~see CBackendThread
     */
    Q_INVOKABLE [[nodiscard]] virtual CBackend* InstanceBackend() = 0;
    /*!
     * \brief Get parameter
     */
    [[nodiscard]] virtual CParameterBase* GetParameter();
    /*!
     * \~chinese 设置参数指针
     * \note 先建立参数，然后构造函数或 Initial() 中调用此函数设置参数指针
     * \~english Set parameter pointer
     * \note establishes the parameter first, and then calls this function
     *       in the constructor or Initial() to set the parameter pointer
     */
    virtual int SetParameter(CParameterBase* p);

    [[nodiscard]] virtual const QString Id() override;
    [[nodiscard]] virtual const QString Name() override;
    [[nodiscard]] virtual const QString Description() override;
    [[nodiscard]] virtual const QString Protocol() const override;
    [[nodiscard]] virtual const qint16 Version() const override;
    [[nodiscard]] virtual const QIcon Icon() const override;
    [[nodiscard]] virtual QWidget *GetViewer() override;

public Q_SLOTS:
    virtual int Start() override;
    virtual int Stop() override;
    virtual void slotScreenShot();

protected:
    virtual int SetParameterPlugin(CParameterPlugin *pPara) override;

private Q_SLOTS:
    //! emit by zoom menu in the class
    void slotValueChanged(int v);
    void slotShortcutCtlAltDel();
    void slotShortcutLock();

protected:
    virtual int Initial() override;
    virtual int Clean() override;
    /*!
     * \~chinese \brief 加载参数
     * \~english \brief Load parameters
     */
    virtual int Load(QSettings &set) override;
    /*!
     * \~chinese 保存参数
     * \~english Save parameters
     */
    virtual int Save(QSettings &set) override;

    virtual int InitialMenu();

private:
    CParameterBase* m_pPara;
    CBackendThread* m_pThread;
    CFrmViewer *m_pFrmViewer;
    CFrmScroll* m_pScroll;

protected:
    /*!
     * \~chinese
     * \brief 当前连接名（远程桌面的名称，如果没有，则是 IP:端口）。例如：服务名或 IP:端口
     * \return 返回服务名
     *
     * \~english
     * \brief Current connect server name
     *        (remote desktop name, if not present, then IP:PORT).
     *        eg: Server name or Ip:Port
     * \return Current connect server name.
     */
    [[nodiscard]] virtual QString ServerName();
private:
    QString m_szServerName;
private Q_SLOTS:
    //! \~chinese \note 仅由 CBackendDesktop::SetConnect() 使用
    //! \~english \note The slot only is used by CBackendDesktop::SetConnect()
    virtual void slotSetServerName(const QString &szName);

protected:
    QAction* m_pMenuZoom;
    QAction* m_pZoomToWindow;
    QAction* m_pZoomAspectRatio;
    QAction* m_pZoomOriginal;
    QAction* m_pZoomIn;
    QAction* m_pZoomOut;
    QSpinBox* m_psbZoomFactor;
    QAction* m_pScreenShot;

#if HAVE_QT6_RECORD
protected:
    QAction* m_pRecord;
    QAction* m_pRecordPause;
public Q_SLOTS:
    void slotRecorderStateChanged(QMediaRecorder::RecorderState state);
Q_SIGNALS:
    /*! emit by record menu in the class
     *  \see slotRecord
     */
    void sigRecord(bool bRecord);
    void sigRecordPause(bool bPause);
private Q_SLOTS:
    /*!
     * \brief Record action
     * \param checked
     */
    virtual void slotRecord(bool checked);
#endif
};

