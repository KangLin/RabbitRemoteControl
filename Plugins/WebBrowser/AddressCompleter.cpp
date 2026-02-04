// Author: Kang Lin <kl222@126.com>

#include <QStyle>
#include <QKeyEvent>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QScrollBar>
#include <QLoggingCategory>
#include <algorithm>
#include "AddressCompleter.h"
#include "AutoCompleteLineEdit.h"
#include "HistoryDatabase.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.Address")
CAddressCompleterItem::CAddressCompleterItem(const QString &title,
                                           const QString &url,
                                           const QIcon &icon,
                                           QWidget *parent)
    : QWidget(parent)
    , m_title(title)
    , m_url(url)
{
    setFixedHeight(40);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 4, 8, 4);
    layout->setSpacing(8);

    // 图标
    m_iconLabel = new QLabel(this);
    m_iconLabel->setFixedSize(16, 16);
    QPixmap pixmap = icon.pixmap(16, 16);
    m_iconLabel->setPixmap(pixmap);
    layout->addWidget(m_iconLabel);

    // 标题
    m_titleLabel = new QLabel(title, this);
    m_titleLabel->setStyleSheet("font-weight: bold;");
    layout->addWidget(m_titleLabel, 1);

    // URL（灰色显示）
    m_urlLabel = new QLabel(url, this);
    //m_urlLabel->setStyleSheet("color: gray; font-size: 11px;");
    layout->addWidget(m_urlLabel);

    setLayout(layout);

    // 鼠标悬停效果
    setAttribute(Qt::WA_Hover);
}

CAddressCompleter::CAddressCompleter(QWidget *parent)
    : QWidget(parent)
    , m_pLineEdit(nullptr)
    , m_pShowAnimation(nullptr)
    , m_pHideAnimation(nullptr)
    , m_currentSelectedIndex(-1)
    , m_maxVisibleItems(8)
    , m_isCompleterVisible(false)
{
    m_szEnter = tr("Enter '@' show commands") + "; "
                + tr("Enter a website URL or search content ......");
    m_szLineEditToolTip = m_szEnter  + "\n\n"
                          + tr("Enter ↲ key: Apply current url");

    m_szListWidgetToolTip += tr("Enter ↲ key: Apply current item") + "\n";
    m_szListWidgetToolTip += tr("Tab ⇆ key: Apply current item") + "\n";
    m_szListWidgetToolTip += tr("Esc Key: Exit address completer") + "\n";
    m_szListWidgetToolTip += tr("Space Key: Exit address completer") + "\n";
    m_szListWidgetToolTip += tr("↑ (Upper arrow) key: Select previous item") + "\n";
    m_szListWidgetToolTip += tr("↓ (Down arrow) key: Select next item");

    m_szLineEditToolTipShow = m_szEnter + "\n\n" + m_szListWidgetToolTip;
    setToolTip(m_szListWidgetToolTip);

    setupUI();

    // 设置搜索延迟定时器（300ms防抖动）
    m_pSearchTimer = new QTimer(this);
    if(m_pSearchTimer) {
        m_pSearchTimer->setSingleShot(true);
        m_pSearchTimer->setInterval(300);
        bool check = connect(m_pSearchTimer, &QTimer::timeout,
                             this, &CAddressCompleter::performSearch);
        Q_ASSERT(check);
    }

    // 动画效果
    m_pShowAnimation = new QPropertyAnimation(this, "geometry", this);
    m_pHideAnimation = new QPropertyAnimation(this, "geometry", this);

    // 初始隐藏
    hide();
}

CAddressCompleter::~CAddressCompleter()
{
}

