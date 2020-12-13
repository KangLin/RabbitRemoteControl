#include "DlgSetFreeRdp.h"
#include "ui_DlgSetFreeRdp.h"
#include <QApplication>
#include <QScreen>

CDlgSetFreeRdp::CDlgSetFreeRdp(rdpSettings *pSettings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSetFreeRdp),
    m_pSettings(pSettings)
{
    ui->setupUi(this);

    ui->cbDesktopSize->addItem("640×480");
    ui->cbDesktopSize->addItem("800×600");
    ui->cbDesktopSize->addItem("1280×720");
        
    ui->cbColorDepth->addItem(tr("8 bits"), 8);
    ui->cbColorDepth->addItem(tr("16 bits"), 16);
    ui->cbColorDepth->addItem(tr("24 bits"), 24);
    ui->cbColorDepth->addItem(tr("32 bits"), 32);
}

CDlgSetFreeRdp::~CDlgSetFreeRdp()
{
    delete ui;
}

void CDlgSetFreeRdp::on_pbOk_clicked()
{
    // Server
    freerdp_settings_set_string(m_pSettings,
                                FreeRDP_Domain,
                                ui->leDomain->text().toStdString().c_str());
    freerdp_settings_set_string(m_pSettings,
                                FreeRDP_ServerHostname,
                                ui->leServer->text().toStdString().c_str());
    m_pSettings->ServerPort = ui->spPort->value();
    freerdp_settings_set_string(m_pSettings,
                                FreeRDP_Username,
                                ui->leName->text().toStdString().c_str());
    freerdp_settings_set_string(m_pSettings,
                                FreeRDP_Password,
                                ui->lePassword->text().toStdString().c_str());
    
    // Display
    QString szSize = ui->cbDesktopSize->currentText();
    int index = szSize.indexOf("×");
    if(-1 < index)
    {
        int width = szSize.left(index).toInt();
        int height = szSize.right(szSize.length() - index - 1).toInt();
        m_pSettings->DesktopWidth = width;
        m_pSettings->DesktopHeight = height;
    }
    m_pSettings->ColorDepth = ui->cbColorDepth->currentData().toInt();
    
    accept();
}

void CDlgSetFreeRdp::on_pbCancel_clicked()
{
    reject();
}

void CDlgSetFreeRdp::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    Q_ASSERT(m_pSettings);

    // Server
    ui->leDomain->setText(freerdp_settings_get_string(m_pSettings, FreeRDP_Domain));
    ui->leServer->setText(freerdp_settings_get_string(m_pSettings, FreeRDP_ServerHostname));
    ui->spPort->setValue(m_pSettings->ServerPort);
    ui->leName->setText(freerdp_settings_get_string(m_pSettings, FreeRDP_Username));
    ui->lePassword->setText(freerdp_settings_get_string(m_pSettings, FreeRDP_Password));
    
    // Display
    QScreen* pScreen = QApplication::primaryScreen();
    int width = pScreen->availableGeometry().width();
    int height = pScreen->availableGeometry().height();
    QString curSize = QString::number(width) + "×" + QString::number(height);
    InsertDesktopSize(width, height);
    if(width == m_pSettings->DesktopWidth
            && height == m_pSettings->DesktopHeight)
    {
        ui->rbFullScreen->setChecked(true);
        ui->cbDesktopSize->setCurrentText(curSize);
    } else {
        ui->rbSelect->setChecked(true);
        InsertDesktopSize(m_pSettings->DesktopWidth, m_pSettings->DesktopHeight);
        curSize = QString::number(m_pSettings->DesktopWidth)
                + "×" + QString::number(m_pSettings->DesktopHeight);
        ui->cbDesktopSize->setCurrentText(curSize);
    }
    
    int nIndex = ui->cbColorDepth->findData(m_pSettings->ColorDepth);
    if(-1 != nIndex)
        ui->cbColorDepth->setCurrentIndex(nIndex);
}

void CDlgSetFreeRdp::on_rbFullScreen_clicked(bool checked)
{
    if(!checked) return;
    QScreen* pScreen = QApplication::primaryScreen();
    int width = pScreen->availableGeometry().width();
    int height = pScreen->availableGeometry().height();
    QString curSize = QString::number(width) + "×" + QString::number(height);
    if(ui->cbDesktopSize->findText(curSize) == -1)
        ui->cbDesktopSize->addItem(curSize);
    ui->cbDesktopSize->setCurrentText(curSize);
}

int CDlgSetFreeRdp::InsertDesktopSize(int width, int height)
{
    QString curSize = QString::number(width) + "×" + QString::number(height);
    if(ui->cbDesktopSize->findText(curSize) > -1)
        return 0;
    
    for(int i = 0; i < ui->cbDesktopSize->count(); i++)
    {
        QString curText = ui->cbDesktopSize->itemText(i);
        int nIndex = curText.indexOf("×");
        if(-1 < nIndex)
        {
            int w = curText.left(nIndex).toInt();
            if(w > width)
            {
                ui->cbDesktopSize->insertItem(i, curSize);
                return 0;
            }
        }
    }
    
    if(ui->cbDesktopSize->findText(curSize) == -1)
        ui->cbDesktopSize->addItem(curSize);
    
    return 0;
}
