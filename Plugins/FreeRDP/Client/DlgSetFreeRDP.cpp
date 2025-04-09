// Author: Kang Lin <kl222@126.com>

#if HAVE_OPENSSL
#include <openssl/tls1.h>
#endif

#include "DlgSetFreeRDP.h"
#include "ui_DlgSetFreeRDP.h"
#include "ParameterNetUI.h"

#include <QApplication>
#include <QScreen>
#include <QFileSystemModel>
#include <QButtonGroup>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    #include <QMediaDevices>
    #include <QAudioDevice>
#else
    #include <QAudioDeviceInfo>
#endif

#include "DlgDesktopSize.h"
#include <QLoggingCategory>
static Q_LOGGING_CATEGORY(log, "FreeRDP.Parameter.Dlg")

CDlgSetFreeRDP::CDlgSetFreeRDP(CParameterFreeRDP *pSettings, QWidget *parent) :
    QDialog(parent)
    , ui(new Ui::CDlgSetFreeRDP)
    , m_pSettings(pSettings)
    , m_pFileModel(nullptr)
    , m_pProxyUI(nullptr)
    , m_pRecordUI(nullptr)
    , m_pButtonGroup(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    
    ui->leName->setText(m_pSettings->GetName());

    // Server
    ui->leDomain->setText(m_pSettings->GetDomain());
    ui->wNet->SetParameter(&m_pSettings->m_Net);
    
    ui->cbOnlyView->setChecked(m_pSettings->GetOnlyView());
    ui->cbClipboard->setChecked(m_pSettings->GetClipboard());
    ui->cbShowServerName->setChecked(m_pSettings->GetShowServerName());

    m_pProxyUI = new CParameterProxyUI(ui->tabWidget);
    m_pProxyUI->SetParameter(&m_pSettings->m_Proxy);
    ui->tabWidget->insertTab(1, m_pProxyUI, m_pProxyUI->windowIcon(), tr("Proxy"));
    
    m_pRecordUI = new CParameterRecordUI(ui->tabWidget);
    m_pRecordUI->SetParameter(&m_pSettings->m_Record);
    ui->tabWidget->addTab(m_pRecordUI, m_pRecordUI->windowIcon(), tr("Record"));

    // Display
    // It has to be the first. GetScreenGeometry depends on it
    ui->cbAllMonitor->setChecked(m_pSettings->GetUseMultimon());
    UpdateDesktopSize();
    int width = GetScreenGeometry().width();
    int height = GetScreenGeometry().height();
    QString curSize = QString::number(width) + "×" + QString::number(height);
    UINT32 desktopWidth = 0, desktopHeight = 0;
    desktopWidth = m_pSettings->GetDesktopWidth();
    desktopHeight = m_pSettings->GetDesktopHeight();
    if(width == desktopWidth && height == desktopHeight)
    {
        ui->rbLocalScreen->setChecked(true);
        ui->cbDesktopSize->setCurrentText(curSize);
    } else {
        ui->rbSelect->setChecked(true);
        curSize = QString::number(desktopWidth) 
                  + "×" + QString::number(desktopHeight);
        ui->cbDesktopSize->setCurrentText(curSize);
    }

    ui->cbColorDepth->addItem(tr("8 bits"), 8);
    ui->cbColorDepth->addItem(tr("16 bits"), 16);
    ui->cbColorDepth->addItem(tr("24 bits"), 24);
    ui->cbColorDepth->addItem(tr("32 bits"), 32);
    int nIndex = ui->cbColorDepth->findData(
        m_pSettings->GetColorDepth());
    if(-1 != nIndex)
        ui->cbColorDepth->setCurrentIndex(nIndex);

    ui->sbReconnect->setValue(m_pSettings->GetReconnectInterval());
    
    // Redirection printer
    ui->cbPrinter->setChecked(m_pSettings->GetRedirectionPrinter());
    
    // Redirection audio output
    ui->gbAudio->setEnabled(HasAudioOutput() || HasAudioInput());
    if(m_pSettings->GetRedirectionSound() == CParameterFreeRDP::RedirecionSoundType::Disable)
        ui->rbAudioDisable->setChecked(true);
    if(m_pSettings->GetRedirectionSound() == CParameterFreeRDP::RedirecionSoundType::Local)
        ui->rbAudioLocal->setChecked(true);
    if(m_pSettings->GetRedirectionSound() == CParameterFreeRDP::RedirecionSoundType::Remote)
        ui->rbAudioRemote->setChecked(true);
    QString szRdpSndParameters
                     = tr("- [sys:<sys>,][dev:<dev>,][format:<format>,][rate:<rate>,][channel:<channel>]\n"
                     #if defined (Q_OS_WINDOWS) || defined(Q_OS_WIN) || defined(Q_OS_WIN32) || defined(Q_OS_WINRT)
                          "- sys:winmm"
                     #elif defined(Q_OS_IOS)
                          "- sys:ios\n"
                          "- sys:mac"
                     #elif defined (Q_OS_ANDROID)
                          "- sys:opensles" 
                     #elif defined (Q_OS_LINUX) || defined (Q_OS_UNIX)
                          "- sys:alsa\n" 
                          "- sys:oss\n"
                          "- sys:oss,dev:1,format:1\n"
                          "- sys:sndio"
                     #endif
                         );
    QString szRdpSnd = tr("Options for redirection of audio output:\n") + szRdpSndParameters;
    ui->leRdpSnd->setToolTip(szRdpSnd);
    ui->leRdpSnd->setStatusTip(szRdpSnd);
    ui->leRdpSnd->setWhatsThis(szRdpSnd);
    ui->leRdpSnd->setEnabled(HasAudioOutput());
    ui->leRdpSnd->setText(m_pSettings->GetRedirectionSoundParameters());
    // Redirection audio input
    QString szAudin = tr("Options for redirection of audio input:\n") + szRdpSndParameters;
    ui->leAudin->setToolTip(szAudin);
    ui->leAudin->setStatusTip(szAudin);
    ui->leAudin->setWhatsThis(szAudin);
    ui->leAudin->setText(m_pSettings->GetRedirectionMicrophoneParameters());
    if(HasAudioInput()) {
        ui->cbAudin->setChecked(m_pSettings->GetRedirectionMicrophone());
    } else {
        ui->cbAudin->setChecked(false);
        ui->cbAudin->setEnabled(false);
        ui->leAudin->setEnabled(false);
    }
    
    // Drive
    m_pFileModel= new QFileSystemModel();
    m_pFileModel->setNameFilterDisables(false);
    m_pFileModel->setFilter(QDir::Drives | QDir::Dirs | QDir::NoDotAndDotDot);
    m_pFileModel->setRootPath("");
    ui->tvDrive->setModel(m_pFileModel);
    ui->tvDrive->setEditTriggers(QAbstractItemView::NoEditTriggers);
    int count = m_pFileModel->columnCount();
    for(int i = 1; i < count; i++)
        ui->tvDrive->hideColumn(i);
    bool check = connect(ui->tvDrive->selectionModel(),
                         SIGNAL(selectionChanged(const QItemSelection &,
                                                 const QItemSelection &)),
                         this,
                         SLOT(slotSelectionChanged(QItemSelection,QItemSelection)));
    Q_ASSERT(check);
    QStringList lstDrives = m_pSettings->GetRedirectionDrives();
    foreach(auto path, lstDrives)
    {
        QModelIndex index;
        if(!path.isEmpty()) {
            index = m_pFileModel->index(path);
            ui->tvDrive->setCurrentIndex(index);
        }
    }
    ShowDriveSelected(lstDrives.size());
    
    // Security
    ui->cbSecurityEnable->setChecked(m_pSettings->GetNegotiateSecurityLayer());
    on_cbSecurityEnable_checkStateChanged(ui->cbSecurityEnable->checkState());
    CParameterFreeRDP::Security security = m_pSettings->GetSecurity();
    if(CParameterFreeRDP::Security::RDP & (uint)security)
        ui->cbSecurityRDP->setChecked(true);
    if(CParameterFreeRDP::Security::TLS & (uint)security)
        ui->cbSecurityTls->setChecked(true);
    if(CParameterFreeRDP::Security::NLA & (uint)security)
        ui->cbSecurityNLA->setChecked(true);
    if(CParameterFreeRDP::Security::NLA_Ext & (uint)security)
        ui->cbSecurityNlaExt->setChecked(true);
    if(CParameterFreeRDP::Security::RDSAAD & (uint)security)
        ui->cbSecurityRDSAAD->setChecked(true);
    if(CParameterFreeRDP::Security::RDSTLS & (uint)security)
        ui->cbSecurityRDSTLS->setChecked(true);
#if FreeRDP_VERSION_MAJOR >= 3
    ui->cbSecurityRDSAAD->setVisible(true);
    ui->cbSecurityRDSTLS->setVisible(true);
#else
    ui->cbSecurityRDSAAD->setVisible(false);
    ui->cbSecurityRDSTLS->setVisible(false);
#endif
    // Tls version
    switch(m_pSettings->GetTlsVersion())
    {
    case TLS1_VERSION:
        ui->rbTls1_0->setChecked(true);
        break;
    case TLS1_1_VERSION:
        ui->rbTls1_1->setChecked(true);
        break;
    case TLS1_2_VERSION:
        ui->rbTls1_2->setChecked(true);
        break;
    case TLS1_3_VERSION:
        ui->rbTls1_3->setChecked(true);
        break;
    }

    // Initial defalut performance flags
    m_vPerformanceFlags[CONNECTION_TYPE_MODEM - 1] = PERF_DISABLE_WALLPAPER | PERF_DISABLE_FULLWINDOWDRAG | PERF_DISABLE_MENUANIMATIONS | PERF_DISABLE_THEMING;
    m_vPerformanceFlags[CONNECTION_TYPE_BROADBAND_LOW - 1] = PERF_DISABLE_WALLPAPER | PERF_DISABLE_FULLWINDOWDRAG | PERF_DISABLE_MENUANIMATIONS;
    m_vPerformanceFlags[CONNECTION_TYPE_SATELLITE - 1] = PERF_DISABLE_WALLPAPER | PERF_ENABLE_DESKTOP_COMPOSITION | PERF_DISABLE_FULLWINDOWDRAG | PERF_DISABLE_MENUANIMATIONS;
    m_vPerformanceFlags[CONNECTION_TYPE_BROADBAND_HIGH - 1] = PERF_DISABLE_WALLPAPER | PERF_ENABLE_DESKTOP_COMPOSITION | PERF_DISABLE_FULLWINDOWDRAG | PERF_DISABLE_MENUANIMATIONS;
    m_vPerformanceFlags[CONNECTION_TYPE_WAN - 1] = PERF_ENABLE_FONT_SMOOTHING | PERF_ENABLE_DESKTOP_COMPOSITION;
    m_vPerformanceFlags[CONNECTION_TYPE_LAN - 1] = PERF_ENABLE_FONT_SMOOTHING | PERF_ENABLE_DESKTOP_COMPOSITION;
    m_vPerformanceFlags[CONNECTION_TYPE_AUTODETECT - 1] = PERF_ENABLE_FONT_SMOOTHING | PERF_ENABLE_DESKTOP_COMPOSITION;
    // Load performance flags
    if(m_pSettings->GetConnectType() > 0 && m_pSettings->GetPerformanceFlags() > 0)
        m_vPerformanceFlags[m_pSettings->GetConnectType() - 1] = m_pSettings->GetPerformanceFlags();
    // Connect type
    ui->cbConnectType->addItem(tr("Modem(56 kpbs)"), CONNECTION_TYPE_MODEM);
    ui->cbConnectType->addItem(tr("Broadband low(256 kbps - 2 Mbps)"), CONNECTION_TYPE_BROADBAND_LOW);
    ui->cbConnectType->addItem(tr("Satellite(2 Mbps - 16 Mbps, High latency)"), CONNECTION_TYPE_SATELLITE);
    ui->cbConnectType->addItem(tr("Broadband high(2 Mbps - 10 Mbps)"), CONNECTION_TYPE_BROADBAND_HIGH);
    ui->cbConnectType->addItem(tr("Wan(10 Mbps or higher speed, High latency)"), CONNECTION_TYPE_WAN);
    ui->cbConnectType->addItem(tr("Lan(10 Mbps or higher speed)"), CONNECTION_TYPE_LAN);
    ui->cbConnectType->addItem(tr("Automatically detect"), CONNECTION_TYPE_AUTODETECT);
    ui->cbConnectType->setCurrentIndex(ui->cbConnectType->findData(m_pSettings->GetConnectType()));

}

