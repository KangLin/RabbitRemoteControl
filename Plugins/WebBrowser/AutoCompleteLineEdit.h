// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QLineEdit>
#include <QStringList>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QTimer>

class CAutoCompleteLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit CAutoCompleteLineEdit(QWidget *parent = nullptr);
    explicit CAutoCompleteLineEdit(const QString &contents, QWidget *parent = nullptr);

    void setCompletions(const QStringList &completions);

    QColor GetSuggestionColor() const;
    void SetSuggestionColor(const QColor &newSuggestionColor);
    QColor GetSuggestionBackground() const;
    void SetSuggestionBackground(const QColor &newSuggestionBackground);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

private slots:
    void updateCompletion();

private:
    QStringList m_completions;
    QString m_currentSuggestion;

    QColor m_suggestionColor;
    QColor m_suggestionBackground;

    QTimer m_updateTimer;

    void initTimer();
    void showSuggestion();
    void hideSuggestion();
    void acceptSuggestion();
    QString findBestMatch(const QString &input) const;
};