void CAddressCompleter::setupUI()
{
    setWindowFlags(Qt::Tool /*Qt::ToolTip*/ | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_pListWidget = new QListWidget(this);
    if(m_pListWidget) {
        m_pListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_pListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        m_pListWidget->setFocusPolicy(Qt::NoFocus);

        bool check = connect(m_pListWidget, &QListWidget::itemClicked,
                             this, &CAddressCompleter::onItemClicked);
        Q_ASSERT(check);

        mainLayout->addWidget(m_pListWidget);
    }
    setLayout(mainLayout);

    // 设置最大高度
    int itemHeight = 40;
    int maxHeight = m_maxVisibleItems * itemHeight + 10; // 10是边框和内边距
    setMaximumHeight(maxHeight);
}

void CAddressCompleter::attachToLineEdit(QLineEdit *lineEdit)
{
    if (m_pLineEdit) {
        m_pLineEdit->removeEventFilter(this);
        m_pLineEdit->setToolTip(m_szOldLineEditToolTip);
    }

    m_pLineEdit = lineEdit;
    if (m_pLineEdit) {
        m_szOldLineEditToolTip = m_pLineEdit->toolTip();
        m_pLineEdit->setToolTip(m_szLineEditToolTip);
        m_pLineEdit->installEventFilter(this);
        connect(m_pLineEdit, &QLineEdit::textEdited,
                this, &CAddressCompleter::onTextChanged);

        // 设置提示文本
        m_pLineEdit->setPlaceholderText(m_szEnter);
    }
}

void CAddressCompleter::setMaxVisibleItems(int count)
{
    m_maxVisibleItems = count;
    int itemHeight = 40;
    int maxHeight = m_maxVisibleItems * itemHeight + 10;
    setMaximumHeight(maxHeight);
}

bool CAddressCompleter::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_pLineEdit) {
        switch (event->type()) {
        case QEvent::KeyPress: {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            //qDebug(log) << Q_FUNC_INFO << keyEvent;
            switch (keyEvent->key()) {
            case Qt::Key_Down:
            case Qt::Key_PageDown:
                if (m_isCompleterVisible) {
                    moveToNextItem();
                    return true;
                }
                break;
            case Qt::Key_Up:
            case Qt::Key_PageUp:
                if (m_isCompleterVisible) {
                    moveToPreviousItem();
                    return true;
                }
                break;
            case Qt::Key_Return:
            case Qt::Key_Enter:
                if (m_isCompleterVisible && m_currentSelectedIndex >= 0) {
                    selectCurrentItem();
                    return true;
                }
                if(m_pLineEdit) {
                    emit urlSelected(m_pLineEdit->text());
                }
                break;
            case Qt::Key_Escape:
                hideCompleter();
                break;
            case Qt::Key_Space:
                if(m_pLineEdit->text().startsWith('@'))
                    break;
                hideCompleter();
                return true;
            case Qt::Key_Tab:
                if (m_isCompleterVisible && m_currentSelectedIndex >= 0) {
                    selectCurrentItem();
                    return true;
                }
                break;
            }
            break;
        }
        case QEvent::FocusOut:
            // 延迟隐藏
            QTimer::singleShot(100, this, [this]() {
                //qDebug(log) << "lineedit focus out";
                if (!underMouse() && m_pListWidget && !m_pListWidget->underMouse()) {
                    hideCompleter();
                }
            });
            break;
        default:
            break;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void CAddressCompleter::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    m_isCompleterVisible = true;

    // 确保焦点在输入框
    if (m_pLineEdit) {
        m_pLineEdit->setFocus();
    }
}

void CAddressCompleter::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);
    m_isCompleterVisible = false;
    m_currentSelectedIndex = -1;
}

void CAddressCompleter::onTextChanged(const QString &text)
{
    if (text.isEmpty()) {
        hideCompleter();
        return;
    }

    // 重启定时器（防抖动）
    if(m_pSearchTimer)
        m_pSearchTimer->start();
}

void CAddressCompleter::performSearch()
{
    qDebug(log) << Q_FUNC_INFO;
    QString keyword = m_pLineEdit->text().trimmed();
    if (keyword.isEmpty() || !m_pListWidget) {
        hideCompleter();
        return;
    }

    // 清空现有项
    m_pListWidget->clear();
    m_currentSelectedIndex = -1;

    // 增加 “@” 命令
    if(keyword.startsWith('@')) {
        QList<Command> lstCommonds;
        lstCommonds << Command{tr("Search"), "@search:", QIcon::fromTheme("system-search")};
        lstCommonds << Command{tr("Setting"), "@setting", QIcon::fromTheme("system-settings")};
        lstCommonds << Command{tr("History"), "@history", QIcon()};
        lstCommonds << Command{tr("Bookmarks"), "@bookmarks", QIcon::fromTheme("user-bookmarks")};

        // 根据 keyword 进行排序
        std::sort(lstCommonds.begin(), lstCommonds.end(), [keyword](Command a, Command b){
            // 1. 完全匹配的排在最前面
            if (a.cmd.startsWith(keyword) && !b.cmd.startsWith(keyword))
                return true;
            if (!a.cmd.startsWith(keyword) && b.cmd.startsWith(keyword))
                return false;

            // 2. 包含关键字的排在前面
            bool aContains = a.cmd.contains(keyword, Qt::CaseInsensitive);
            bool bContains = b.cmd.contains(keyword, Qt::CaseInsensitive);
            if (aContains && !bContains) return true;
            if (!aContains && bContains) return false;

            // 3. 否则按字母顺序排序
            return a.cmd < b.cmd;
        });
        foreach(auto cmd, lstCommonds) {
            QListWidgetItem *item = new QListWidgetItem(m_pListWidget);
            item->setSizeHint(QSize(0, 40));
            item->setData(Qt::UserRole, cmd.cmd);
            CAddressCompleterItem *pCompleterItem = new CAddressCompleterItem(
                cmd.title, cmd.cmd, cmd.icon);
            if(pCompleterItem)
                m_pListWidget->setItemWidget(item, pCompleterItem);
        }
    }

    // 搜索历史记录
    QList<HistoryItem> lstHistory;
    if(CHistoryDatabase::Instance())
        lstHistory = CHistoryDatabase::Instance()->searchHistory(keyword);

    // 添加搜索结果
    int count = 0;
    QStringList addedUrls;  // 用于去重

    foreach(auto i, lstHistory) {
        QString url = i.url;
        QString title = i.title;

        // 去重
        if (addedUrls.contains(url)) {
            continue;
        }

        // 创建自定义项
        QListWidgetItem *item = new QListWidgetItem(m_pListWidget);
        item->setSizeHint(QSize(0, 40));

        // 创建自定义widget
        CAddressCompleterItem *completerItem = new CAddressCompleterItem(
            title.isEmpty() ? url : title,
            url,
            i.icon
            );
        if(completerItem) {
            completerItem->setToolTip(title + "\n" + url + "\n\n" + toolTip());
            m_pListWidget->setItemWidget(item, completerItem);
        }
        item->setData(Qt::UserRole, url);

        addedUrls << url;

        count++;
    }

    CAutoCompleteLineEdit* pEdit = qobject_cast<CAutoCompleteLineEdit*>(m_pLineEdit);
    if(pEdit)
        pEdit->setCompletions(addedUrls);

    // 如果没有找到历史记录，显示搜索建议
    if (m_pListWidget->count() == 0) {
        addSearchSuggestions(keyword);
    }

    // 如果有结果，显示下拉列表
    if (m_pListWidget->count() > 0) {
        // 选中第一项
        if(-1 == m_currentSelectedIndex)
            m_currentSelectedIndex = 0;

        m_pListWidget->setCurrentRow(m_currentSelectedIndex);
        // 确保选中项可见
        m_pListWidget->scrollToItem(m_pListWidget->item(m_currentSelectedIndex));

        showCompleter();
    } else {
        hideCompleter();
    }
}

