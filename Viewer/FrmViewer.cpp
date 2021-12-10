// Author: Kang Lin <kl222@126.com>

#include "FrmViewer.h"
#include "ui_FrmViewer.h"
#include <QPainter>
#include <QClipboard>
#include <QDebug>
#include <QResizeEvent>
#include <QCursor>
#include "Connect.h"
#include "RabbitCommonLog.h"

CFrmViewer::CFrmViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmViewer)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    SetAdaptWindows(ZoomToWindow);
    SetZoomFactor(1);
    
    setMouseTracking(true);
    setFocusPolicy(Qt::WheelFocus);
    setFocus();
}

CFrmViewer::~CFrmViewer()
{
    qDebug() << "CFrmViewer::~CFrmViewer()";
    delete ui;
}

void CFrmViewer::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    //qDebug() << "CFrmViewer::resizeEvent:" << event->size();
}

QRectF CFrmViewer::GetAspectRationRect()
{
    QRectF dstRect = rect();
    qreal newW = dstRect.width();
    qreal newH = dstRect.height();
    qreal newT = 0;
    qreal newL = 0;
    
    qreal rateW = static_cast<qreal>(rect().width())
            / static_cast<qreal>(m_Desktop.width());
    qreal rateH = static_cast<qreal>(rect().height())
            / static_cast<qreal>(m_Desktop.height());
    if(rateW < rateH)
    {
        newW = m_Desktop.width() * rateW;
        newH = m_Desktop.height() * rateW;
        newT = (static_cast<qreal>(rect().height()) - newH)
                / static_cast<qreal>(2);
    } else if(rateW > rateH) {
        newW = m_Desktop.width() * rateH;
        newH = m_Desktop.height() * rateH;
        newL = (static_cast<qreal>(rect().width()) - newW)
                / static_cast<qreal>(2);
    }
    dstRect = QRectF(newL, newT, newW, newH);
    return dstRect;
}

void CFrmViewer::paintDesktop()
{
    if(this->isHidden())
    {
        qDebug() << "CFrmViewer is hidden";
        return;
    }
    
    if(m_Desktop.isNull())
    {
        qDebug() << "m_Desktop.isNull";
        return;
    }
    
    QPainter painter(this);
    // 设置平滑模式
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    QRectF dstRect = rect();
    
    switch (m_AdaptWindows) {
    case Disable:
    case Auto:
    case ZoomToWindow:
    case Original:
        break;
    case OriginalCenter:
        dstRect.setLeft((rect().width() - m_Desktop.rect().width()) >> 1); 
        dstRect.setTop((rect().height() - m_Desktop.rect().height()) >> 1);
        dstRect.setWidth(m_Desktop.width());
        dstRect.setHeight(m_Desktop.height());
        break;
    case KeepAspectRationToWindow:
    {
        dstRect = GetAspectRationRect();
        break;
    }
    default:
        break;
    }
    painter.drawImage(dstRect, m_Desktop);
}

void CFrmViewer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    if(this->isHidden())
    {
        qDebug() << "CFrmViewer is hidden";
        return;
    }
    if(m_Desktop.isNull())
    {
        //TODO: Test
        QWidget::paintEvent(event);
        qDebug() << "m_Desktop.isNull";
        return;
    }
    
    paintDesktop();
}

int CFrmViewer::TranslationMousePoint(QPointF inPos, QPointF &outPos)
{
    //qDebug() << "TranslationPoint x:" << inPos.x() << ";y:" << inPos.y();

    switch (m_AdaptWindows) {
    case Auto:
    case Original:
    case OriginalCenter:
        outPos = inPos;
        break;
    case Zoom:
        outPos.setX(inPos.x() / GetZoomFactor());
        outPos.setY(inPos.y() / GetZoomFactor());
        break;
    case ZoomToWindow:
        outPos.setX(m_Desktop.width() * inPos.x() / width());
        outPos.setY(m_Desktop.height() * inPos.y() / height());
        break;
    case KeepAspectRationToWindow:
    {
        QRectF r = GetAspectRationRect();
        if(inPos.x() < r.left()
                || inPos.x() > r.right()
                || inPos.y() < r.top()
                || inPos.y() > r.bottom())
            return -1;
        outPos.setX(m_Desktop.width() * (inPos.x() - r.left()) / r.width());
        outPos.setY(m_Desktop.height() * (inPos.y() - r.top()) / r.height());
        break;
    }
    default:
        break;
    }

    return 0; 
}

void CFrmViewer::mousePressEvent(QMouseEvent *event)
{
    QMouseEvent e = *event;
    QPointF pos = e.pos();
    if(TranslationMousePoint(e.pos(), pos)) return;
    //qDebug() << "CFrmViewer::mousePressEvent" << event->button() << event->buttons();
    emit sigMousePressEvent(event->buttons(), QPoint(pos.x(), pos.y()));
    event->accept();
}

void CFrmViewer::mouseReleaseEvent(QMouseEvent *event)
{
    QMouseEvent e = *event;
    QPointF pos = e.pos();
    if(TranslationMousePoint(e.pos(), pos)) return;
    //qDebug() << "CFrmViewer::mouseReleaseEvent" << event->button() << event->buttons();
    emit sigMouseReleaseEvent(event->button(), QPoint(pos.x(), pos.y()));
    event->accept();
}

