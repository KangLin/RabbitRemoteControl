// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include "FrmDownload.h"
#include <ParameterWebBrowser.h>

class CFrmDownloadManager : public QWidget
{
    Q_OBJECT
public:
    explicit CFrmDownloadManager(CParameterWebBrowser* para, QWidget *parent = nullptr);
    virtual ~CFrmDownloadManager();
    
public Q_SLOTS:
    void slotDownloadRequested(QWebEngineDownloadRequest *download);

Q_SIGNALS:
    void sigVisible(bool visible);
protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
private Q_SLOTS:
    void Add(CFrmDownload* item);
    void Remove(CFrmDownload* item);
    void slotSelected(CFrmDownload* item);
private:
    int m_nCount;
    QVBoxLayout* m_pItems;
    CParameterWebBrowser* m_pPara;
};
