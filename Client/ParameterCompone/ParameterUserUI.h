#ifndef PARAMETERUSERUI_H
#define PARAMETERUSERUI_H

#include <QWidget>
#include "ParameterUser.h"

namespace Ui {
class CParameterUserUI;
}

class CLIENT_EXPORT CParameterUserUI : public QWidget
{
    Q_OBJECT

public:
    explicit CParameterUserUI(QWidget *parent = nullptr);
    ~CParameterUserUI();

    int SetParameter(CParameterUser* pParameter);
    int slotAccept();
    
private slots:
    void on_pbShow_clicked();
    void on_pbSave_clicked();
    
    void on_pbBrowsePublicFile_clicked();
    void on_pbBrowsePrivateFile_clicked();
    void on_pbShowPassphrase_clicked();
    void on_pbSavePassphrase_clicked();
    
    void on_cbType_currentIndexChanged(int index);
    
private:
    Ui::CParameterUserUI *ui;
    CParameterUser* m_pUser;
};

#endif // PARAMETERUSERUI_H