void CFrmViewer::mouseMoveEvent(QMouseEvent *event)
{
    QMouseEvent e = *event;
    QPointF pos = e.pos();
    if(TranslationMousePoint(e.pos(), pos)) return;
    emit sigMouseMoveEvent(event->buttons(), QPoint(pos.x(), pos.y()));
    emit sigMouseMoveEvent(event);
    event->accept();
}

void CFrmViewer::wheelEvent(QWheelEvent *event)
{
    QWheelEvent e = *event;
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QPointF pos = e.position();
    if(TranslationMousePoint(e.position(), pos)) return;
#else
    QPointF pos = e.pos();
    if(TranslationMousePoint(e.pos(), pos)) return;
#endif
    emit sigWheelEvent(event->buttons(), QPoint(pos.x(), pos.y()), event->angleDelta());
    event->accept();
}

void CFrmViewer::keyPressEvent(QKeyEvent *event)
{
    //LOG_MODEL_DEBUG("CFrmViewer", "keyPressEvent key:%d;modifiers:%d", event->key(), event->modifiers());
    emit sigKeyPressEvent(event->key(), event->modifiers());
    event->accept();
}

void CFrmViewer::keyReleaseEvent(QKeyEvent *event)
{
    //LOG_MODEL_DEBUG("CFrmViewer", "keyPressEvent key:%d;modifiers:%d", event->key(), event->modifiers());
    emit sigKeyReleaseEvent(event->key(), event->modifiers());
    event->accept();
}

void CFrmViewer::slotSystemCombination()
{
    // Send ctl+alt+del
    emit sigKeyPressEvent(Qt::Key_Control, Qt::NoModifier);
    emit sigKeyPressEvent(Qt::Key_Alt, Qt::NoModifier);
    emit sigKeyPressEvent(Qt::Key_Delete, Qt::NoModifier);
    emit sigKeyPressEvent(Qt::Key_Control, Qt::NoModifier);
    emit sigKeyPressEvent(Qt::Key_Alt, Qt::NoModifier);
    emit sigKeyPressEvent(Qt::Key_Delete, Qt::NoModifier);
}

double CFrmViewer::GetZoomFactor() const
{
    return m_dbZoomFactor;
}

int CFrmViewer::SetZoomFactor(double newZoomFactor)
{
    if(newZoomFactor < 0) return -1;
    if (qFuzzyCompare(m_dbZoomFactor, newZoomFactor))
        return 0;
    m_dbZoomFactor = newZoomFactor;
    return 0;
}

int CFrmViewer::ReSize(int width, int height)
{
    int w = width * GetZoomFactor();
    int h = height * GetZoomFactor();
    resize(w, h);
    return 0;
}

void CFrmViewer::SetAdaptWindows(ADAPT_WINDOWS aw)
{
    m_AdaptWindows = aw;
    if(!m_Desktop.isNull())
    {
        switch (m_AdaptWindows) {
        case Original:
        case OriginalCenter:
            SetZoomFactor(1);
        case Zoom:
            ReSize(m_Desktop.width(), m_Desktop.height());
            break;
        default:
            break;
        }
    }   
    update();
    //setFocus();
}

CFrmViewer::ADAPT_WINDOWS CFrmViewer::GetAdaptWindows()
{
    return m_AdaptWindows;
}

void CFrmViewer::slotSetDesktopSize(int width, int height)
{
    m_Desktop = QImage(width, height, QImage::Format_RGB32);
    
    if(Original == m_AdaptWindows
            || OriginalCenter == m_AdaptWindows
            || Zoom == m_AdaptWindows)
        ReSize(width, height);

    return;
}

void CFrmViewer::slotSetName(const QString& szName)
{
    this->setWindowTitle(szName);
    emit sigServerName(szName);
}

void CFrmViewer::slotUpdateRect(const QRect& r, const QImage& image)
{
    if(m_Desktop.isNull())
    {
        m_Desktop = image;
    } else if(r == m_Desktop.rect())
        m_Desktop = image;
    else
    {
        QPainter painter(&m_Desktop);
        painter.drawImage(r, image, r);
    }
    update();
}

void CFrmViewer::slotUpdateCursor(const QCursor& cursor)
{
    setCursor(cursor);
}

void CFrmViewer::slotUpdateCursorPosition(const QPoint& pos)
{
    cursor().setPos(pos);
}

int CFrmViewer::Load(QSettings &set)
{
    SetZoomFactor(set.value("Viewer/ZoomFactor", GetZoomFactor()).toDouble());
    SetAdaptWindows(static_cast<ADAPT_WINDOWS>(set.value("Viewer/AdaptType", GetAdaptWindows()).toInt()));
    return 0;
}

int CFrmViewer::Save(QSettings &set)
{
    set.setValue("Viewer/ZoomFactor", GetZoomFactor());
    set.setValue("Viewer/AdaptType", GetAdaptWindows());
    return 0;
}
