//! @author: Kang Lin (kl222@126.com)

#ifndef CVIEWTABLE_H
#define CVIEWTABLE_H

#include <QTabWidget>
#include <QScrollArea>
#include "View.h"

/**
 * @brief The CViewTable class
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
    virtual void SetWidowsTitle(QWidget* pView, const QString& szTitle) override;
    virtual int SetFullScreen(bool bFull) override;
    virtual void SetAdaptWindows(CFrmViewer::ADAPT_WINDOWS aw = CFrmViewer::Original, QWidget* pView = nullptr) override;

private Q_SLOTS:
    void slotCurrentChanged(int index);
    void slotTabCloseRequested(int index);

    virtual void slotSystemCombination();
    
private:
    QTabWidget* m_pTab;

    QWidget* GetViewer(int index);
    int GetViewIndex(QWidget* pView);
    
    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *event) override;
};

#endif // CVIEWTABLE_H
