#include "FrmParameterTerminalAppearanceSettings.h"
#include "ui_FrmParameterTerminalAppearanceSettings.h"
#include "RabbitCommonLog.h"
#include "RabbitCommonDir.h"
#include "qtermwidget.h"

#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"

#include <QCoreApplication>
#include <QLocale>
#include <QDebug>
#include <QTranslator>
#include <QFont>

class CRabbitRemoteControlTerminal
{
public:
    CRabbitRemoteControlTerminal()
    {
    #if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
        Q_INIT_RESOURCE(translations_RabbitRemoteControlTerminal);
    #endif
    
        QString szTranslatorFile = RabbitCommon::CDir::Instance()->GetDirTranslations()
                + QDir::separator() + "RabbitRemoteControlTerminal_" + QLocale::system().name() + ".qm";
        if(!m_Translator.load(szTranslatorFile))
            qCritical() << "Open translator file fail:" << szTranslatorFile;
        qApp->installTranslator(&m_Translator);
    };
    
    ~CRabbitRemoteControlTerminal()
    {
        qApp->removeTranslator(&m_Translator);
        qDebug() << "CRabbitRemoteControlTerminal::~CRabbitRemoteControlTerminal()";
    #if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
        Q_INIT_RESOURCE(translations_RabbitRemoteControlTerminal);
    #endif
    };
    
private:
    QTranslator m_Translator;
};

const CRabbitRemoteControlTerminal g_CRabbitRemoteControlTerminal;

CFrmParameterTerminalAppearanceSettings::CFrmParameterTerminalAppearanceSettings(CParameterTerminal *pPara, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmParameterTerminalAppearanceSettings),
    m_pPara(pPara)
{
    ui->setupUi(this);
    
    Q_ASSERT(pPara);
    if(!pPara) return;

    QFont f = pPara->font;
    ui->spFontSize->setValue(f.pointSize());
    ui->lbFont->setFont(f);
    
    ui->fontComboBox->setFontFilters(QFontComboBox::MonospacedFonts
                                     | QFontComboBox::NonScalableFonts
                                     | QFontComboBox::ScalableFonts);
    ui->fontComboBox->setCurrentFont(f);
    ui->fontComboBox->setEditable(false);
    
    ui->cbCursorShape->addItem(tr("BlockCursor"), (int)Konsole::Emulation::KeyboardCursorShape::BlockCursor);
    ui->cbCursorShape->addItem(tr("UnderlineCursor"), (int)Konsole::Emulation::KeyboardCursorShape::UnderlineCursor);
    ui->cbCursorShape->addItem(tr("IBeamCursor"), (int)Konsole::Emulation::KeyboardCursorShape::BlockCursor);
    ui->cbCursorShape->setCurrentIndex((int)pPara->cursorShape);
    
    ui->cbColorScheme->addItems(QTermWidget::availableColorSchemes());
    if(!m_pPara->colorScheme.isEmpty())
        ui->cbColorScheme->setCurrentText(pPara->colorScheme);
    
    ui->cbScrollBarPositioin->addItem(tr("No"), QTermWidget::NoScrollBar);
    ui->cbScrollBarPositioin->addItem(tr("Left"), QTermWidget::ScrollBarLeft);
    ui->cbScrollBarPositioin->addItem(tr("Right"), QTermWidget::ScrollBarRight);
    ui->cbScrollBarPositioin->setCurrentIndex(pPara->scrollBarPosition);

    ui->spTerminalTransparecy->setValue(pPara->termTransparency);
    ui->cbFlowControl->setChecked(m_pPara->flowControl);
    ui->leImage->setText(m_pPara->backgroupImage);
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
    m_pPara->font = ui->fontComboBox->currentFont();
    m_pPara->font.setPointSize(ui->spFontSize->value());
    m_pPara->colorScheme = ui->cbColorScheme->currentText();
    m_pPara->cursorShape = static_cast<Konsole::Emulation::KeyboardCursorShape>(ui->cbCursorShape->currentData().toInt());
    m_pPara->scrollBarPosition = static_cast<QTermWidget::ScrollBarPosition>(ui->cbScrollBarPositioin->currentData().toInt());
    m_pPara->termTransparency = ui->spTerminalTransparecy->value();
    m_pPara->flowControl = ui->cbFlowControl->isChecked();
    m_pPara->backgroupImage = ui->leImage->text();
    return 0;
}

void CFrmParameterTerminalAppearanceSettings::on_pbBrower_clicked()
{
    QString file = RabbitCommon::CDir::Instance()->GetOpenFileName(this,
                                               tr("Backgroup image"), QString(),
                tr("Images (*.png *.xpm *.jpg *.bmp *.jpeg);;All files (*.*)"));
    if(!file.isEmpty())
        ui->leImage->setText(file);
}
