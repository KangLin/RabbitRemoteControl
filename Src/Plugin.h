// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QIcon>
#include <QTranslator>
#include "Operate.h"

/*!
 * \~chinese 插件接口
 * \~english Plugin interface
 * \ingroup LIBAPI
 */
class PLUGIN_EXPORT CPlugin : public QObject
{
    Q_OBJECT
public:
    explicit CPlugin(QObject *parent = nullptr);
    virtual ~CPlugin();

    enum class TYPE{
        Client,
        Service,
        Terminal,
        Tool,
        Custom = 1000 //!\note please override `virtual const QString TypeName(const TYPE t) const`
    };
    [[nodiscard]] virtual const TYPE Type() const = 0;
    [[nodiscard]] virtual const QString TypeName(const TYPE t) const;
    /// \~chinese 标识。默认： Type() + ":" + Protocol() + ":" + Name()
    /// \~english ID. Default: Type() + ":" + Protocol() + ":" + Name()
    [[nodiscard]] virtual const QString Id();
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

protected:
    /*!
     * \~chinese
     * \brief 新建 COperate 实例。仅由 CClient 调用
     * \snippet Client/Client.cpp CClient CreateOperate
     * \return 返回 COperate 指针, 它的所有者是调用者。
     * \note 此函数新建一个对象指针，
     *       <b>调用者必须负责在用完后释放指针</b>。
     * \param szId 连接 ID
     *
     * \~english
     * \brief New COperate instance. Only is called by CClient
     * \snippet Client/Client.cpp CClient CreateOperate
     * \return Return COperate pointer, the owner is caller
     * \note The function new object pointer,
     *       <b>the caller must delete it when don't use it</b>.
     * \param szId
     *
     * \~
     * \see CClient::CreateOperate CClient::LoadOperate
     *
     */
    Q_INVOKABLE [[nodiscard]] virtual COperate* CreateOperate(
        const QString& szId, CParameterPlugin* para);
    [[nodiscard]] virtual COperate* OnCreateOperate(const QString& szId) = 0;
    /*!
     * Delete COperate
     */
    Q_INVOKABLE virtual int DeleteOperate(COperate* p);

private:
    QSharedPointer<QTranslator> m_Translator;
    /*!
     * \~chinese 初始化翻译资源，仅由 CClient 调用。
     *   因为它调用了 Name() ，所以不能在此类的构造函数中直接调用。
     *
     * \~english Initialize the translation resource,
     *   which is only called by CClient.
     *   Because it calls Name(),
     *   So it can't be called directly in the constructor of this class.
     *
     * \see CClient::AppendPlugin
     */
    Q_INVOKABLE int InitTranslator();
};

QT_BEGIN_NAMESPACE
#define CPlugin_iid "KangLinStudio.Rabbit.RemoteControl.CPlugin"
Q_DECLARE_INTERFACE(CPlugin, CPlugin_iid)
QT_END_NAMESPACE
