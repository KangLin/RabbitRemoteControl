// Author: Kang Lin <kl222@126.com>
//! 
#include "DlgSetFreeRDP.h"
#include "ui_DlgSetFreeRDP.h"
#include <QApplication>
#include <QScreen>

CDlgSetFreeRDP::CDlgSetFreeRDP(CParameterFreeRDP *pSettings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSetFreeRDP),
    m_pSettings(pSettings)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    ui->cbDesktopSize->addItem("640×480");
    ui->cbDesktopSize->addItem("800×600");
    ui->cbDesktopSize->addItem("1024×600");
    ui->cbDesktopSize->addItem("1024×768");
    ui->cbDesktopSize->addItem("1280×1024");
    ui->cbDesktopSize->addItem("1280×854");
    ui->cbDesktopSize->addItem("1280×720");
    ui->cbDesktopSize->addItem("1280×960");
    ui->cbDesktopSize->addItem("1366×768");
    ui->cbDesktopSize->addItem("1400×1050");
    ui->cbDesktopSize->addItem("1440×900");
    ui->cbDesktopSize->addItem("1600×900");
    ui->cbDesktopSize->addItem("1600×1024");
    ui->cbDesktopSize->addItem("1600×1200");
    ui->cbDesktopSize->addItem("1680×1050");
    ui->cbDesktopSize->addItem("1920×1080");
    ui->cbDesktopSize->addItem("1920×1200");

    QScreen* pScreen = QApplication::primaryScreen();
    int width = pScreen->availableGeometry().width();
    int height = pScreen->availableGeometry().height();
    QString curSize = QString::number(width) + "×" + QString::number(height);
    ui->rbFullScreen->setText(tr("Full screen") + ": " + curSize);
    if(ui->cbDesktopSize->findText(curSize) == -1)
        ui->cbDesktopSize->addItem(curSize);
    
    ui->cbColorDepth->addItem(tr("8 bits"), 8);
    ui->cbColorDepth->addItem(tr("16 bits"), 16);
    ui->cbColorDepth->addItem(tr("24 bits"), 24);
    ui->cbColorDepth->addItem(tr("32 bits"), 32);
}

CDlgSetFreeRDP::~CDlgSetFreeRDP()
{
    delete ui;
}

void CDlgSetFreeRDP::on_pbOk_clicked()
{
    // Server
    m_pSettings->SetName(ui->leName->text());
    m_pSettings->SetHost(ui->leServer->text());
    m_pSettings->SetPort(ui->spPort->value());
    m_pSettings->SetUser(ui->leUserName->text());
    m_pSettings->SetPassword(ui->lePassword->text());
    freerdp_settings_set_string(m_pSettings->m_pSettings,
                                FreeRDP_Domain,
                                ui->leDomain->text().toStdString().c_str());

    m_pSettings->SetSavePassword(ui->cbSavePassword->isChecked());
    m_pSettings->SetOnlyView(ui->cbOnlyView->isChecked());
    m_pSettings->SetClipboard(ui->cbClipboard->isChecked());
    m_pSettings->SetShowServerName(ui->cbShowServerName->isChecked());
    
    // Display
    QString szSize = ui->cbDesktopSize->currentText();
    int index = szSize.indexOf("×");
    if(-1 < index)
    {
        int width = szSize.left(index).toInt();
        int height = szSize.right(szSize.length() - index - 1).toInt();
        freerdp_settings_set_uint32(m_pSettings->m_pSettings,
                                    FreeRDP_DesktopWidth, width);
        freerdp_settings_set_uint32(m_pSettings->m_pSettings,
                                    FreeRDP_DesktopHeight, height);
    }
    if(ui->cbAllMonitor->isChecked())
    {
        //TODO: complete it
        m_pSettings->m_pSettings->MonitorCount = QApplication::screens().length();
    }
    freerdp_settings_set_uint32(m_pSettings->m_pSettings, FreeRDP_ColorDepth,
                               ui->cbColorDepth->currentData().toInt());
    m_pSettings->SetReconnectInterval(ui->sbReconnect->value());
    accept();
}

