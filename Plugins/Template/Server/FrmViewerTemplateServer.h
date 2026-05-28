#ifndef FRMVIEWERTEMPLATEBASE_H
#define FRMVIEWERTEMPLATEBASE_H

#include <QWidget>

namespace Ui {
class CFrmViewerTemplateServer;
}

class CFrmViewerTemplateServer : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmViewerTemplateServer(QWidget *parent = nullptr);
    ~CFrmViewerTemplateServer();

private:
    Ui::CFrmViewerTemplateServer *ui;
};

#endif // FRMVIEWERTEMPLATEBASE_H
