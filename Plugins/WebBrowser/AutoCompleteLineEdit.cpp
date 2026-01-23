// Author: Kang Lin <kl222@126.com>

#include <QApplication>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionFrame>
#include <QLoggingCategory>

#include "AutoCompleteLineEdit.h"

static Q_LOGGING_CATEGORY(log, "Web.LineEdit")
CAutoCompleteLineEdit::CAutoCompleteLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    initTimer();
}

CAutoCompleteLineEdit::CAutoCompleteLineEdit(const QString &contents, QWidget *parent)
    : QLineEdit(contents, parent)
{
    initTimer();
}

void CAutoCompleteLineEdit::initTimer()
{
    // 设置定时器，避免频繁更新
    m_updateTimer.setSingleShot(true);
    m_updateTimer.setInterval(100); // 100ms延迟

    connect(this, &QLineEdit::textChanged, this, [this]() {
        m_updateTimer.start();
    });

    connect(&m_updateTimer, &QTimer::timeout, this, &CAutoCompleteLineEdit::updateCompletion);

    QPalette palette = QApplication::palette();
    m_suggestionColor = palette.color(QPalette::HighlightedText);
    m_suggestionBackground = palette.color(QPalette::Highlight);
}

void CAutoCompleteLineEdit::setCompletions(const QStringList &completions)
{
    m_completions = completions;
    m_currentSuggestion.clear();
}

void CAutoCompleteLineEdit::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Tab:
    case Qt::Key_Right:
        // 按下Tab或右箭头接受建议
        acceptSuggestion();
        event->accept();
        return;
    case Qt::Key_Escape:
        // ESC取消建议
        hideSuggestion();
        event->accept();
        return;
    default:
        QLineEdit::keyPressEvent(event);
    }
}

void CAutoCompleteLineEdit::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    if (!text().isEmpty()) {
        updateCompletion();
    }
}

void CAutoCompleteLineEdit::focusOutEvent(QFocusEvent *event)
{
    hideSuggestion();
    QLineEdit::focusOutEvent(event);
}

void CAutoCompleteLineEdit::updateCompletion()
{
    QString input = text();

    if (input.isEmpty()) {
        hideSuggestion();
        return;
    }

    QString suggestion = findBestMatch(input);
    if (!suggestion.isEmpty() && suggestion != input) {
        m_currentSuggestion = suggestion;
        showSuggestion();
    } else {
        hideSuggestion();
    }
}

QColor CAutoCompleteLineEdit::GetSuggestionColor() const
{
    return m_suggestionColor;
}

void CAutoCompleteLineEdit::SetSuggestionColor(const QColor &newSuggestionColor)
{
    m_suggestionColor = newSuggestionColor;
}

QColor CAutoCompleteLineEdit::GetSuggestionBackground() const
{
    return m_suggestionBackground;
}

void CAutoCompleteLineEdit::SetSuggestionBackground(const QColor &newSuggestionBackground)
{
    m_suggestionBackground = newSuggestionBackground;
}

QString CAutoCompleteLineEdit::findBestMatch(const QString &input) const
{
    if (m_completions.isEmpty()) {
        return QString();
    }

    QString lowerInput = input.toLower();

    // 1. 优先查找开头匹配的
    for (const QString &completion : m_completions) {
        if (completion.toLower().startsWith(lowerInput)) {
            return completion;
        }
    }

    // 2. 查找包含匹配的
    for (const QString &completion : m_completions) {
        if (completion.toLower().contains(lowerInput)) {
            return completion;
        }
    }

    return QString();
}

void CAutoCompleteLineEdit::showSuggestion()
{
    if (m_currentSuggestion.isEmpty() || m_currentSuggestion == text()) {
        return;
    }

    QString currentText = text();
    QString suggestion = m_currentSuggestion;

    // 计算需要显示的补全部分
    QString complement = suggestion.mid(currentText.length());

    if (!complement.isEmpty()) {
        // 设置带有建议的文本显示
        QString displayText = currentText +
                              QString("<span style='color: gray; background-color: #f0f0f0;'>%1</span>")
                                  .arg(complement.toHtmlEscaped());

        // 这里我们使用paintEvent来绘制，稍后实现
        update();
    }
}

void CAutoCompleteLineEdit::hideSuggestion()
{
    m_currentSuggestion.clear();
    update();
}

void CAutoCompleteLineEdit::acceptSuggestion()
{
    if (!m_currentSuggestion.isEmpty()) {
        setText(m_currentSuggestion);
        hideSuggestion();
        emit editingFinished();
    }
}

void CAutoCompleteLineEdit::paintEvent(QPaintEvent *event)
{
    // 先调用基类的绘制
    QLineEdit::paintEvent(event);

    if (m_currentSuggestion.isEmpty() || !hasFocus())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 如果有建议，绘制背景补全
    QString currentText = text();
    if (currentText.isEmpty() || currentText == m_currentSuggestion) {
        return;
    }

    // 计算补全部分
    QString complement = m_currentSuggestion; //.mid(currentText.length());
    if (complement.isEmpty()) {
        return;
    }

    // 获取当前文本的位置和大小
    QFontMetrics fm(font());
    int textWidth = fm.horizontalAdvance(currentText);
    int textHeight = fm.height();

    // 计算光标位置
    int cursorPos = cursorPosition();
    QString textBeforeCursor = currentText.left(cursorPos);
    int cursorX = fm.horizontalAdvance(textBeforeCursor);

    // 计算补全文本的起始位置
    QRect contentRect = contentsRect();

    int leftMargin = 0;

    // 获取文本对齐方式
    Qt::Alignment alignment = this->alignment();
    if (alignment & Qt::AlignHCenter) {
        leftMargin = (contentRect.width() - fm.horizontalAdvance(currentText + complement)) / 2;
    } else if (alignment & Qt::AlignRight) {
        leftMargin = contentRect.width() - fm.horizontalAdvance(currentText + complement);
    }

    // 绘制背景补全文本
    painter.save();

    painter.setPen(m_suggestionColor);

    int left = height()/*icon*/ + fm.horizontalAdvance("W") + leftMargin + textWidth;

    // 如果使用背景色
    QRect rBackground(left,
          (height() - textHeight) / 2,
          fm.horizontalAdvance(complement),
          textHeight);
    painter.fillRect(rBackground,
                     m_suggestionBackground);

    qDebug(log) << "currentText width:" << textWidth << "; height:" << textHeight
                << currentText << "Background:" << rBackground
                << "; cursorX:" << cursorX << "; cursorPos:" << cursorPos;

    painter.drawText(left,
                     (height() + fm.ascent() - fm.descent()) / 2,
                     complement);
    painter.restore();

}
