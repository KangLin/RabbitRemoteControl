// Author: Kang Lin <kl222@126.com>

#include "ViewSplitter.h"
#include <QVBoxLayout>
#include <QLoggingCategory>
#include <QEvent>
#include "ViewSplitterContainer.h"

static Q_LOGGING_CATEGORY(log, "App.View.Splitter")

CViewSplitter::CViewSplitter(CParameterApp *pPara, QWidget *parent)
    : CView(pPara, parent)
    , m_nRow(0)
    , m_nCount(0)
    , m_nIdxRow(-1)
    , m_nIdxCol(-1)
{
    qDebug(log) << Q_FUNC_INFO << this;

    QVBoxLayout* p = new QVBoxLayout(this);
    if(!p) {
        qCritical(log) << "new QVBoxLayout fail";
        return;
    }
    p->setContentsMargins(0, 0, 0, 0);
    setLayout(p);
    m_pMain = new QSplitter(Qt::Vertical, this);
    if(!m_pMain) {
        qCritical(log) << "m_pMain is nullptr";
        return;
    }
    p->addWidget(m_pMain);
    m_HandleWidth = m_pMain->handleWidth();
    /*
    setStyleSheet("QWidget { background-color: red }");
    m_pMain->setStyleSheet("QSplitter::handle { background-color: white }");//*/
}

CViewSplitter::~CViewSplitter()
{
    qDebug(log) << Q_FUNC_INFO << this;
    for(int i = 0; i < m_nRow; i++)
    {
        auto sp = m_Row[i];
        for(int j = 0; j < sp->count(); j++)
        {
            auto pw = sp->widget(j);
            pw->setParent(nullptr);
        }
    }
}

int CViewSplitter::AddView(QWidget *pView)
{
    int nRet = 0;
    QSplitter* sp = nullptr;
    if(!pView) {
        qCritical(log) << "The view is null";
        Q_ASSERT(pView);
        return -1;
    }
    
    //TODO: 增加检查 pView 是否已存在?

    // 是否需要新增加一行
    if(m_nCount + 1 > m_nRow * m_nRow) {
        QSplitter* p = new QSplitter(Qt::Horizontal, m_pMain);
        if(p) {
            //p->setStyleSheet("QSplitter::handle { background-color: blue }");
            m_Row.append(p);
            m_nRow++;
        }
    }
    // 顺序查找哪行没满，加到未尾
    int i = 0;
    for(i = 0; i < m_nRow; i++) {
        auto p = m_Row[i];
        if(p->count() < m_nRow) {
            sp = p;
            break;
        }
    }
    if(!sp)
        return nRet;

    auto pContainer = new CViewSplitterContainer(pView, m_pParameterApp);
    if(pContainer) {
        // 设置当前视频索引
        m_nIdxRow = i;
        m_nIdxCol = sp->count();
        sp->addWidget(pContainer);
        pContainer->show();
        m_Container.insert(pView, pContainer);
        if(m_pParameterApp)
        {
            bool check = connect(m_pParameterApp, SIGNAL(sigTabPositionChanged()),
                            pContainer, SLOT(slotTabPositionChanged()));
            Q_ASSERT(check);
            check = connect(pContainer, SIGNAL(sigCloseView(const QWidget*)),
                            this, SIGNAL(sigCloseView(const QWidget*)));
            Q_ASSERT(check);
        }
        m_nCount++;
    } else
        return -1;

    //sp->setStretchFactor(sp->count() - 1, 1);

    qDebug(log) << Q_FUNC_INFO << "Row:" << m_nRow << "Count:" << m_nCount
                << "Current row:" << m_nIdxRow << "Current col:" << m_nIdxCol
                << "Current count:" << sp->count();
    emit sigCurrentChanged(pView);
    return nRet;
}

int CViewSplitter::RemoveView(QWidget *pView)
{
    int nRet = 0;
    if(!pView)
        return -1;

    // Get view position
    int nRow = -1;
    int nCol = -1;
    nRet = GetIndex(pView, nRow, nCol);
    if(nRet)
        return nRet;

    CViewSplitterContainer* pContainerNext = nullptr;
    QWidget* pCurView = GetCurrentView();   
    if(pView == pCurView)
    {
        // Get next
        if(m_Row[nRow]->count() > nCol + 1)
            pContainerNext = GetContainer(nRow, nCol + 1);
        else if(m_Row[nRow]->count() - 1 == nCol && 0 != nCol)
            pContainerNext = GetContainer(nRow, nCol -1);
        else if(0 == nCol && nRow + 1 < m_nRow) {
            for(int i = nRow + 1; i < m_nRow; i++) {
                if(m_Row[i]->count() > 0) {
                    pContainerNext = GetContainer(nRow + 1, 0);
                    break;
                }
            }
        }
        if(!pContainerNext) {
            for(int i = nRow - 1; i >= 0; i--) {
                if(m_Row[i]->count() > 0) {
                    pContainerNext = GetContainer(i, m_Row[i]->count() - 1);
                    break;
                }
            }
        }
    }

    // Delete view
    pView->setParent(nullptr);
    auto pContainer = GetContainer(pView);
    if(pContainer) {
        m_Container.remove(pView);
        delete pContainer;
    }
    qDebug(log) << "Row:" << nRow << "remain:" << m_Row[nRow]->count();
    m_nCount--;

    qDebug(log) << "Row:" << m_nRow << "Count:" << m_nCount
                << "Current row:" << m_nIdxRow << "Current col:" << m_nIdxCol;
    
    if(pContainerNext)
        pCurView = pContainerNext->GetView();
    
    if(pCurView)
        pCurView->setFocus();

    return SetCurrentView(pCurView);
}

