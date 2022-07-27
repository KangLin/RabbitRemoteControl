// Author: Kang Lin <kl222@126.com>

#ifndef CPARAMETER_H_KL_2022_07_27
#define CPARAMETER_H_KL_2022_07_27

#pragma once

#include <QSettings>
#include <QString>
#include "viewer_export.h"

/*!
 *
 * \~chinese
 * \brief 参数接口类
 * \details
 * 参数有以下类型：
 * 1. 仅在插件内有效。应用程序不能直接访问，
 *    应用程序只能通过 CConnecter::OpenDialogSettings 进行设置。
 *    \see CParameterConnect
 * 2. 在控制端库内有效。应用程序不能直接访问。
 *    应用程序只通过 CManagePlugin::GetSettings 进行设置。
 *    \see CParameterViwer
 * 3. 同时在应用程序和控制端库有效。
 *
 * \~english
 * \brief The CParameter interface class
 *
 * \details
 * The parameter has the following type:
 * 1. Only valid in the plugin.
 *    The application cannot access it directly,
 *    it can only be set via CConnecter::OpenDialogSettings.
 *    \see CParameterConnect
 * 2. Valid in the viewer.
 *    The application cannot access it directly,
 *    it can only be set via CManagePlugin::GetSettings
 *    \see CParameterViwer
 * 3. Valid in both the application and the viewer.
 *
 * \~
 * \ingroup VIEWER_PLUGIN_API
 */
class VIEWER_EXPORT CParameter : public QObject
{
    Q_OBJECT

public:
    explicit CParameter(QObject *parent = nullptr);
    virtual ~CParameter();

    virtual int Load(QString szFile = QString());
    virtual int Save(QString szFile = QString());
    virtual int Load(QSettings &set) = 0;
    virtual int Save(QSettings &set) = 0;

Q_SIGNALS:
    void sigChanged();
    // Notify save to file
    void sigSave();
};

#endif // CPARAMETER_H_KL_2022_07_27
