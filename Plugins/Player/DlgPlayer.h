#ifndef DLGPLAYER_H
#define DLGPLAYER_H

#include <QDialog>
#include "ParameterPlayer.h"

namespace Ui {
class CDlgPlayer;
}

class CDlgPlayer : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgPlayer(CParameterPlayer* pPara, QWidget *parent = nullptr);
    ~CDlgPlayer();

private:
    Ui::CDlgPlayer *ui;
    CParameterPlayer* m_pParameters;

    // QDialog interface
public slots:
    virtual void accept() override;
private slots:
    void on_cmbType_currentIndexChanged(int index);
    void on_pbUrlBrowse_clicked();
};

#endif // DLGPLAYER_H