void CAddressCompleter::addSearchSuggestions(const QString &keyword)
{
    qDebug(log) << Q_FUNC_INFO;
    if(!m_pListWidget)
        return;

    // 添加搜索建议
    QString searchText = tr("Search \"%1\"").arg(keyword);

    QListWidgetItem *pSearchItem = new QListWidgetItem(m_pListWidget);
    if(pSearchItem) {
        pSearchItem->setSizeHint(QSize(0, 40));
        pSearchItem->setData(Qt::UserRole, QString("@search:%1").arg(keyword));

        CAddressCompleterItem *pCompleterItem = new CAddressCompleterItem(
            searchText,
            tr("Use default search engine"),
            QIcon(":/icons/search.png")
            );
        if(pCompleterItem)
            m_pListWidget->setItemWidget(pSearchItem, pCompleterItem);
    }

    // 添加常用网站建议
    QStringList commonSites = {
        "https://www.bing.com/search?q=%1",
        "https://www.google.com/search?q=%1",
        "https://www.baidu.com/s?wd=%1",
        "https://github.com/search?q=%1"
    };

    for (const QString &site : commonSites) {
        QString url = site.arg(keyword);
        QString displayUrl = site.left(site.indexOf("?"));

        QListWidgetItem *pSiteItem = new QListWidgetItem(m_pListWidget);
        if(pSiteItem) {
            pSiteItem->setSizeHint(QSize(0, 40));
            pSiteItem->setData(Qt::UserRole, url);

            CAddressCompleterItem *pSiteCompleterItem = new CAddressCompleterItem(
                tr("Search in %1").arg(QUrl(displayUrl).host()),
                url,
                getIconForUrl(displayUrl)
                );
            if(pSiteCompleterItem)
                m_pListWidget->setItemWidget(pSiteItem, pSiteCompleterItem);
        }
    }
}

void CAddressCompleter::onItemClicked(QListWidgetItem *item)
{
    if (!item) return;

    QString url = item->data(Qt::UserRole).toString();

    // 处理搜索请求
    if (url.startsWith("@search:", Qt::CaseInsensitive)) {
        QString keyword = url.mid(8);
        //qDebug(log) << "emit searchRequested:" << keyword;
        emit searchRequested(keyword);
    } if(url.startsWith("@")) {
        emit sigCommand(url);
    }else {
        //qDebug(log) << "emit urlSelected:" << url;
        emit urlSelected(url);
    }

    hideCompleter();

    // 将URL填入地址栏
    if (m_pLineEdit) {
        m_pLineEdit->setText(url);
        m_pLineEdit->setFocus();
    }
}