QWidget *CViewSplitter::GetCurrentView()
{
    auto p = GetContainer(m_nIdxRow, m_nIdxCol);
    if(p)
        return p->GetView();
    return nullptr;
}

int CViewSplitter::SetCurrentView(QWidget *pView)
{
    int nRet = 0;
    if(!pView)
        return -1;
    int nRow = -1;
    int nCol = -1;
    nRet = GetIndex(pView, nRow, nCol);
    if(nRet) return nRet;

    if(m_nIdxCol == nCol && m_nIdxRow == nRow)
        return 0;

    m_nIdxRow = nRow;
    m_nIdxCol = nCol;
    pView->setFocus();
    emit sigCurrentChanged(pView);

    return nRet;
}

void CViewSplitter::SetWidowsTitle(
    QWidget *pView, const QString &szTitle,
    const QIcon &icon, const QString &szToolTip)
{
    if(!pView)
        return;
    auto pContainer = GetContainer(pView);
    if(!pContainer)
        return;
    pContainer->setWindowTitle(szTitle);
    if(m_pParameterApp->GetEnableTabIcon())
        pContainer->setWindowIcon(icon);
    else
        pContainer->setWindowIcon(QIcon());
    if(m_pParameterApp->GetEnableTabToolTip())
        pContainer->SetPrompt(szToolTip);
    else
        pContainer->SetPrompt(QString());
}

int CViewSplitter::SetFullScreen(bool bFull)
{
    int nRet = 0;
    if(0 >= m_nCount) return 0;
    QWidget* p = GetCurrentView();
    qDebug(log) << "CurrentView:" << p;
    if(!p) return 0;
    if(bFull) {
        m_szStyleSheet = styleSheet();
        //qDebug(log) << "Style:" << m_szStyleSheet;
        setStyleSheet("QWidget::pane{top:0px;left:0px;border:none;}");
        m_HandleWidth = m_pMain->handleWidth();
        m_pMain->setHandleWidth(0);
        for(int i = 0; i < m_nRow; i++) {
            auto sp = m_Row[i];
            if(!sp) {
                qCritical(log) << "Row" << i << "is null";
                continue;
            }
            sp->setHandleWidth(0);
            for(int j = 0; j < m_nRow; j++) {
                if(m_nIdxRow == i && m_nIdxCol == j)
                    continue;
                auto p = sp->widget(j);
                if(p) {
                    p->hide();
                } else {
                    qCritical(log) << "Widget: Row" << i << "Col" << j << "is null";
                }
            }
        }
    }
    else {
        setStyleSheet(m_szStyleSheet);
        m_pMain->setHandleWidth(m_HandleWidth);
        for(int i = 0; i < m_nRow; i++) {
            auto sp = m_Row[i];
            sp->setHandleWidth(m_HandleWidth);
            if(!sp) continue;
            for(int j = 0; j < m_nRow; j++) {
                auto p = sp->widget(j);
                if(p)
                    p->show();
            }
        }
    }
    return nRet;
}

int CViewSplitter::GetIndex(QWidget* pView, int &nRow, int &nCol)
{
    if(!pView)
        return -1;

    nCol = -1;
    
    auto pContainer = m_Container[pView];
    for(nRow = 0; nRow < m_nRow; nRow++)
    {
        auto sp = m_Row[nRow];
        if(!sp) continue;
        nCol = sp->indexOf(pContainer);
        if(-1 < nCol)
            return 0;
    }

    return -1;
}

CViewSplitterContainer* CViewSplitter::GetContainer(const int &nRow, const int &nCol)
{
    QWidget* pContainer = nullptr;
    QSplitter* sp = nullptr;
    if(-1 < nRow && nRow < m_nRow)
        sp = m_Row[nRow];
    if(sp && -1 < nCol && nCol < sp->count())
        pContainer = sp->widget(nCol);
    qDebug(log) << "Container" << "Row number:" << m_nRow << m_Row.size()
                << "Count:" << m_nCount
                << "row:" << m_nIdxRow << "col:" << m_nIdxCol;
    return qobject_cast<CViewSplitterContainer*>(pContainer);
}

CViewSplitterContainer* CViewSplitter::GetContainer(QWidget *pView)
{
    auto it = m_Container.find(pView);
    if(m_Container.end() == it)
        return nullptr;
    return *it;
}
