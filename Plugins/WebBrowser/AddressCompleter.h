// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QTimer>
#include <QSqlDatabase>
#include <QPropertyAnimation>
#include "History/HistoryDatabase.h"

/*!
 * \~chinese 浏览器的地址栏自动完成功能
 * \~english Browser address bar autocomplete feature
 */
class CAddressCompleterItem : public QWidget
{
    Q_OBJECT
public:
    explicit CAddressCompleterItem(const QString &title,
                                  const QString &url,
                                  const QIcon &icon,
                                  QWidget *parent = nullptr);

    QString title() const { return m_title; }
    QString url() const { return m_url; }

private:
    QString m_title;
    QString m_url;
    QLabel *m_iconLabel;
    QLabel *m_titleLabel;
    QLabel *m_urlLabel;
};

class CAddressCompleter : public QWidget
{
    Q_OBJECT
public:
    explicit CAddressCompleter(QWidget *parent = nullptr);
    ~CAddressCompleter();

    void setHistoryDatabase(CHistoryDatabase *db);
    void attachToLineEdit(QLineEdit *lineEdit);
    void setMaxVisibleItems(int count);

signals:
    void urlSelected(const QString &url);
    void searchRequested(const QString &keyword);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private Q_SLOTS:
    void onTextChanged(const QString &text);
    void onItemClicked(QListWidgetItem *item);
    virtual void performSearch();
    void moveToNextItem();
    void moveToPreviousItem();
    void selectCurrentItem();

private:
    void setupUI();
    void showCompleter();
    void hideCompleter();
    void updateCompleterPosition();
    QIcon getIconForUrl(const QString &url);
    void addSearchSuggestions(const QString &keyword);

private:
    QLineEdit *m_pLineEdit;
    QListWidget *m_pListWidget;
    CHistoryDatabase *m_pHistoryDatabase;
    QTimer *m_pSearchTimer;
    QPropertyAnimation *m_pShowAnimation;
    QPropertyAnimation *m_pHideAnimation;
    int m_currentSelectedIndex;
    int m_maxVisibleItems;
    bool m_isCompleterVisible;
};
