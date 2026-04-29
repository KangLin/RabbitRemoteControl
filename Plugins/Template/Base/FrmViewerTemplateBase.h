#ifndef FRMVIEWERTEMPLATEBASE_H
#define FRMVIEWERTEMPLATEBASE_H

#include <QWidget>

namespace Ui {
class CFrmViewerTemplateBase;
}

class CFrmViewerTemplateBase : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmViewerTemplateBase(QWidget *parent = nullptr);
    ~CFrmViewerTemplateBase();

private:
    Ui::CFrmViewerTemplateBase *ui;
};

#endif // FRMVIEWERTEMPLATEBASE_H
