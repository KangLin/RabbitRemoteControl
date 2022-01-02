#ifndef DLGGETUSERPASSWORD_H
#define DLGGETUSERPASSWORD_H

#include <QDialog>

namespace Ui {
class CDlgGetUserPassword;
}

class CDlgGetUserPassword : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgGetUserPassword(QWidget *parent = nullptr);
    virtual ~CDlgGetUserPassword();
    
    const QString &GetUser() const;
    void SetUser(const QString &NewUser);
    const QString &GetPassword() const;
    void SetPassword(const QString &NewPassword);
    
    bool GetSavePassword() const;
    void SetSavePassword(bool NewSave);
    
    const QString &GetText() const;
    void SetText(const QString &newText);
    
private slots:
    void on_pbOK_clicked();
    
    void on_pbCancel_clicked();
    
private:
    Ui::CDlgGetUserPassword *ui;

    QString m_Text;
    QString m_User;
    QString m_Password;
    bool m_SavePassword;
    
    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *event) override;
};

#endif // DLGGETUSERPASSWORD_H
