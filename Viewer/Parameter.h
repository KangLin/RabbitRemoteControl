//! @author: Kang Lin (kl222@126.com)

#ifndef CPARAMTER_H
#define CPARAMTER_H

#include "rabbitremotecontrol_export.h"
#include <QObject>
#include <QDataStream>

/**
 * @brief The CParameter class
 * @note The interface only is implemented by plugin
 * @addtogroup PLUGIN_API
 */
class RABBITREMOTECONTROL_EXPORT CParameter : public QObject
{
    Q_OBJECT

public:
    explicit CParameter(QObject *parent = nullptr);

    QString szHost;
    qint16 nPort;
    
    QString szUser;
    QString szPassword;
    bool bSavePassword;
    
    bool bOnlyView;
    bool bLocalCursor;
    bool bClipboard;
};

QDataStream &operator<<(QDataStream &, const CParameter &);
QDataStream &operator>>(QDataStream &, CParameter &);

#endif // CPARAMTER_H
