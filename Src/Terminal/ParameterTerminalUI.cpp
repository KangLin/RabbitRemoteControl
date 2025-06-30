#include <QCoreApplication>
#include <QLocale>
#include <QDebug>
#include <QTranslator>
#include <QFont>
#include <QLoggingCategory>

#include "qtermwidget.h"

#include "ParameterTerminalUI.h"
#include "ui_ParameterTerminalUI.h"

#include "RabbitCommonDir.h"
#include "RabbitCommonTools.h"

static Q_LOGGING_CATEGORY(log, "Parameter.Terminal.UI")

CParameterTerminalUI::CParameterTerminalUI(QWidget *parent) :
    CParameterUI(parent),
    ui(new Ui::CParameterTerminalUI),
    m_pPara(nullptr)
{
    ui->setupUi(this);
}

CParameterTerminalUI::~CParameterTerminalUI()
{
    delete ui;
}

void CParameterTerminalUI::on_fontComboBox_currentFontChanged(const QFont &f)
{
    QFont font = f;
    font.setPointSize(ui->spFontSize->value());
    ui->lbFont->setFont(font);
    ui->lbFont->setText(tr("Example: Display font"));
}

void CParameterTerminalUI::on_spFontSize_valueChanged(int size)
{
    QFont font = ui->fontComboBox->currentFont();
    font.setPointSize(size);
    ui->lbFont->setFont(font);
    ui->lbFont->setText(tr("Example: Display font"));
}

int CParameterTerminalUI::Accept()
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
    m_pPara->SetKeyBindings(ui->cbKeyBinding->currentText());

    if(ui->cbHistoryUnlimited->isChecked())
        m_pPara->SetHistorySize(-1);
    else    
        m_pPara->SetHistorySize(ui->sbHistorySize->value());
    
    return 0;
}

void CParameterTerminalUI::on_pbBrowser_clicked()
{
    QString file = QFileDialog::getOpenFileName(this,
                                               tr("Backgroup image"), QString(),
                tr("Images (*.png *.xpm *.jpg *.bmp *.jpeg);;All files (*.*)"));
    if(!file.isEmpty())
        ui->leImage->setText(file);
}

int CParameterTerminalUI::SetParameter(CParameter *pParameter)
{
    m_pPara = qobject_cast<CParameterTerminal*>(pParameter);
    Q_ASSERT(m_pPara);
    if(!m_pPara) return -1;

    QFont f = m_pPara->GetFont();
    ui->spFontSize->setValue(f.pointSize());
    ui->lbFont->setFont(f);

    ui->fontComboBox->setFontFilters(QFontComboBox::MonospacedFonts
                                     | QFontComboBox::NonScalableFonts
                                     | QFontComboBox::ScalableFonts);
    ui->fontComboBox->setCurrentFont(f);
    ui->fontComboBox->setEditable(false);

    ui->cbShowResize->setChecked(m_pPara->GetSizeHint());

    ui->cbCursorShape->addItem(tr("BlockCursor"), (int)Konsole::Emulation::KeyboardCursorShape::BlockCursor);
    ui->cbCursorShape->addItem(tr("UnderlineCursor"), (int)Konsole::Emulation::KeyboardCursorShape::UnderlineCursor);
    ui->cbCursorShape->addItem(tr("IBeamCursor"), (int)Konsole::Emulation::KeyboardCursorShape::BlockCursor);
    ui->cbCursorShape->setCurrentIndex((int)m_pPara->GetCursorShape());
    
    QTermWidget::addCustomColorSchemeDir(QApplication::applicationDirPath());
    QTermWidget::addCustomColorSchemeDir(QApplication::applicationDirPath()
                                         + QDir::separator() + ".."
                                         + QDir::separator() + "share");
    ui->cbColorScheme->addItems(QTermWidget::availableColorSchemes());
    if(!m_pPara->GetColorScheme().isEmpty())
        ui->cbColorScheme->setCurrentText(m_pPara->GetColorScheme());

    ui->cbScrollBarPositioin->addItem(tr("No"), QTermWidget::NoScrollBar);
    ui->cbScrollBarPositioin->addItem(tr("Left"), QTermWidget::ScrollBarLeft);
    ui->cbScrollBarPositioin->addItem(tr("Right"), QTermWidget::ScrollBarRight);
    ui->cbScrollBarPositioin->setCurrentIndex(m_pPara->GetScrollBarPosition());

    ui->spTerminalTransparecy->setValue(m_pPara->GetTransparency());
    ui->cbFlowControl->setChecked(m_pPara->GetFlowControl());
    ui->cbBiDirectional->setChecked(m_pPara->GetDirectional());
    ui->leImage->setText(m_pPara->GetBackgroupImage());
    
    ui->cbKeyBinding->addItems(QTermWidget::availableKeyBindings());
    ui->cbKeyBinding->setCurrentText(m_pPara->GetKeyBindings());

    if(0 > m_pPara->GetHistorySize())
    {
        ui->cbHistoryUnlimited->setChecked(true);
        ui->sbHistorySize->setValue(1000);
    }
    else
    {
        ui->cbHistorySize->setChecked(true);
        ui->sbHistorySize->setValue(m_pPara->GetHistorySize());
    }
    return 0;
}
