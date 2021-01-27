#include "FrmTerminalAppearanceSettings.h"
#include "ui_FrmTerminalAppearanceSettings.h"
#include "RabbitCommonLog.h"
#include "RabbitCommonDir.h"
#include <QDebug>
#include "qtermwidget.h"

CFrmTerminalAppearanceSettings::CFrmTerminalAppearanceSettings(CParameterTerminalAppearance *pPara, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmTerminalAppearanceSettings),
    m_pPara(pPara)
{
    ui->setupUi(this);
    
    Q_ASSERT(pPara);
    if(!pPara) return;

    ui->cbCursorShape->addItem("BlockCursor", (int)Konsole::Emulation::KeyboardCursorShape::BlockCursor);
    ui->cbCursorShape->addItem("UnderlineCursor", (int)Konsole::Emulation::KeyboardCursorShape::UnderlineCursor);
    ui->cbCursorShape->addItem("IBeamCursor", (int)Konsole::Emulation::KeyboardCursorShape::BlockCursor);
    ui->cbCursorShape->setCurrentIndex((int)pPara->cursorShape);
    
    ui->cbColorScheme->addItems(QTermWidget::availableColorSchemes());
    if(!m_pPara->colorScheme.isEmpty())
        ui->cbColorScheme->setCurrentText(pPara->colorScheme);
    
    ui->cbScrollBarPositioin->addItem(tr("No"), QTermWidget::NoScrollBar);
    ui->cbScrollBarPositioin->addItem(tr("Left"), QTermWidget::ScrollBarLeft);
    ui->cbScrollBarPositioin->addItem(tr("Right"), QTermWidget::ScrollBarRight);
    ui->cbScrollBarPositioin->setCurrentIndex(pPara->scrollBarPosition);

    qDebug() << QTermWidget::availableKeyBindings();

//    QFont font = QApplication::font();
//#ifdef Q_OS_MACOS
//    font.setFamily(QStringLiteral("Monaco"));
//#elif defined(Q_WS_QWS)
//    font.setFamily(QStringLiteral("fixed"));
//#else
//    font.setFamily(QStringLiteral("Monospace"));
//#endif
//    font.setPointSize(12);

    QFont f = pPara->font;
    ui->spFontSize->setValue(f.pointSize());
    ui->lbFont->setFont(f);

    ui->spTerminalTransparecy->setValue(pPara->termTransparency);
    ui->cbFlowControl->setChecked(m_pPara->flowControl);
    ui->leImage->setText(m_pPara->backgroupImage);
}

CFrmTerminalAppearanceSettings::~CFrmTerminalAppearanceSettings()
{
    delete ui;
}

void CFrmTerminalAppearanceSettings::on_fontComboBox_currentFontChanged(const QFont &f)
{
    QFont font = f;
    font.setPointSize(ui->spFontSize->value());
    ui->lbFont->setFont(font);
}

void CFrmTerminalAppearanceSettings::on_spFontSize_valueChanged(int size)
{
    QFont font = ui->fontComboBox->currentFont();
    font.setPointSize(size);
    ui->lbFont->setFont(font);
}

int CFrmTerminalAppearanceSettings::AcceptSettings()
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

void CFrmTerminalAppearanceSettings::on_pbBrower_clicked()
{
    QString file = RabbitCommon::CDir::Instance()->GetOpenFileName(this,
                                               tr("Backgroup image"), QString(),
                tr("Images (*.png *.xpm *.jpg *.bmp *.jpeg);;All files (*.*)"));
    if(!file.isEmpty())
        ui->leImage->setText(file);
}
