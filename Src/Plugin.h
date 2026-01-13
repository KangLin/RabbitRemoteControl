// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QIcon>
#include <QTranslator>
#include "Operate.h"

/*!
 * \~chinese 插件接口
 * \~english Plugin interface
 * \ingroup PLUGIN_API
 */
class PLUGIN_EXPORT CPlugin : public QObject
{
    Q_OBJECT

public:
    explicit CPlugin(QObject *parent = nullptr);
    virtual ~CPlugin();

    enum class TYPE{
        RemoteDesktop,
        Terminal,
        Client,
        Service,
        FileTransfers,
        NetworkTools,
        Tools,
        Custom = 1000 //!\note please override `virtual const QString TypeName(const TYPE t) const`
    };
    Q_ENUM(TYPE)
    [[nodiscard]] virtual const TYPE Type() const = 0;
    [[nodiscard]] virtual const QString TypeName(const TYPE t) const;
    /// \~chinese 标识。默认： Type() + ":" + Protocol() + ":" + Name()
    /// \~english ID. Default: Type() + ":" + Protocol() + ":" + Name()
    [[nodiscard]] virtual const QString Id() const;
    /// \~chinese 协议
    /// \~english Plugin Protocol
    [[nodiscard]] virtual const QString Protocol() const = 0;
    /// \~chinese 插件名，这个名一定要与工程名(${PROJECT_NAME})相同。
    ///           翻译文件(${PROJECT_NAME}_*.ts)）名与其相关。
    /// \~english This name must be the same as the project name (${PROJECT_NAME}).
    ///           The translation file (${PROJECT_NAME}_*.ts)) name is associated with it. 
    [[nodiscard]] virtual const QString Name() const = 0;
    /// \~chinese 在界面上显示的名称
    /// \~english The plugin display name
    [[nodiscard]] virtual const QString DisplayName() const;
    /// \~chinese 描述
    /// \~english Plugin description.
    [[nodiscard]] virtual const QString Description() const = 0;
    //! Version
    [[nodiscard]] virtual const QString Version() const = 0;
    [[nodiscard]] virtual const QIcon Icon() const = 0;
    /*!
     * \~chinese 显示更多细节。
     *    例如： 在关于对话框或日志中显示。
     *    包括插件的依赖库的版本信息和描述
     * \~english Display more information in About dialog or log.
     *    eg: Include the plugin depends on the library version and description
     * \~
     * \see CPluginFreeRDP::Details()
     */
    [[nodiscard]] virtual const QString Details() const;

Q_SIGNALS:
    /*!
     * \~chinese 当在插件中新建操作时，触发此信号
     * \param pOperate: 新建的操作
     * \param bOpenSettingsDialog:
     *           - true: 打开设置对话框
     *           - false: 不打开设置对话框
     *
     * \~english
     * \brief This signal is triggered when a new operate is created in the plugin.
     * \param pOperate: a new operate is created.
     * \param bOpenSettingsDialog:
     *           - true: open the settings dialog
     *           - false: don't open the settings dialog
     */
    void sigNewOperate(COperate* pOperate, bool bOpenSettingsDialog);

protected:
    /*!
     * \~chinese
     * \brief 新建 COperate 实例。仅由 CManager 调用
     * \snippet Src/Manager.cpp CManager CreateOperate
     * \return 返回 COperate 指针, 它的所有者是调用者。
     * \note 此函数新建一个对象指针，
     *       <b>调用者必须负责在用完后释放指针</b>。
     * \param szId 连接 ID
     *
     * \~english
     * \brief New COperate instance. Only is called by CManager
     * \snippet Src/Manager.cpp CManager CreateOperate
     * \return Return COperate pointer, the owner is caller
     * \note The function new object pointer,
     *       <b>the caller must delete it when don't use it</b>.
     * \param szId
     *
     * \~
     * \see CManager::CreateOperate CManager::LoadOperate
     *
     */
    Q_INVOKABLE virtual COperate* CreateOperate(
        const QString& szId, CParameterPlugin* para);
    [[nodiscard]] virtual COperate* OnCreateOperate(const QString& szId) = 0;
    /*!
     * Delete COperate
     */
    Q_INVOKABLE virtual int DeleteOperate(COperate* p);
    /*!
     * \~chinese 得到设置此插件全局参数的窗口
     * \return 设置参数窗口。如果不需要，则返回 nullptr。此窗口的所有者为调用者。
     * \note 此窗口必须从 CParameterUI 派生。
     * 用法示例：
     * \snippet Src/Manager.cpp  Get the widget to set global parameters for the plugin
     *
     * \~english Get the widget to set global parameters for the plugin
     * \return Sets the parameters widget.
     *         Returns nullptr if no parameters are needed.
     *         This widget belongs to the caller.
     * \note The widget must be a derivative of CParameterUI.
     *         Eg: CParameterPluginUI
     *
     * \snippet Src/ParameterPluginUI.h Accept parameters
     * Usage:
     * \snippet Src/Manager.cpp  Get the widget to set global parameters for the plugin
     * \~
     * \see CManager::GetSettingsWidgets
     */
    Q_INVOKABLE virtual QWidget* GetSettingsWidget(QWidget *parent);

private:
    QSharedPointer<QTranslator> m_Translator;
    /*!
     * \~chinese 初始化翻译资源，仅由 CManager 调用。
     *   因为它调用了 Name() ，所以不能在此类的构造函数中直接调用。
     *
     * \~english Initialize the translation resource,
     *   which is only called by CManager.
     *   Because it calls Name(),
     *   So it can't be called directly in the constructor of this class.
     *
     * \see CManager::AppendPlugin
     */
    Q_INVOKABLE int InitTranslator();
};

QT_BEGIN_NAMESPACE
#define CPlugin_iid "KangLinStudio.Rabbit.RemoteControl.CPlugin"
Q_DECLARE_INTERFACE(CPlugin, CPlugin_iid)
QT_END_NAMESPACE