CDlgSetFreeRDP::~CDlgSetFreeRDP()
{
    delete ui;
}

void CDlgSetFreeRDP::on_pbOk_clicked()
{
    int nRet = 0;

    if(!ui->wNet->CheckValidity(true)) {
        ui->tabWidget->setCurrentIndex(0);
        return;
    }
    if(!m_pProxyUI->CheckValidity(true)) {
        ui->tabWidget->setCurrentWidget(m_pProxyUI);
        return;
    }
    if(!m_pRecordUI->CheckValidity(true)) {
        ui->tabWidget->setCurrentWidget(m_pRecordUI);
        return;
    }

    m_pSettings->SetName(ui->leName->text());

    // Server
    m_pSettings->SetDomain(ui->leDomain->text());
    nRet = ui->wNet->Accept();
    if(nRet) return;
    
    nRet = m_pProxyUI->Accept();
    if(nRet) return;

    nRet = m_pRecordUI->Accept();
    if(nRet) return;

    m_pSettings->SetOnlyView(ui->cbOnlyView->isChecked());
    m_pSettings->SetClipboard(ui->cbClipboard->isChecked());
    m_pSettings->SetShowServerName(ui->cbShowServerName->isChecked());
    
    // Display
    m_pSettings->SetUseMultimon(ui->cbAllMonitor->isChecked());
    QString szSize = ui->cbDesktopSize->currentText();
    int index = szSize.indexOf("×");
    if(-1 < index)
    {
        UINT32 width = szSize.left(index).toInt();
        UINT32 height = szSize.right(szSize.length() - index - 1).toInt();
        m_pSettings->SetDesktopWidth(width);
        m_pSettings->SetDesktopHeight(height);
    }
    m_pSettings->SetColorDepth(ui->cbColorDepth->currentData().toInt());
    m_pSettings->SetReconnectInterval(ui->sbReconnect->value());

    // Redirection
    m_pSettings->SetRedirectionPrinter(ui->cbPrinter->isChecked());
    if(HasAudioOutput()) {
        CParameterFreeRDP::RedirecionSoundType tRdirectionSound
            = CParameterFreeRDP::RedirecionSoundType::Disable;
        if(ui->rbAudioDisable->isChecked())
            tRdirectionSound = CParameterFreeRDP::RedirecionSoundType::Disable;
        else if(ui->rbAudioLocal->isChecked())
            tRdirectionSound = CParameterFreeRDP::RedirecionSoundType::Local;
        else if(ui->rbAudioRemote->isChecked())
            tRdirectionSound = CParameterFreeRDP::RedirecionSoundType::Remote;
        m_pSettings->SetRedirectionSound(tRdirectionSound);
    } else {
        m_pSettings->SetRedirectionSound(
            CParameterFreeRDP::RedirecionSoundType::Disable);
    }
    m_pSettings->SetRedirectionSoundParameters(ui->leRdpSnd->text());
    if(HasAudioInput())
        m_pSettings->SetRedirectionMicrophone(ui->cbAudin->isChecked());
    else
        m_pSettings->SetRedirectionMicrophone(false);
    m_pSettings->SetRedirectionMicrophoneParameters(ui->leAudin->text());

    QStringList lstDrives;
    //获取选中的行，默认获取选中行的第一列数据（0），列的索引值和上面一样0、1、2、3......
    QModelIndexList selected = ui->tvDrive->selectionModel()->selectedRows(0);
    QList<QModelIndex>::iterator it;
    QModelIndex modelIndex;
    QString szPath;
    for (it = selected.begin(); it != selected.end(); ++it)
    {
        modelIndex = *it;
        szPath = m_pFileModel->filePath(modelIndex);
        if(!szPath.isEmpty())
            lstDrives.append(szPath);
    }
    m_pSettings->SetRedirectionDrives(lstDrives);

    // Security
    m_pSettings->SetNegotiateSecurityLayer(ui->cbSecurityEnable->isChecked());
    uint security = 0;
    if(ui->cbSecurityRDP->isChecked())
        security |= CParameterFreeRDP::Security::RDP;
    if(ui->cbSecurityTls->isChecked())
        security |= CParameterFreeRDP::Security::TLS;
    if(ui->cbSecurityNLA->isChecked())
        security |= CParameterFreeRDP::Security::NLA;
    if(ui->cbSecurityNlaExt->isChecked())
        security |= CParameterFreeRDP::Security::NLA_Ext;
    if(ui->cbSecurityRDSTLS->isChecked())
        security |= CParameterFreeRDP::Security::RDSTLS;
    if(ui->cbSecurityRDSAAD->isChecked())
        security |= CParameterFreeRDP::Security::RDSAAD;
    m_pSettings->SetSecurity((CParameterFreeRDP::Security)security);

    // Tls version
    if(ui->rbTls1_0->isChecked())
        m_pSettings->SetTlsVersion(TLS1_VERSION);
    else if(ui->rbTls1_1->isChecked())
        m_pSettings->SetTlsVersion(TLS1_1_VERSION);
    else if(ui->rbTls1_2->isChecked())
        m_pSettings->SetTlsVersion(TLS1_2_VERSION);
    else if(ui->rbTls1_3->isChecked())
        m_pSettings->SetTlsVersion(TLS1_3_VERSION);

    // Connect type
    m_pSettings->SetConnectType(ui->cbConnectType->currentData().toUInt());
    // Perfomance flags
    UINT32 performanceFlags = 0;
    if(!ui->cbDesktopBackground->isChecked())
        performanceFlags |= PERF_DISABLE_WALLPAPER;
    if(!ui->cbWindowDrag->isChecked())
        performanceFlags |= PERF_DISABLE_FULLWINDOWDRAG;
    if(!ui->cbMenuAnims->isChecked())
        performanceFlags |= PERF_DISABLE_MENUANIMATIONS;
    if(!ui->cbThemes->isChecked())
        performanceFlags |= PERF_DISABLE_THEMING;
    if(ui->cbFontSmoothing->isChecked())
        performanceFlags |= PERF_ENABLE_FONT_SMOOTHING;
    if(ui->cbDesktopCompositing->isChecked())
        performanceFlags |= PERF_ENABLE_DESKTOP_COMPOSITION;
    m_pSettings->SetPerformanceFlags(performanceFlags);

    accept();
}

