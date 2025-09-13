#ifndef FRMDOWNLOADMANAGER_H
#define FRMDOWNLOADMANAGER_H

#include <QWidget>
#include <QVBoxLayout>
#include "FrmDownload.h"

class CFrmDownloadManager : public QWidget
{
    Q_OBJECT
public:
    explicit CFrmDownloadManager(QWidget *parent = nullptr);
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
};

#endif // FRMDOWNLOADMANAGER_H