void CDlgSetFreeRDP::on_pbCancel_clicked()
{
    reject();
}

void CDlgSetFreeRDP::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    Q_ASSERT(m_pSettings);

    // Server
    ui->leName->setText(m_pSettings->GetName());
    ui->leDomain->setText(freerdp_settings_get_string(m_pSettings->m_pSettings,
                                                      FreeRDP_Domain));
    ui->leServer->setText(m_pSettings->GetHost());
    ui->spPort->setValue(m_pSettings->GetPort());
    ui->leUserName->setText(m_pSettings->GetUser());
    ui->lePassword->setText(m_pSettings->GetPassword());
    ui->pbShow->setEnabled(m_pSettings->GetParameterViewer()->GetViewPassowrd());

    ui->cbSavePassword->setChecked(m_pSettings->GetSavePassword());
    ui->cbOnlyView->setChecked(m_pSettings->GetOnlyView());
    ui->cbClipboard->setChecked(m_pSettings->GetClipboard());
    ui->cbShowServerName->setChecked(m_pSettings->GetShowServerName());

    // Display
    QScreen* pScreen = QApplication::primaryScreen();
    int width = pScreen->availableGeometry().width();
    int height = pScreen->availableGeometry().height();
    QString curSize = QString::number(width) + "×" + QString::number(height);
    InsertDesktopSize(width, height);
    UINT32 desktopWidth = 0, desktopHeight = 0;
    desktopWidth = freerdp_settings_get_uint32(m_pSettings->m_pSettings,
                                               FreeRDP_DesktopWidth);
    desktopHeight = freerdp_settings_get_uint32(m_pSettings->m_pSettings,
                                                FreeRDP_DesktopHeight);
    if(width == desktopWidth && height == desktopHeight)
    {
        ui->rbFullScreen->setChecked(true);
        ui->cbDesktopSize->setCurrentText(curSize);
    } else {
        ui->rbSelect->setChecked(true);
        InsertDesktopSize(desktopWidth, desktopHeight);
        curSize = QString::number(desktopWidth) 
                + "×" + QString::number(desktopHeight);
        ui->cbDesktopSize->setCurrentText(curSize);
    }
    if(m_pSettings->m_pSettings->MonitorCount > 1)
        ui->cbAllMonitor->setChecked(true);
    int nIndex = ui->cbColorDepth->findData(
                freerdp_settings_get_uint32(m_pSettings->m_pSettings,
                                            FreeRDP_ColorDepth));
    if(-1 != nIndex)
        ui->cbColorDepth->setCurrentIndex(nIndex);
    
    ui->sbReconnect->setValue(m_pSettings->GetReconnectInterval());
}

void CDlgSetFreeRDP::on_rbFullScreen_clicked(bool checked)
{
    if(!checked) return;
    QScreen* pScreen = QApplication::primaryScreen();
    int width = pScreen->availableGeometry().width();
    int height = pScreen->availableGeometry().height();
    QString curSize = QString::number(width) + "×" + QString::number(height);
    //ui->rbFullScreen->setText(tr("Full screen") + ": " + curSize);
    if(ui->cbDesktopSize->findText(curSize) == -1)
        ui->cbDesktopSize->addItem(curSize);
    ui->cbDesktopSize->setCurrentText(curSize);
}

int CDlgSetFreeRDP::InsertDesktopSize(int width, int height)
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

void CDlgSetFreeRDP::on_pbShow_clicked()
{
    switch(ui->lePassword->echoMode())
    {
    case QLineEdit::Password:
        ui->lePassword->setEchoMode(QLineEdit::Normal);
        ui->pbShow->setIcon(QIcon::fromTheme("eye-off"));
        break;
    case QLineEdit::Normal:
        ui->lePassword->setEchoMode(QLineEdit::Password);
        ui->pbShow->setIcon(QIcon::fromTheme("eye-on"));
        break;
    }
}