void CDlgSetFreeRDP::on_pbCancel_clicked()
{
    reject();
}

void CDlgSetFreeRDP::on_rbLocalScreen_clicked(bool checked)
{
    if(!checked) return;
    int width = GetScreenGeometry().width();
    int height = GetScreenGeometry().height();
    QString curSize = QString::number(width) + "×" + QString::number(height);
    ui->rbLocalScreen->setText(tr("Local screen") + ": " + curSize);
    if(ui->cbDesktopSize->findText(curSize) == -1)
        InsertDesktopSize(width, height);
    ui->cbDesktopSize->setCurrentText(curSize);
}

QRect CDlgSetFreeRDP::GetScreenGeometry()
{
    QRect r;
    QScreen* pScreen = QApplication::primaryScreen();
    if(ui->cbAllMonitor->isChecked())
    {
        //TODO: check this is a virtual geometry
        r = pScreen->virtualGeometry();
        /*
        auto lstScreen = QApplication::screens();
        qDebug(log) << "Screen counts:" << lstScreen.count();
        int i = 0;
        foreach(auto pScreen, lstScreen)
        {
            //r = r.united(pScreen->geometry());
            qDebug(log)
                << "ID:" << i++
                << "geometry:" << pScreen->geometry()
                << "availableGeometry:" << pScreen->availableGeometry()
                << "virtualGeometry" << pScreen->virtualGeometry()
                << "availableVirtualGeometry" << pScreen->availableVirtualGeometry();
        }//*/
    } else {
        r = pScreen->geometry();
    }
    return r;
}

