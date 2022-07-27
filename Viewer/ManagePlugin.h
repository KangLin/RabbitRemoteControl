// Author: Kang Lin <kl222@126.com>

#ifndef CMANAGECONNECTER_H
#define CMANAGECONNECTER_H

#pragma once

#include <QObject>
#include <QDir>
#include <QMap>
#include <QIcon>
#include <QTranslator>
#include <QSharedPointer>
#include "PluginViewer.h"
#include "Hook.h"

/**
 * \~chinese \brief 管理插件和连接者
 * \~english \brief manage plugins and connecter
 * \~
 * \see   CPluginViewer CConnecter
 * \ingroup VIEWER_API
 */
class VIEWER_EXPORT CManagePlugin : public QObject
{
    Q_OBJECT
    
public:
    explicit CManagePlugin(QObject *parent = nullptr);
    virtual ~CManagePlugin();
    
    /*!
     * \~chinese
     * \brief 新建 CConnecter 指针，所有者是调用者。
     *        <b>当不在使用时，调用者必须释放指针</b>。
     *        调用者必须连接信号 CConnecter::sigDisconnected 。
     *        释放指针 ( CConnecter::deleteLater )
     * \param id 插件 ID
     * \~english New CConnecter pointer, the owner is caller.
     *           <b>The caller must delete it, when isn't need it</b>.
     *           The caller must connect CConnecter::sigDisconnected,
     *           then delete it( CConnecter::deleteLater )
     * \param id Plugin ID
     */
    virtual CConnecter* CreateConnecter(const QString &id);
    
    /*! \~chinese 从文件中新建 CConnecter 指针，所有者是调用者。
     *          <b>当不再使用时，调用者必须负责删除此指针</b>
     *          调用者必须连接信号 CConnecter::sigDisconnected 。
     *          释放指针( CConnecter::deleteLater )
     * \~english New CConnecter pointer from file, the owner is caller.
     *          <b>The caller must delete it, when isn't need it</b>.
     *          The caller must connect CConnecter::sigDisconnected,
     *          then delete it ( CConnecter::deleteLater )
     */
    virtual CConnecter* LoadConnecter(const QString& szFile);
    
    /// \~chinese 保存连接参数到文件
    /// \~english Accept connecter parameters to file
    virtual int SaveConnecter(QString szFile, CConnecter* pConnecter);

    /**
     * \~chinese
     * \brief 处理连接者。用于客户端得到连接者信号
     * 
     * \~english
     * \brief The Handle CConnecter class
     */
    class Handle{
    public:
        Handle(): m_bIgnoreReturn(false){}
        /**
         * @brief Process plugins
         * @param id: plugin id
         * @param pPlug: CPluginViewer pointer
         * @return 
         */
        virtual int onProcess(const QString& id, CPluginViewer* pPlug) = 0;
        int m_bIgnoreReturn;
    };
    virtual int EnumPlugins(Handle* handle);

private:    
    int LoadPlugins();
    int FindPlugins(QDir dir, QStringList filters);
    
private:
    QMap<QString, CPluginViewer*> m_Plugins;
    qint8 m_FileVersion;
    QTranslator m_Translator;
    QSharedPointer<CHook> m_Hook;
};

#endif // CMANAGECONNECTER_H
