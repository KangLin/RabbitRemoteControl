//! @author: Kang Lin(kl222@126.com)
//! 
#include "DlgSetFreeRdp.h"
#include "ui_DlgSetFreeRdp.h"
#include <QApplication>
#include <QScreen>

CDlgSetFreeRdp::CDlgSetFreeRdp(CConnecterFreeRdp::CParameterFreeRdp *pSettings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSetFreeRdp),
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

CDlgSetFreeRdp::~CDlgSetFreeRdp()
{
    delete ui;
}

void CDlgSetFreeRdp::on_pbOk_clicked()
{
    // Server
    m_pSettings->szHost = ui->leServer->text();
    m_pSettings->nPort = ui->spPort->value();
    m_pSettings->szUser = ui->leName->text();
    m_pSettings->szPassword = ui->lePassword->text();
    freerdp_settings_set_string(m_pSettings->pSettings,
                                FreeRDP_Domain,
                                ui->leDomain->text().toStdString().c_str());

    m_pSettings->bSavePassword = ui->cbSavePassword->isChecked();
    m_pSettings->bOnlyView = ui->cbOnlyView->isChecked();
    m_pSettings->bClipboard = ui->cbClipboard->isChecked();
    
    // Display
    QString szSize = ui->cbDesktopSize->currentText();
    int index = szSize.indexOf("×");
    if(-1 < index)
    {
        int width = szSize.left(index).toInt();
        int height = szSize.right(szSize.length() - index - 1).toInt();
        m_pSettings->pSettings->DesktopWidth = width;
        m_pSettings->pSettings->DesktopHeight = height;
    }
    if(ui->cbAllMonitor->isChecked())
    {
        //TODO: complete it
        m_pSettings->pSettings->MonitorCount = QApplication::screens().length();
    }
    m_pSettings->pSettings->ColorDepth = ui->cbColorDepth->currentData().toInt();
    
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
    ui->leDomain->setText(freerdp_settings_get_string(m_pSettings->pSettings, FreeRDP_Domain));
    ui->leServer->setText(m_pSettings->szHost);
    ui->spPort->setValue(m_pSettings->nPort);
    ui->leName->setText(m_pSettings->szUser);
    ui->lePassword->setText(m_pSettings->szPassword);
    
    ui->cbSavePassword->setChecked(m_pSettings->bSavePassword);
    ui->cbOnlyView->setChecked(m_pSettings->bOnlyView);
    ui->cbClipboard->setChecked(m_pSettings->bClipboard);
    
    // Display
    QScreen* pScreen = QApplication::primaryScreen();
    int width = pScreen->availableGeometry().width();
    int height = pScreen->availableGeometry().height();
    QString curSize = QString::number(width) + "×" + QString::number(height);
    InsertDesktopSize(width, height);
    if(width == m_pSettings->pSettings->DesktopWidth
            && height == m_pSettings->pSettings->DesktopHeight)
    {
        ui->rbFullScreen->setChecked(true);
        ui->cbDesktopSize->setCurrentText(curSize);
    } else {
        ui->rbSelect->setChecked(true);
        InsertDesktopSize(m_pSettings->pSettings->DesktopWidth,
                          m_pSettings->pSettings->DesktopHeight);
        curSize = QString::number(m_pSettings->pSettings->DesktopWidth)
                + "×" + QString::number(m_pSettings->pSettings->DesktopHeight);
        ui->cbDesktopSize->setCurrentText(curSize);
    }
    if(m_pSettings->pSettings->MonitorCount > 1)
        ui->cbAllMonitor->setChecked(true);
    int nIndex = ui->cbColorDepth->findData(m_pSettings->pSettings->ColorDepth);
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
    //ui->rbFullScreen->setText(tr("Full screen") + ": " + curSize);
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

void CDlgSetFreeRdp::on_pbShow_clicked()
{
    switch(ui->lePassword->echoMode())
    {
    case QLineEdit::Password:
        ui->lePassword->setEchoMode(QLineEdit::Normal);
        ui->pbShow->setIcon(QIcon(":/image/EyeOff"));
        break;
    case QLineEdit::Normal:
        ui->lePassword->setEchoMode(QLineEdit::Password);
        ui->pbShow->setIcon(QIcon(":/image/EyeOn"));
        break;
    }
}
