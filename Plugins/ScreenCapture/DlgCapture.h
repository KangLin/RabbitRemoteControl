#ifndef DLGCAPTURE_H
#define DLGCAPTURE_H

#include <QDialog>
#include <QCapturableWindow>
#include "ParameterScreenCapture.h"
#include "ParameterRecordUI.h"

namespace Ui {
class CDlgCapture;
}

class CDlgCapture : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgCapture(CParameterScreenCapture* pPara, QWidget *parent = nullptr);
    ~CDlgCapture();

private:
    Ui::CDlgCapture *ui;
    CParameterScreenCapture* m_pParameters;
    QList<QCapturableWindow> m_Windows;
    CParameterRecordUI* m_pRecordUI;

    // QDialog interface
public slots:
    virtual void accept() override;

};

#endif // DLGCAPTURE_H
