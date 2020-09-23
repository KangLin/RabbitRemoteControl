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
};

#endif // FRMVIEWER_H
