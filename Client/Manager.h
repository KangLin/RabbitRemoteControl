// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QObject>
#include <QDir>
#include <QMap>
#include <QIcon>
#include <QTranslator>
#include <QSharedPointer>

#include "Plugin.h"
#include "Hook.h"
#include "ParameterPlugin.h"

#if defined(__clang__) || defined(__GNUC__)
#define CPP_STANDARD __cplusplus
#elif defined(_MSC_VER)
#define CPP_STANDARD _MSVC_LANG
#endif
#if CPP_STANDARD >= 199711L
#define HAS_CPP_03 1
#endif
#if CPP_STANDARD >= 201103L
#define HAS_CPP_11 1
#endif
#if CPP_STANDARD >= 201402L
#define HAS_CPP_14 1
#endif
#if CPP_STANDARD >= 201703L
#define HAS_CPP_17 1
#endif

#if HAS_CPP_11
#include <functional>
#endif

/*!
 * \~chinese 管理插件
 * \~english Manage plugins
 * \~
 * \ingroup LIBAPI
 */
class CManager : public QObject
{
    Q_OBJECT
public:
    explicit CManager(QObject *parent = nullptr, QString szFile = QString());
    virtual ~CManager();

    /*!
     * \~chinese
     * \brief 新建 COperate 指针，所有者是调用者。
     *    <b>当不在使用时，调用者必调用　DeteleOperate()　须释放指针</b>。
     *       调用者必须连接信号 COperate::sigFinished 。
     *       释放指针 (使用 DeleteOperate() )
     * \param id 插件 ID
     * \~english New COperate pointer, the owner is caller.
     *    <b>The caller must be call DeteleOperate() to delete it, when it isn't need</b>.
     *    The caller must connect COperate::sigFinished,
     *    then delete it( Use DeleteOperate() )
     * \param id Plugin ID
     * \~
     * \callgraph
     * \see DeleteOperate
     */
    virtual COperate* CreateOperate(const QString &id);
    /*!
     * Delete COperate
     */
    virtual int DeleteOperate(COperate* p);
    
    /*! \~chinese 从文件中新建 COperate 指针，所有者是调用者。
     *     <b>当不再使用时，调用者必须负责调用　DeleteOperate() 删除此指针</b>。
     *        调用者必须连接信号 COperate::sigFinished 。
     *        释放指针(使用 DeleteOperate() )
     * \~english New COperate pointer from file, the owner is caller.
     *     <b>The caller must delete it(call DeleteOperate()),
     *        when it isn't need</b>.
     *        The caller must connect COperate::sigFinished,
     *        then delete it (Use DeleteOperate() )
     * \~
     * \callgraph
     * \see DeleteOperate
     */
    virtual COperate* LoadOperate(const QString& szFile);
    
    /*! \~chinese 保存连接参数到文件
     * \~english Accept Operate parameters to file
     * \~
     * \callgraph
     */
    virtual int SaveOperate(COperate* pOperate);
    
    /*!
     * \~chinese 得到设置参数窗口
     * \return 设置参数窗口。此窗口的所有者为调用者。
     * \note 此窗口必须从 CParameterUI 派生。
     *       例如: CFrmParameterClient
     * 用法示例：
     * \snippet App/Client/mainwindow.cpp Get the widget that settings client parameters
     * 
     * 在 CParameterDlgSettings 的构造函数中连接信号和槽
     * \snippet App/Client/ParameterDlgSettings.cpp connect accepted to slotAccept of widget
     * 
     * \~english Get parameter settings widget
     * \return Parameter widget. The QWidget owner is caller.
     * \note The widget must be a derivative of CParameterUI.
     *         Eg: CFrmParameterClient
     *
     * \snippet Client/FrmParameterClient.h Accept parameters
     * Usage:
     * \snippet App/Client/mainwindow.cpp Get the widget that settings client parameters
     * Connect the signal(accepted) to the slot(slotAccept) in CParameterDlgSettings
     * \snippet App/Client/ParameterDlgSettings.cpp connect accepted to slotAccept of widget
     * 
     * \~
     * \see CFrmParameterClient CParameterDlgSettings
     */   
    virtual QList<QWidget*> GetSettingsWidgets(QWidget *parent);
    /*!
     * \~chinese
     * \brief 从文件中加载客户端参数
     * \param szFile: 文件名
     * \~english Load Client parameters from file.
     * \param szFile: file name
     */
    virtual int LoadSettings(const QString szFile = QString());
    /*!
     * \~chinese
     * \brief 保存客户端参数到文件
     * \param szFile：文件名
     * \~english Save Client parameters to file
     * \param szFile: file name
     */
    virtual int SaveSettings(const QString szFile = QString());
    
    /**
     * \~chinese
     * \brief 处理连接者。用于客户端得到连接者信号
     * 
     * \~english
     * \brief The Handle COperate class
     */
    class Handle{
    public:
        Handle(){}
        /**
         * \brief Process plugins
         * \param id: plugin id
         * \param pPlugin: CPlugin pointer
         */
        virtual int onProcess(const QString& id, CPlugin* pPlugin) = 0;
    };
    /*!
     * \brief Enum plugins
     */
    virtual int EnumPlugins(Handle* handle);
#if HAS_CPP_11
    /*!
     * \brief Enum plugins
     * \note It is need c++ standard 11
     */
    virtual int EnumPlugins(
        /**
             * \brief Process plugins callback function
             * \param id: plugin id
             * \param pPlugin: CPlugin pointer
             */
        std::function<int(const QString& id, CPlugin* pPlugin)> cb);
#endif
    
    const QString Details() const;
    
private:
    int LoadPlugins();
    int FindPlugins(QDir dir, QStringList filters);
    int AppendPlugin(CPlugin* plugin);
    
private:
    QString m_szSettingsFile;
    QMap<QString, CPlugin*> m_Plugins;
    qint8 m_FileVersion;
    QSharedPointer<QTranslator> m_Translator;
    CHook* m_pHook;
    CParameterPlugin* m_pParameter;
    QString m_szDetails;
};
