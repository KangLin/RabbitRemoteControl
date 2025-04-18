// Author: Kang Lin <kl222@126.com>

#ifndef VIEWSPLITTER_H
#define VIEWSPLITTER_H

#include "View.h"
#include <QSplitter>
/*!
 * \brief The split view class
 * \ingroup ViewApi
 */
class CViewSplitter : public CView
{
    Q_OBJECT

public:
    CViewSplitter(QWidget *parent = nullptr);
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

private:
    int m_nRow;
    int m_nCount;
    int m_nIdxRow, m_nIdxCol;
    QSplitter* m_pMain;
    QVector<QSplitter*> m_Row;
    QString m_szStyleSheet;
    int m_HandleWidth;

private:
    int GetIndex(QWidget* pView, int &nRow, int &nCol);
    QWidget* GetView(const int &nRow, const int &nCol);
};

#endif // VIEWSPLITTER_H