int CDlgSetFreeRDP::UpdateDesktopSize()
{
    ui->cbDesktopSize->clear();
    QStringList lstDesktopSizes;
    lstDesktopSizes <<"640×480"
                      <<"800×600"
                      <<"1024×600"
                      <<"1024×768"
                      <<"1280×720"
                      <<"1280×854"
                      <<"1280×960"
                      <<"1280×1024"
                      <<"1366×768"
                      <<"1400×1050"
                      <<"1440×900"
                      <<"1600×900"
                      <<"1600×1024"
                      <<"1600×1200"
                      <<"1680×1050"
                      <<"1920×1080"
                      <<"1920×1200";
    ui->cbDesktopSize->addItems(lstDesktopSizes);
    
    int width = GetScreenGeometry().width();
    int height = GetScreenGeometry().height();
    QString curSize = QString::number(width) + "×" + QString::number(height);
    ui->rbLocalScreen->setText(tr("Local screen") + ": " + curSize);
    if(ui->cbDesktopSize->findText(curSize) == -1)
        InsertDesktopSize(width, height);
    return 0;
}

int CDlgSetFreeRDP::InsertDesktopSize(QString szSize)
{
    int w, h;
    int nIndex = szSize.indexOf("×");
    if(nIndex > -1)
    {
        bool ok = false;
        w = szSize.left(nIndex).toInt(&ok);
        if(ok)
            h = szSize.right(szSize.length() - nIndex - 1).toInt(&ok);
        if(!ok)
            return -1;
    }
    return InsertDesktopSize(w, h);
}

