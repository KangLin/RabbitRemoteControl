// Author: Kang Lin <kl222@126.com>

#ifndef CVIEWTABLE_H
#define CVIEWTABLE_H

#include <QTabWidget>
#include <QScrollArea>
#include "View.h"

class MainWindow;
/**
 * @brief The CViewTable class
 * @ingroup ViewApi
 */
class CViewTable : public CView
{
    Q_OBJECT
    
public:
    explicit CViewTable(QWidget *parent = nullptr);
    virtual ~CViewTable();
    
    // CView interface
    virtual int AddView(QWidget *pView) override;
    virtual int RemoveView(QWidget *pView) override;
    virtual QWidget *GetCurrentView() override;
    virtual void SetWidowsTitle(QWidget* pView, const QString& szTitle, const QIcon& icon, const QString& szToolTip) override;
    virtual int SetFullScreen(bool bFull) override;
    virtual void SetAdaptWindows(CFrmViewer::ADAPT_WINDOWS aw = CFrmViewer::Auto, QWidget* pView = nullptr) override;

    int ShowTabBar(bool bShow);
    virtual int Screenslot(const QString& szFile, bool bRemoteDesktop = true) override;
    virtual double GetZoomFactor() override;
    virtual QSize GetDesktopSize() override;
    
private Q_SLOTS:
    void slotCurrentChanged(int index);
    void slotTabCloseRequested(int index);
    
    void slotTabPositionChanged();

    virtual void slotSystemCombination() override;

    virtual void slotZoomIn() override;
    virtual void slotZoomOut() override;
    virtual void slotZoomFactor(double v) override;
    
private:
    QTabWidget* m_pTab;
    MainWindow* m_pMainWindow;

    QWidget* GetViewer(int index);
    int GetViewIndex(QWidget* pView);
    
    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *event) override;
};

#endif // CVIEWTABLE_H
