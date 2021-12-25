// Author: Kang Lin <kl222@126.com>

#ifndef CMANAGEPASSWORD_H
#define CMANAGEPASSWORD_H

#include <QObject>
#include "viewer_export.h"

/*!
 * \brief Manage password
 */
class VIEWER_EXPORT CManagePassword : public QObject
{
    Q_OBJECT
     
public:
    explicit CManagePassword(QObject *parent = nullptr);
    
    static CManagePassword* Instance();

public:
    const QString &GetPassword() const;
    void SetPassword(const QString &newPassword);
Q_SIGNALS:
    void PasswordChanged();
private:
    QString m_szPassword;
    Q_PROPERTY(QString Password READ GetPassword WRITE SetPassword NOTIFY PasswordChanged)

public:
    const bool &GetSavePassword() const;
    void SetSavePassword(bool NewAutoSavePassword);
Q_SIGNALS:
    void sigSavePasswordChanged(bool AutoSavePassword);
private:
    bool m_bSavePassword;
    Q_PROPERTY(bool SavePassword READ GetSavePassword WRITE SetSavePassword NOTIFY sigSavePasswordChanged)
};

#endif // CMANAGEPASSWORD_H
