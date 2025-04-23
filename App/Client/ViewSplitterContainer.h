// Author: Kang Lin <kl222@126.com>

#ifndef VIEWSPLITTERCONTAINER_H
#define VIEWSPLITTERCONTAINER_H

#include <QWidget>
#include "TitleBar.h"
#include "ParameterApp.h"

/*!
 * \note only used by CViewSplitter
 */
class CViewSplitterContainer : public QFrame
{
    Q_OBJECT
public:
    explicit CViewSplitterContainer(QWidget* pView, CParameterApp* pPara);
    virtual ~CViewSplitterContainer();
    
    QWidget* GetView();
    void SetPrompt(const QString& szPrompt);
    int SetVisibleTab(bool bVisible);
    bool GetVisibleTab();

public Q_SLOTS:
    void slotTabPositionChanged();
    
Q_SIGNALS:
    void sigCloseView(const QWidget* pView);

private:
    RabbitCommon::CTitleBar* m_pTab;
    QWidget* m_pView;
    CParameterApp* m_pParameterApp;
    
    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event) override;
};

#endif // VIEWSPLITTERCONTAINER_H
