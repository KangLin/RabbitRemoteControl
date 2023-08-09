// Author: Kang Lin <kl222@126.com>

#include "FrmViewer.h"

#include <QPainter>
#include <QClipboard>
#include <QDebug>
#include <QResizeEvent>
#include <QCursor>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Client)

CFrmViewer::CFrmViewer(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    //setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    SetAdaptWindows(ZoomToWindow);
    SetZoomFactor(1);
    
    setMouseTracking(true);
    setFocusPolicy(Qt::WheelFocus);
    setFocus();
}

CFrmViewer::~CFrmViewer()
{
    qDebug(Client) << "CFrmViewer::~CFrmViewer()";
}

QRectF CFrmViewer::GetAspectRationRect()
{
    QRectF dstRect = rect();
    qreal newW = dstRect.width();
    qreal newH = dstRect.height();
    qreal newT = 0;
    qreal newL = 0;
    
    qreal rateW = static_cast<qreal>(rect().width())
            / static_cast<qreal>(m_DesktopSize.width());
    qreal rateH = static_cast<qreal>(rect().height())
            / static_cast<qreal>(m_DesktopSize.height());
    if(rateW < rateH)
    {
        newW = m_DesktopSize.width() * rateW;
        newH = m_DesktopSize.height() * rateW;
        newT = (static_cast<qreal>(rect().height()) - newH)
                / static_cast<qreal>(2);
    } else if(rateW > rateH) {
        newW = m_DesktopSize.width() * rateH;
        newH = m_DesktopSize.height() * rateH;
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
        qDebug(Client) << "CFrmViewer is hidden";
        return;
    }

    QRectF dstRect = rect();
    
    switch (m_AdaptWindows) {
    case Disable:
    case Auto:
    case ZoomToWindow:
    case Original:
        break;
    case OriginalCenter:
        dstRect.setLeft((rect().width() - m_DesktopSize.width()) >> 1);
        dstRect.setTop((rect().height() - m_DesktopSize.height()) >> 1);
        dstRect.setWidth(m_DesktopSize.width());
        dstRect.setHeight(m_DesktopSize.height());
        break;
    case KeepAspectRationToWindow:
    {
        dstRect = GetAspectRationRect();
        break;
    }
    default:
        break;
    }

    if(!m_Desktop.isNull())
    {
        QPainter painter(this);
        // Clear background
        if(KeepAspectRationToWindow == m_AdaptWindows)
        {
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
            painter.fillRect(rect(), QBrush(palette().color(QPalette::Window)));
#else
            painter.fillRect(rect(), QBrush(palette().color(QPalette::Background)));
#endif
        }

        // 设置平滑模式
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.drawImage(dstRect, m_Desktop);
    }
}

void CFrmViewer::paintEvent(QPaintEvent *event)
{
    //qqDebug(Client) << "CFrmViewer::paintEvent";
    Q_UNUSED(event)
    if(this->isHidden())
    {
        qDebug() << "CFrmViewer is hidden";
        return;
    }
    
    paintDesktop();
}

int CFrmViewer::TranslationMousePoint(QPointF inPos, QPointF &outPos)
{
    //qDebug(Client) << "TranslationPoint x:" << inPos.x() << ";y:" << inPos.y();

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
        outPos.setX(m_DesktopSize.width() * inPos.x() / width());
        outPos.setY(m_DesktopSize.height() * inPos.y() / height());
        break;
    case KeepAspectRationToWindow:
    {
        QRectF r = GetAspectRationRect();
        if(inPos.x() < r.left()
                || inPos.x() > r.right()
                || inPos.y() < r.top()
                || inPos.y() > r.bottom())
            return -1;
        outPos.setX(m_DesktopSize.width() * (inPos.x() - r.left()) / r.width());
        outPos.setY(m_DesktopSize.height() * (inPos.y() - r.top()) / r.height());
        break;
    }
    default:
        break;
    }

    return 0; 
}

void CFrmViewer::mousePressEvent(QMouseEvent *event)
{
    QPointF pos = event->pos();
    if(TranslationMousePoint(event->pos(), pos)) return;
    //qDebug(Client) << "CFrmViewer::mousePressEvent" << event->button() << event->buttons();
    emit sigMousePressEvent(event->buttons(), QPoint(pos.x(), pos.y()));
    event->accept();
}

void CFrmViewer::mouseReleaseEvent(QMouseEvent *event)
{
    QPointF pos = event->pos();
    if(TranslationMousePoint(event->pos(), pos)) return;
    //qDebug(Client) << "CFrmViewer::mouseReleaseEvent" << event->button() << event->buttons();
    emit sigMouseReleaseEvent(event->button(), QPoint(pos.x(), pos.y()));
    event->accept();
}

void CFrmViewer::mouseMoveEvent(QMouseEvent *event)
{
    QPointF pos = event->pos();
    if(TranslationMousePoint(event->pos(), pos)) return;
    emit sigMouseMoveEvent(event->buttons(), QPoint(pos.x(), pos.y()));
    emit sigMouseMoveEvent(event);
    event->accept();
}

void CFrmViewer::wheelEvent(QWheelEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QPointF pos = event->position();
    if(TranslationMousePoint(event->position(), pos)) return;
#else
    QPointF pos = event->pos();
    if(TranslationMousePoint(event->pos(), pos)) return;
#endif
    emit sigWheelEvent(event->buttons(), QPoint(pos.x(), pos.y()), event->angleDelta());
    event->accept();
}

void CFrmViewer::keyPressEvent(QKeyEvent *event)
{
    //qDebug(Client) << "CFrmViewer::keyPressEvent" << event;
    emit sigKeyPressEvent(event->key(), event->modifiers());
    event->accept();
}

void CFrmViewer::keyReleaseEvent(QKeyEvent *event)
{
    //qDebug(Client) << "CFrmViewer::keyReleaseEvent" << event;
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

QSize CFrmViewer::GetDesktopSize()
{
    return m_DesktopSize;
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
            ReSize(m_DesktopSize.width(), m_DesktopSize.height());
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
    m_DesktopSize = QSize(width, height);
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
    //qDebug(Client) << "void CFrmViewer::slotUpdateRect(const QRect& r, const QImage& image)";
    if(m_Desktop.isNull() || m_Desktop.rect() == r)
    {
        m_Desktop = image;
        //qDebug(Client) << "Update image size is same old image size";
    }
    else
    {
        QPainter painter(&m_Desktop);
        painter.drawImage(r, image);
        //qDebug(Client) << "Update image size isn't same old image size";
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

QImage CFrmViewer::GrabImage(int x, int y, int w, int h)
{
    int width = w, height = h;
    if(-1 == w)
        width = m_DesktopSize.width();
    if(-1 == w)
        height = m_DesktopSize.height();
    return m_Desktop.copy(x, y, width, height);
}
