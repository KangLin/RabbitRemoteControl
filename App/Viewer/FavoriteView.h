#ifndef CFAVORITEVIEW_H
#define CFAVORITEVIEW_H

#include <QTreeView>

class CFavoriteView : public QTreeView
{
    Q_OBJECT
public:
    explicit CFavoriteView(QWidget *parent = nullptr);

    // QWidget interface
protected:
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;

//    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

private:
    QPoint m_DragStartPosition;
};

#endif // CFAVORITEVIEW_H
