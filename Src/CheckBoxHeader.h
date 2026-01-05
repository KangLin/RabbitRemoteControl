// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QHeaderView>
#include <QSet>

/*!
 * \~chinese 包含复选框的表头
 * \details
 *   - 如果模型是 QStandardItemModel ，则会根据 QStandardItem::isCheckable() 来判断是否显示复选框
 *   - 如果模型不是 QStandardItemModel ，则使用 SetCheckState() 来决定是否显示复选框
 *
 * \~english Check box header view
 * \details
 *   - If the model is a QStandardItemModel, whether to show a checkbox is determined by QStandardItem::isCheckable()
 *   - If the model is not a QStandardItemModel, SetCheckState() is used to decide whether to show a checkbox
 */
class CCheckBoxHeader : public QHeaderView {
    Q_OBJECT

public:
    explicit CCheckBoxHeader(Qt::Orientation orientation, QWidget *parent = nullptr);

    /*!
     *  \~chinese 设置是否显示复选框
     *  \note 在调用前必须先设置了模型
     */
    void SetCheckable(int index, bool checkable, Qt::CheckState state = Qt::Unchecked);
    bool isCheckable(int index) const;
    /*!
     *  \~chinese 设置表头复选框状态
     *  \note 在调用前必须先设置了模型
     */
    void SetCheckState(int index, Qt::CheckState state);
    Qt::CheckState GetCheckState(int index) const;

signals:
    void sigCheckStateChanged(int index, Qt::CheckState state);

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QRect CheckboxRect(const QRect &sectionRect) const;
    void DrawText(QPainter* painter, const QRect& rect, 
                  const QString& text, Qt::Alignment alignment) const;
    void DrawCheckBox(QPainter* painter, const QRect& rect,
                      Qt::CheckState state, bool enabled) const;

    QHash<int, Qt::CheckState> m_CheckableSections;
};
