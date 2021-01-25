#include "FrmTerminalAppearanceSettings.h"
#include "ui_FrmTerminalAppearanceSettings.h"
#include "RabbitCommonLog.h"
#include <QDebug>
#include "qtermwidget.h"

CFrmTerminalAppearanceSettings::CFrmTerminalAppearanceSettings(CParameterTerminalAppearance *pPara, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmTerminalAppearanceSettings),
    m_pPara(pPara)
{
    ui->setupUi(this);
    
    if(!pPara) return;

    ui->cbCursorShape->addItem("BlockCursor", (int)Konsole::Emulation::KeyboardCursorShape::BlockCursor);
    ui->cbCursorShape->addItem("UnderlineCursor", (int)Konsole::Emulation::KeyboardCursorShape::UnderlineCursor);
    ui->cbCursorShape->addItem("IBeamCursor", (int)Konsole::Emulation::KeyboardCursorShape::BlockCursor);
    ui->cbCursorShape->setCurrentIndex((int)pPara->cursorShape);
    
    ui->cbColorScheme->addItems(QTermWidget::availableColorSchemes());
    ui->cbColorScheme->setCurrentText(pPara->colorScheme);
    
    ui->cbScrollBarPositioin->addItem(tr("No"), QTermWidget::NoScrollBar);
    ui->cbScrollBarPositioin->addItem(tr("Left"), QTermWidget::ScrollBarLeft);
    ui->cbScrollBarPositioin->addItem(tr("Right"), QTermWidget::ScrollBarRight);
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

}

CFrmTerminalAppearanceSettings::~CFrmTerminalAppearanceSettings()
{
    delete ui;
}

void CFrmTerminalAppearanceSettings::on_fontComboBox_currentFontChanged(const QFont &f)
{
    if(!m_pPara) return;
    m_pPara->font = f;
    m_pPara->font.setPointSize(ui->spFontSize->value());
    ui->lbFont->setFont(m_pPara->font);
}

void CFrmTerminalAppearanceSettings::on_spFontSize_valueChanged(int size)
{
    if(!m_pPara) return;
    m_pPara->font = ui->fontComboBox->font();
    m_pPara->font.setPointSize(size);
    ui->lbFont->setFont(m_pPara->font);
}

void CFrmTerminalAppearanceSettings::showEvent(QShowEvent *event)
{
}

int CFrmTerminalAppearanceSettings::Acceptsettings()
{
    if(!m_pPara) return -1;
    
    return 0;
}

void CFrmTerminalAppearanceSettings::on_cbColorScheme_currentIndexChanged(const QString &arg1)
{
    if(!m_pPara) return;
    m_pPara->colorScheme = arg1;
}

void CFrmTerminalAppearanceSettings::on_cbCursorShape_currentIndexChanged(int index)
{
    if(!m_pPara) return;
    m_pPara->cursorShape = static_cast<Konsole::Emulation::KeyboardCursorShape>(index);
}

void CFrmTerminalAppearanceSettings::on_cbScrollBarPositioin_currentIndexChanged(int index)
{
    if(!m_pPara) return;
    m_pPara->scrollBarPosition = static_cast<QTermWidget::ScrollBarPosition>(index);
}