void CAddressCompleter::moveToNextItem()
{
    if(!m_pListWidget) return;
    int count = m_pListWidget->count();
    if (count == 0) return;

    m_currentSelectedIndex = (m_currentSelectedIndex + 1) % count;
    m_pListWidget->setCurrentRow(m_currentSelectedIndex);

    // 确保选中项可见
    m_pListWidget->scrollToItem(m_pListWidget->item(m_currentSelectedIndex));
}

void CAddressCompleter::moveToPreviousItem()
{
    if(!m_pListWidget) return;
    int count = m_pListWidget->count();
    if (count == 0) return;

    m_currentSelectedIndex = (m_currentSelectedIndex - 1 + count) % count;
    m_pListWidget->setCurrentRow(m_currentSelectedIndex);

    // 确保选中项可见
    m_pListWidget->scrollToItem(m_pListWidget->item(m_currentSelectedIndex));
}

void CAddressCompleter::selectCurrentItem()
{
    //qDebug(log) << Q_FUNC_INFO;
    if(!m_pListWidget) return;
    QListWidgetItem *item = m_pListWidget->item(m_currentSelectedIndex);
    if (item) {
        onItemClicked(item);
    }
}

void CAddressCompleter::showCompleter()
{
    if(!m_pListWidget) return;
    if (m_isCompleterVisible || m_pListWidget->count() == 0) {
        return;
    }

    m_pLineEdit->setToolTip(m_szLineEditToolTipShow);
    updateCompleterPosition();

    // 动画显示
    QRect startRect = geometry();
    startRect.setHeight(0);

    QRect endRect = geometry();
    int itemHeight = 40;
    int visibleItems = qMin(m_pListWidget->count(), m_maxVisibleItems);
    int totalHeight = visibleItems * itemHeight + 10;

    endRect.setHeight(totalHeight);

    if(m_pShowAnimation) {
        m_pShowAnimation->setDuration(200);
        m_pShowAnimation->setStartValue(startRect);
        m_pShowAnimation->setEndValue(endRect);
        m_pShowAnimation->setEasingCurve(QEasingCurve::OutCubic);
        connect(m_pShowAnimation, &QPropertyAnimation::finished,
                this, &CAddressCompleter::show);
        m_pShowAnimation->start();
    } else {
        setGeometry(endRect);
        show();
    }
}

void CAddressCompleter::hideCompleter()
{
    qDebug(log) << Q_FUNC_INFO;
    if (!m_isCompleterVisible) {
        return;
    }

    qDebug(log) << Q_FUNC_INFO << "end";

    m_pLineEdit->setToolTip(m_szLineEditToolTip);

    // 动画隐藏
    QRect startRect = geometry();
    QRect endRect = geometry();
    endRect.setHeight(0);

    if(m_pHideAnimation) {
        m_pHideAnimation->setDuration(150);
        m_pHideAnimation->setStartValue(startRect);
        m_pHideAnimation->setEndValue(endRect);
        m_pHideAnimation->setEasingCurve(QEasingCurve::InCubic);

        connect(m_pHideAnimation, &QPropertyAnimation::finished,
                this, &CAddressCompleter::hide);

        m_pHideAnimation->start();
    } else
        hide();
}

void CAddressCompleter::updateCompleterPosition()
{
    if (!m_pLineEdit) return;

    // 获取输入框的全局位置
    QPoint globalPos = m_pLineEdit->mapToGlobal(QPoint(0, m_pLineEdit->height()));

    // 设置宽度与输入框相同
    int width = m_pLineEdit->width();

    // 检查是否超出屏幕
    QScreen *screen = QApplication::screenAt(globalPos);
    if (screen) {
        QRect screenRect = screen->availableGeometry();

        // 如果下拉框超出屏幕底部，显示在输入框上方
        int availableHeight = screenRect.bottom() - globalPos.y();
        int itemHeight = 40;
        int requiredHeight = qMin(m_pListWidget->count(), m_maxVisibleItems) * itemHeight + 10;

        if (availableHeight < requiredHeight) {
            globalPos = m_pLineEdit->mapToGlobal(QPoint(0, -requiredHeight));
        }

        // 确保不超出屏幕右侧
        if (globalPos.x() + width > screenRect.right()) {
            globalPos.setX(screenRect.right() - width);
        }
    }

    // 设置位置和大小
    setGeometry(globalPos.x(), globalPos.y(), width, 0);
}

QIcon CAddressCompleter::getIconForUrl(const QString &url)
{
    // TODO: 这里可以根据URL返回不同的图标
    // 简化实现：返回默认图标

    static QIcon defaultIcon;
    static QIcon httpIcon;
    static QIcon httpsIcon;
    static QIcon searchIcon = QIcon::fromTheme("system-search");

    if (url.startsWith("https://")) {
        return httpsIcon;
    } else if (url.startsWith("http://")) {
        return httpIcon;
    } else if (url.contains("@search", Qt::CaseInsensitive)) {
        return searchIcon;
    }

    return defaultIcon;
}
