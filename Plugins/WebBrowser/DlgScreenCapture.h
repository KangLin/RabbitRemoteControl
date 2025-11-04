#ifndef DLGSCREENCAPTURE_H
#define DLGSCREENCAPTURE_H

#include <QDialog>
#include <QWebEngineDesktopMediaRequest>

namespace Ui {
class CDlgScreenCapture;
}

class CDlgScreenCapture : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgScreenCapture(const QWebEngineDesktopMediaRequest request, QWidget *parent = nullptr);
    ~CDlgScreenCapture();
    enum class Type {
        Screen,
        Window
    };
    Q_ENUM(Type)
    int GetIndex(Type &type, int &id);
private slots:
    void on_cbScreen_currentIndexChanged(int index);
    void on_cbWindow_currentIndexChanged(int index);

private:
    Ui::CDlgScreenCapture *ui;
};

#endif // DLGSCREENCAPTURE_H
