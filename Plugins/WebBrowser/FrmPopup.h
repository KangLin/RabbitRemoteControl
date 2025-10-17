// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QLineEdit>
#include <QAction>
#include <QWidget>
#include <QWebEnginePage>
class CFrmWebView;
class CFrmWebBrowser;
class CFrmPopup : public QWidget
{
    Q_OBJECT
public:
    explicit CFrmPopup(QWebEngineProfile *profile, CFrmWebBrowser* pWebBrowser, QWidget *parent = nullptr);

    CFrmWebView* GetView();
private Q_SLOTS:
    void slotHandleGeometryChangeRequested(const QRect &newGeometry);
private:
    QLineEdit* m_pleUrl;
    QAction* m_pFavAction;
    CFrmWebView* m_pView;
};
