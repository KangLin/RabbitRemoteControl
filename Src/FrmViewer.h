#ifndef FRMVIEWER_H
#define FRMVIEWER_H

#include <QWidget>

namespace Ui {
class CFrmViewer;
}

class CFrmViewer : public QWidget
{
    Q_OBJECT
    
public:
    explicit CFrmViewer(QWidget *parent = nullptr);
    ~CFrmViewer();
    
private:
    Ui::CFrmViewer *ui;
    
    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

#endif // FRMVIEWER_H
