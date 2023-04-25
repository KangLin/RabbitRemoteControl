#include "FrmParameterTerminalAppearanceSettings.h"
#include "ui_FrmParameterTerminalAppearanceSettings.h"
#include "RabbitCommonDir.h"
#include "qtermwidget.h"

#include "RabbitCommonDir.h"

#include <QCoreApplication>
#include <QLocale>
#include <QDebug>
#include <QTranslator>
#include <QFont>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Terminal)

class CTerminal
{
public:
    CTerminal()
    { 
        QString szTranslatorFile = RabbitCommon::CDir::Instance()->GetDirTranslations()
                + QDir::separator() + "Terminal_" + QLocale::system().name() + ".qm";
        if(!m_Translator.load(szTranslatorFile))
            qCritical(Terminal) << "Open translator file fail:" << szTranslatorFile;
        qApp->installTranslator(&m_Translator);
    };
    
    ~CTerminal()
    {
        qApp->removeTranslator(&m_Translator);
        qDebug(Terminal) << "CTerminal::~CTerminal()";
    };
    
private:
    QTranslator m_Translator;
};

const CTerminal g_CTerminal;

CFrmParameterTerminalAppearanceSettings::CFrmParameterTerminalAppearanceSettings(CParameterTerminal *pPara, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmParameterTerminalAppearanceSettings),
    m_pPara(pPara)
{
    ui->setupUi(this);
    
    Q_ASSERT(pPara);
    if(!pPara) return;

    QFont f = pPara->GetFont();
    ui->spFontSize->setValue(f.pointSize());
    ui->lbFont->setFont(f);
    
    ui->fontComboBox->setFontFilters(QFontComboBox::MonospacedFonts
                                     | QFontComboBox::NonScalableFonts
                                     | QFontComboBox::ScalableFonts);
    ui->fontComboBox->setCurrentFont(f);
    ui->fontComboBox->setEditable(false);
    
    ui->cbShowResize->setChecked(pPara->GetSizeHint());
    
    ui->cbCursorShape->addItem(tr("BlockCursor"), (int)Konsole::Emulation::KeyboardCursorShape::BlockCursor);
    ui->cbCursorShape->addItem(tr("UnderlineCursor"), (int)Konsole::Emulation::KeyboardCursorShape::UnderlineCursor);
    ui->cbCursorShape->addItem(tr("IBeamCursor"), (int)Konsole::Emulation::KeyboardCursorShape::BlockCursor);
    ui->cbCursorShape->setCurrentIndex((int)pPara->GetCursorShape());
    
    ui->cbColorScheme->addItems(QTermWidget::availableColorSchemes());
    if(!m_pPara->GetColorScheme().isEmpty())
        ui->cbColorScheme->setCurrentText(pPara->GetColorScheme());
    
    ui->cbScrollBarPositioin->addItem(tr("No"), QTermWidget::NoScrollBar);
    ui->cbScrollBarPositioin->addItem(tr("Left"), QTermWidget::ScrollBarLeft);
    ui->cbScrollBarPositioin->addItem(tr("Right"), QTermWidget::ScrollBarRight);
    ui->cbScrollBarPositioin->setCurrentIndex(pPara->GetScrollBarPosition());

    ui->spTerminalTransparecy->setValue(pPara->GetTransparency());
    ui->cbFlowControl->setChecked(m_pPara->GetFlowControl());
    ui->cbBiDirectional->setChecked(m_pPara->GetDirectional());
    ui->leImage->setText(m_pPara->GetBackgroupImage());
}

CFrmParameterTerminalAppearanceSettings::~CFrmParameterTerminalAppearanceSettings()
{
    delete ui;
}

void CFrmParameterTerminalAppearanceSettings::on_fontComboBox_currentFontChanged(const QFont &f)
{
    QFont font = f;
    font.setPointSize(ui->spFontSize->value());
    ui->lbFont->setFont(font);
    ui->lbFont->setText(tr("Example: Display font"));
}

void CFrmParameterTerminalAppearanceSettings::on_spFontSize_valueChanged(int size)
{
    QFont font = ui->fontComboBox->currentFont();
    font.setPointSize(size);
    ui->lbFont->setFont(font);
    ui->lbFont->setText(tr("Example: Display font"));
}

int CFrmParameterTerminalAppearanceSettings::AcceptSettings()
{
    if(!m_pPara) return -1;
    m_pPara->SetFont(ui->fontComboBox->currentFont());
    QFont font = m_pPara->GetFont();
    font.setPointSize(ui->spFontSize->value());
    m_pPara->SetFont(font);
    m_pPara->SetSizeHint(ui->cbShowResize->isChecked());
    m_pPara->SetColorScheme(ui->cbColorScheme->currentText());
    m_pPara->SetCursorShape(static_cast<Konsole::Emulation::KeyboardCursorShape>(ui->cbCursorShape->currentData().toInt()));
    m_pPara->SetScrollBarPosition(static_cast<QTermWidget::ScrollBarPosition>(ui->cbScrollBarPositioin->currentData().toInt()));
    m_pPara->SetTransparency(ui->spTerminalTransparecy->value());
    m_pPara->SetFlowControl(ui->cbFlowControl->isChecked());
    m_pPara->SetBackgroupImage(ui->leImage->text());
    m_pPara->SetDirectional(ui->cbBiDirectional->isChecked());
    return 0;
}

void CFrmParameterTerminalAppearanceSettings::on_pbBrowser_clicked()
{
    QString file = RabbitCommon::CDir::Instance()->GetOpenFileName(this,
                                               tr("Backgroup image"), QString(),
                tr("Images (*.png *.xpm *.jpg *.bmp *.jpeg);;All files (*.*)"));
    if(!file.isEmpty())
        ui->leImage->setText(file);
}
