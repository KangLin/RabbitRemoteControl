//! @author: Kang Lin(kl222@126.com)

#ifndef CPARAMTER_H
#define CPARAMTER_H

#include "rabbitremotecontrol_export.h"
#include <QObject>

/**
 * @brief The CParamter class
 * @note The interface only is implemented by plugin
 */
class RABBITREMOTECONTROL_EXPORT CParamter : public QObject
{
    Q_OBJECT

public:
    explicit CParamter(QObject *parent = nullptr);

    QString szHost;
    qint16 nPort;
    
    QString szUser;
    QString szPassword;
    bool bSavePassword;
    
    bool bOnlyView;
    bool bLocalCursor;
    bool bClipboard;
};

#endif // CPARAMTER_H