int CDlgSetFreeRDP::InsertDesktopSize(int width, int height)
{
    QString curSize = QString::number(width) + "×" + QString::number(height);
    if(ui->cbDesktopSize->findText(curSize) > -1)
        return 0;

    int nCount = ui->cbDesktopSize->count();
    for(int i = 0; i < nCount; i++)
    {
        QString curText = ui->cbDesktopSize->itemText(i);
        int nIndex = curText.indexOf("×");
        if(nIndex > -1)
        {
            int w = curText.left(nIndex).toInt();
            int h = curText.right(curText.length() - nIndex - 1).toInt();
            if(w > width)
            {
                ui->cbDesktopSize->insertItem(i, curSize);
                return 0;
            } else if(w == width) {
                if(h > height)
                {
                    ui->cbDesktopSize->insertItem(i, curSize);
                    return 0;
                } if(h == height) {
                    return 0;
                } else {
                    int j = i + 1;
                     while(j < nCount) {
                        QString curText = ui->cbDesktopSize->itemText(j);
                        int nIndex = curText.indexOf("×");
                        if(-1 >= nIndex) {
                            j++;
                        } else {
                            int w = curText.left(nIndex).toInt();
                            int h = curText.right(curText.length() - nIndex - 1).toInt();
                            if(w != width) {
                                ui->cbDesktopSize->insertItem(j, curSize);
                                return 0;
                            } else {
                                if(h > height)
                                {
                                    ui->cbDesktopSize->insertItem(j, curSize);
                                    return 0;
                                } else if(h == height)
                                    return 0;
                                else
                                    j++;
                            }
                        }
                    }
                }
            }
        }
    }

    if(ui->cbDesktopSize->findText(curSize) == -1)
        ui->cbDesktopSize->addItem(curSize);
    
    return 0;
}


