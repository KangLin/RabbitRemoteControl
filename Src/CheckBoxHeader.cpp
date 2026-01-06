// Author: Kang Lin <kl222@126.com>

#include <QStandardItemModel>
#include <QPainter>
#include <QMouseEvent>
#include <QStyleOptionButton>
#include <QStyle>
#include <QApplication>
#include <QLoggingCategory>
#include "CheckBoxHeader.h"

static Q_LOGGING_CATEGORY(log, "CCheckBoxHeader")
CCheckBoxHeader::CCheckBoxHeader(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent)
{
    // 可根据需要启用鼠标追踪等
    setSectionsClickable(true);
}

void CCheckBoxHeader::SetCheckState(int index, Qt::CheckState state)
{
    QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(this->model());
    if(pModel) {
        QStandardItem* item = nullptr;
        if(orientation() == Qt::Horizontal)
            item = pModel->horizontalHeaderItem(index);
        else
            item = pModel->verticalHeaderItem(index);
        if(item && item->isCheckable()) {
            item->setCheckState(state);
            viewport()->update();
            return;
        }
    }

    if(!m_CheckableSections.contains(index)) {
        qCritical(log) << "Please call SetCheckable first to enable the checkbox";
        return;
    }
    if (m_CheckableSections[index] == state) return;
    m_CheckableSections[index] = state;
    viewport()->update();
}

Qt::CheckState CCheckBoxHeader::GetCheckState(int index) const
{
    QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(this->model());
    if(pModel) {
        QStandardItem* item = nullptr;
        if(orientation() == Qt::Horizontal)
            item = pModel->horizontalHeaderItem(index);
        else
            item = pModel->verticalHeaderItem(index);
        if(item && item->isCheckable()) {
            return item->checkState();
        }
    }

    auto it = m_CheckableSections.find(index);
    if(m_CheckableSections.end() == it)
        return Qt::Unchecked;
    return *it;
}

void CCheckBoxHeader::SetCheckable(int index, bool checkable, Qt::CheckState state)
{
    QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(this->model());
    if(pModel) {
        QStandardItem* item = nullptr;
        if(orientation() == Qt::Horizontal)
            item = pModel->horizontalHeaderItem(index);
        else
            item = pModel->verticalHeaderItem(index);
        if(item) {
            item->setCheckable(checkable);
            item->setCheckState(state);
            return;
        }
    }

    if (checkable)
        m_CheckableSections.insert(index, state);
    else
        m_CheckableSections.remove(index);
    viewport()->update();
}

bool CCheckBoxHeader::isCheckable(int index) const
{
    QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(this->model());
    if(pModel) {
        QStandardItem* item = nullptr;
        if(orientation() == Qt::Horizontal)
            item = pModel->horizontalHeaderItem(index);
        else
            item = pModel->verticalHeaderItem(index);
        return item && item->isCheckable();
    }

    return m_CheckableSections.contains(index);
}

QRect CCheckBoxHeader::CheckboxRect(const QRect &sectionRect) const
{
    int size = style()->pixelMetric(QStyle::PM_IndicatorWidth, nullptr, this);
    if (size <= 0) size = 16;
    //qDebug(log) << "Size:" << size;
    int x = 0;
    int y = 0;
    x = sectionRect.left() + 4;
    y = sectionRect.center().y() - size / 2;
    return QRect(x, y, size, size);
}

void CCheckBoxHeader::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    if (!isCheckable(logicalIndex) || isSectionHidden(logicalIndex)) {
        QHeaderView::paintSection(painter, rect, logicalIndex);
        return;
    }

    // 1) 绘制默认背景
    painter->save();
    QStyleOptionHeader option;
    option.initFrom(this);
    option.rect = rect;
    option.section = logicalIndex;
    option.orientation = this->orientation();
    style()->drawControl(QStyle::CE_Header, &option, painter, this);
    painter->restore();

    // 2) 如果该列可显示复选框，绘制复选框
    QRect cbRect = CheckboxRect(rect);
    DrawCheckBox(painter, cbRect, GetCheckState(logicalIndex), true);

    // 3) 绘制文本（在复选框右侧留出间距）
    QString text;
    if (model()) {
        QVariant v = model()->headerData(logicalIndex, Qt::Horizontal, Qt::DisplayRole);
        text = v.isValid() ? v.toString() : QString();
    }
    if (!text.isEmpty()) {
        // 调整文本区域（避开复选框）
        int textLeft = cbRect.right() + 4; // 复选框右边留4像素间距
        QRect textRect(textLeft, rect.top(), rect.width() - (textLeft - rect.left()), rect.height());
        // 绘制文本
        QVariant vAlign = model() ? model()->headerData(
                                        logicalIndex, orientation(),
                                        Qt::TextAlignmentRole)
                                  : Qt::AlignLeft;
        Qt::Alignment alignment = Qt::AlignCenter;
        if(vAlign.isValid())
            alignment = (Qt::Alignment)vAlign.toInt();
        this->DrawText(painter, textRect, text, alignment);
    }

    return;
}

void CCheckBoxHeader::DrawCheckBox(QPainter* painter, const QRect& rect,
                                   Qt::CheckState state, bool enabled) const
{
    QStyleOptionButton option;
    option.rect = rect;
    option.state = QStyle::State_Enabled | QStyle::State_Active;
    
    if (enabled) {
        option.state |= QStyle::State_Enabled;
    }
    
    switch (state) {
    case Qt::Checked:
        option.state |= QStyle::State_On;
        break;
    case Qt::PartiallyChecked:
        option.state |= QStyle::State_NoChange;
        break;
    case Qt::Unchecked:
        option.state |= QStyle::State_Off;
        break;
    }
    
    // 绘制复选框
    style()->drawControl(QStyle::CE_CheckBox, &option, painter, this);
}

void CCheckBoxHeader::DrawText(QPainter* painter, const QRect& rect,
                               const QString& text, Qt::Alignment alignment) const
{
    if (text.isEmpty()) return;

    painter->save();
    
    // 文本省略处理
    QFontMetrics fm = painter->fontMetrics();
    QString elided = fm.elidedText(text, Qt::ElideRight, rect.width());

    QPalette palette = this->palette();
    painter->setPen(palette.color(QPalette::Text));

    // 绘制文本
    painter->drawText(rect, alignment, elided);

    painter->restore();
}

void CCheckBoxHeader::mousePressEvent(QMouseEvent *event)
{
    int logical = logicalIndexAt(event->pos());
    if (logical >= 0 && isCheckable(logical)) {
        // 使用 sectionViewportPosition + sectionSize 构造节矩形，避免兼容性问题
        int x = sectionViewportPosition(logical);
        int w = sectionSize(logical);
        QRect sectionRect;
        sectionRect = QRect(x, 0, w, height());
        if (CheckboxRect(sectionRect).contains(event->pos())) {
            Qt::CheckState state = GetCheckState(logical);
            switch (state) {
            case Qt::Unchecked:
                state = Qt::PartiallyChecked;
                break;
            case Qt::PartiallyChecked:
                state = Qt::Checked;
                break;
            case Qt::Checked:
                state = Qt::Unchecked;
                break;
            }
            SetCheckState(logical, state);
            emit sigCheckStateChanged(logical, state);
            // 结束事件以避免触发排序（如不需要可调整）
            //event->accept();
            //return;
        }
    }

    QHeaderView::mousePressEvent(event);
}
