// Author: Kang Lin <kl222@126.com>

#ifndef CMANAGEPASSWORD_H
#define CMANAGEPASSWORD_H

#include <QObject>
#include "viewer_export.h"

class VIEWER_EXPORT CManagePassword : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString GetPassword READ GetPassword WRITE SetPassword NOTIFY PasswordChanged)
    
public:
    explicit CManagePassword(QObject *parent = nullptr);
    
    static CManagePassword* Instance();

    const QString &GetPassword() const;
    void SetPassword(const QString &newPassword);

Q_SIGNALS:
    void PasswordChanged();
    
private:
    QString m_szPassword;
};

#endif // CMANAGEPASSWORD_H