void CDlgSetFreeRDP::on_rbAudioDisable_toggled(bool checked)
{
    ui->leRdpSnd->setEnabled(!checked);
}

void CDlgSetFreeRDP::on_rbAudioLocal_toggled(bool checked)
{
    ui->leRdpSnd->setEnabled(checked);
}

void CDlgSetFreeRDP::on_rbAudioRemote_toggled(bool checked)
{
    ui->leRdpSnd->setEnabled(!checked);
    ui->leAudin->setEnabled(!checked);
    ui->cbAudin->setEnabled(!checked);
}

bool CDlgSetFreeRDP::HasAudioOutput()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    auto d = QMediaDevices::defaultAudioOutput();
    return !d.isNull();
#else
    return !QAudioDeviceInfo::defaultOutputDevice().isNull();
#endif
}

bool CDlgSetFreeRDP::HasAudioInput()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    auto d = QMediaDevices::defaultAudioInput();
    return !d.isNull();
#else
    return !QAudioDeviceInfo::defaultInputDevice().isNull();
#endif
}

void CDlgSetFreeRDP::on_pbSizeEdit_clicked()
{
    CDlgDesktopSize dlg;
    QStringList lstSize;
    for(int i = 0; i < ui->cbDesktopSize->count(); i++)
        lstSize << ui->cbDesktopSize->itemText(i);
    dlg.SetDesktopSizes(lstSize);
    if(QDialog::Accepted == dlg.exec())
    {
        ui->cbDesktopSize->clear();
        foreach(auto s, dlg.GetDesktopSize())
        {
            InsertDesktopSize(s);
        }
    }
}

