#ifndef FRMVIEWERTEMPLATEBASE_H
#define FRMVIEWERTEMPLATEBASE_H

#include <QWidget>

namespace Ui {
class CFrmViewerSftpServer;
}

class CFrmViewerSftpServer : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmViewerSftpServer(QWidget *parent = nullptr);
    ~CFrmViewerSftpServer();

private:
    Ui::CFrmViewerSftpServer *ui;
};

#endif // FRMVIEWERTEMPLATEBASE_H
