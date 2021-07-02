// Author: Kang Lin <kl222@126.com>

#ifndef CPARAMTER_H
#define CPARAMTER_H

#pragma once

#include "rabbitremotecontrol_export.h"
#include <QObject>
#include <QDataStream>

/**
 * @~english
 * @brief The parameter interface. It contains basic parameters.
 * @note The interface only is implemented by plugin
 * 
 * @~chinese
 * @brief 参数接口。它包含基本参数
 * @note 此接口仅由插件派生实现
 * 
 * @~
 * @ingroup PLUGIN_API
 */
class RABBITREMOTECONTROL_EXPORT CParameter : public QObject
{
    Q_OBJECT

public:
    explicit CParameter(QObject *parent = nullptr);

    QString szName;
    
    QString szHost;
    quint16 nPort;
    
    QString szUser;
    QString szPassword;
    bool bSavePassword;
    
    bool bOnlyView;
    bool bLocalCursor;
    bool bClipboard;
    
    enum class emProxy {
        No,
        SocksV4,
        SocksV5,
        Http,
        User = 100
    };
    emProxy eProxyType;
    QString szProxyHost;
    quint16 nProxyPort;
    QString szProxyUser;
    QString szProxyPassword;
    
Q_SIGNALS:
    void sigUpdate();
};

QDataStream &operator<<(QDataStream &, const CParameter &);
QDataStream &operator>>(QDataStream &, CParameter &);

#endif // CPARAMTER_H