void CDlgSetFreeRDP::on_pbDriveClearAll_clicked()
{
    ui->tvDrive->clearSelection();
    ShowDriveSelected(0);
}

void CDlgSetFreeRDP::slotSelectionChanged(const QItemSelection &selected,
                                          const QItemSelection &deselected)
{
    QModelIndexList s = ui->tvDrive->selectionModel()->selectedRows(0);
    ShowDriveSelected(s.size());
}

int CDlgSetFreeRDP::ShowDriveSelected(int counts)
{
    ui->lbDriveSelected->setText(tr("Selected counts: ") + QString::number(counts));
    return 0;
}

void CDlgSetFreeRDP::on_cbAllMonitor_stateChanged(int arg1)
{
    on_rbLocalScreen_clicked(true);
}

void CDlgSetFreeRDP::on_cbSecurityEnable_checkStateChanged(const Qt::CheckState &arg1)
{
    if(m_pButtonGroup) {
        m_pButtonGroup->deleteLater();
        m_pButtonGroup = nullptr;
    }
    if(!ui->cbSecurityEnable->isChecked()) {
        m_pButtonGroup = new QButtonGroup(this);
        m_pButtonGroup->addButton(ui->cbSecurityRDP);
        m_pButtonGroup->addButton(ui->cbSecurityTls);
        m_pButtonGroup->addButton(ui->cbSecurityNLA);
        m_pButtonGroup->addButton(ui->cbSecurityNlaExt);
        m_pButtonGroup->addButton(ui->cbSecurityRDSAAD);
        m_pButtonGroup->addButton(ui->cbSecurityRDSTLS);
    }
}

void CDlgSetFreeRDP::on_cbConnectType_currentIndexChanged(int index)
{
    UINT32 type = ui->cbConnectType->itemData(index).toUInt();
    if(0 >= type || CONNECTION_TYPE_AUTODETECT < type)
        return;

    UINT32 performanceFlags = m_vPerformanceFlags[type - 1];
    if(CONNECTION_TYPE_AUTODETECT == type)
    {
        performanceFlags = PERF_ENABLE_FONT_SMOOTHING | PERF_ENABLE_DESKTOP_COMPOSITION;
    }

    ui->cbDesktopBackground->setChecked(!(PERF_DISABLE_WALLPAPER & performanceFlags));
    ui->cbWindowDrag->setChecked(!(PERF_DISABLE_FULLWINDOWDRAG & performanceFlags));
    ui->cbMenuAnims->setChecked(!(PERF_DISABLE_MENUANIMATIONS & performanceFlags));
    ui->cbThemes->setChecked(!(PERF_DISABLE_THEMING & performanceFlags));
    ui->cbFontSmoothing->setChecked(PERF_ENABLE_FONT_SMOOTHING & performanceFlags);
    ui->cbDesktopCompositing->setChecked(PERF_ENABLE_DESKTOP_COMPOSITION & performanceFlags);

    ui->cbDesktopBackground->setEnabled(!(CONNECTION_TYPE_AUTODETECT == type));
    ui->cbWindowDrag->setEnabled(!(CONNECTION_TYPE_AUTODETECT == type));
    ui->cbMenuAnims->setEnabled(!(CONNECTION_TYPE_AUTODETECT == type));
    ui->cbThemes->setEnabled(!(CONNECTION_TYPE_AUTODETECT == type));
    ui->cbFontSmoothing->setEnabled(!(CONNECTION_TYPE_AUTODETECT == type));
    ui->cbDesktopCompositing->setEnabled(!(CONNECTION_TYPE_AUTODETECT == type));
}
