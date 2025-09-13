// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QWidget>
#include <QWebEngineView>
#include <QToolBar>
#include <QLineEdit>
#include <QProgressBar>
#include <QTabWidget>

#include "FrmWebView.h"
#include "FrmDownloadManager.h"
#include "ParameterWebBrowser.h"

class CFrmWebBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmWebBrowser(CParameterWebBrowser* pPara, QWidget *parent = nullptr);
    virtual ~CFrmWebBrowser();

    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type);
    int InitMenu(QMenu* pMenu);
    int Start();

Q_SIGNALS:
    void sigInformation(const QString& szInfo);
    //! Title or icon changed
    void sigUpdateTitle();

private Q_SLOTS:
    void slotTabCurrentChanged(int index);
    void slotTabCloseRequested(int index);
    void slotViewCloseRequested();
    void slotReturnPressed();
    void slotInspector(bool checked);

private:
    void EnableAction(bool enable);
    void SetConnect(CFrmWebView* pWeb);
    [[nodiscard]] QWebEngineProfile* GetProfile();
    [[nodiscard]] CFrmWebView *CreateWebView();
    [[nodiscard]] QWidget* CreateTab(/*[in/out]*/CFrmWebView **view = nullptr);
    enum class ViewType{
        Web = 0,
        DevTools = 1
    };
    [[nodiscard]] CFrmWebView *CurrentView(ViewType index = ViewType::Web);
    [[nodiscard]] bool IsCurrentView(CFrmWebView *pView);
    [[nodiscard]] int IndexOfTab(CFrmWebView* pView);

private:
    CParameterWebBrowser* m_pPara;
    QToolBar* m_pToolBar;
    QAction* m_pBack;
    QAction* m_pForward;
    QAction* m_pRefresh;
    QAction* m_pStop;
    QAction* m_pFind;
    QAction* m_pFindNext;
    QAction* m_pFindPrevious;
    QAction* m_pZoomOriginal;
    QAction* m_pZoomIn;
    QAction* m_pZoomOut;
    QAction* m_pFavAction;
    QAction* m_pGo;
    QAction* m_pAddPage;
    QAction* m_pAddWindow;
    QAction* m_pDownload;
    QAction* m_pInspector;
    QLineEdit* m_pUrlLineEdit;
    QProgressBar* m_pProgressBar;
    QTabWidget* m_pTab;
    CFrmDownloadManager m_DownloadManager;
    QString m_szFindText;
};
