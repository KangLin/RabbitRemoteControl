// Author: Kang Lin <kl222@126.com>

#ifndef CVIEWTABLE_H
#define CVIEWTABLE_H

#include <QTabWidget>
#include <QScrollArea>
#include "View.h"

class MainWindow;
/*!
 * \brief The CViewTable class
 * \ingroup ViewApi
 */
class CViewTable : public CView
{
    Q_OBJECT
    
public:
    explicit CViewTable(CParameterApp* pPara, QWidget *parent = nullptr);
    virtual ~CViewTable();
    
    // CView interface
    virtual int AddView(QWidget *pView) override;
    virtual int RemoveView(QWidget *pView) override;
    virtual QWidget *GetCurrentView() override;
    virtual int SetCurrentView(QWidget* pView) override;
    virtual void SetWidowsTitle(QWidget* pView, const QString& szTitle, const QIcon& icon, const QString& szToolTip) override;
    virtual int SetFullScreen(bool bFull) override;
    virtual int SetVisibleTab(bool bVisible) override;

private Q_SLOTS:
    void slotCurrentChanged(int index);
    void slotTabCloseRequested(int index);
    
    void slotTabPositionChanged();

private:
    QTabWidget* m_pTab;
    QString m_szStyleSheet;

    QWidget* GetViewer(int index);
    int GetViewIndex(QWidget* pView);
    
    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *event) override;

};

#endif // CVIEWTABLE_H
