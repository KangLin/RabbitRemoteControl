#ifndef DLGSETTINGS_H
#define DLGSETTINGS_H

#include <QDialog>

struct strPara{
    QString szServerName;
    QString szUser;
    QString szPassword;
    
    int nEncoding;
    int nCompressLevel;
    int nQualityLevel;
    int nColorLevel;
};

namespace Ui {
class CDlgSettings;
}

class CDlgSettings : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSettings(strPara *pPara, QWidget *parent = nullptr);
    virtual ~CDlgSettings();
    
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    
private:
    Ui::CDlgSettings *ui;
    strPara* m_pPara;
};

#endif // DLGSETTINGS_H
