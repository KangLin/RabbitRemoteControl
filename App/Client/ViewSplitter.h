// Author: Kang Lin <kl222@126.com>

#ifndef VIEWSPLITTER_H
#define VIEWSPLITTER_H

#include <QSplitter>
#include "ViewSplitterContainer.h"
#include "View.h"

/*!
 * \brief The split view class
 * \ingroup ViewApi
 */
class CViewSplitter : public CView
{
    Q_OBJECT

public:
    CViewSplitter(CParameterApp* pPara, QWidget *parent = nullptr);
    virtual ~CViewSplitter();

    // CView interface
public:
    virtual int AddView(QWidget *pView) override;
    virtual int RemoveView(QWidget *pView) override;
    virtual QWidget *GetCurrentView() override;
    virtual int SetCurrentView(QWidget *pView) override;
    virtual void SetWidowsTitle(
        QWidget *pView, const QString &szTitle,
        const QIcon &icon, const QString &szToolTip) override;
    virtual int SetFullScreen(bool bFull) override;
    virtual int SetVisibleTab(bool bVisible) override;

private:
    int m_nRow;   // Row number
    int m_nCount; // Total number
    int m_nIdxRow, m_nIdxCol;  // Index

    QSplitter* m_pMain;
    QVector<QSplitter*> m_Row;

    QMap<QWidget*, CViewSplitterContainer*> m_Container;
    QString m_szStyleSheet;
    int m_HandleWidth;

private:
    int GetIndex(QWidget* pView, int &nRow, int &nCol);
    CViewSplitterContainer* GetContainer(const int &nRow, const int &nCol);
    CViewSplitterContainer* GetContainer(QWidget* pView);
    /*!
     * \chinese 突出当前视图
     * \param pView 需要突出的视图。如果为　nullptr　,则重置所有视图为普通
     * \english
     * \brief Highlight the current view
     * \param pView: A view that needs to be highlighted.
     *               If nullptr is used, all views are reset to normal
     */
    int ActiveContainer(QWidget* pView);
    int SetSizes();
};

#endif // VIEWSPLITTER_H
