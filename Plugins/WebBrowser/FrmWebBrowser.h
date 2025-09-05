#ifndef FRMWEBBROWSER_H
#define FRMWEBBROWSER_H

#include <QWidget>
#include <QWebEngineView>
#include <QToolBar>
#include <QLineEdit>
#include <QProgressBar>
#include <QTabWidget>
#include "FrmWebView.h"

class CFrmWebBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit CFrmWebBrowser(QWidget *parent = nullptr);
    virtual ~CFrmWebBrowser();

    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type);
signals:

private:
    void SetConnect(CFrmWebView* pWeb);

private:
    QToolBar* m_pToolBar;
    QTabWidget* m_pTab;
    QAction* m_pBack;
    QAction* m_pForward;
    QAction* m_pRefresh;
    QLineEdit* m_pUrlLineEdit;
    QAction* m_pFavAction;
    QProgressBar* m_pProgressBar;
};

#endif // FRMWEBBROWSER_H
