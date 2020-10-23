#include "FrmViewer.h"
#include "ui_FrmViewer.h"
#include <QPainter>
#include <QClipboard>
#include <QDebug>
#include <QResizeEvent>
#include "Connect.h"

CFrmViewer::CFrmViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmViewer),
    m_bClipboard(true)
{
    ui->setupUi(this);
    
    SetAdaptWindows(Original);
    
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

CFrmViewer::~CFrmViewer()
{
    delete ui;
}

void CFrmViewer::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    qDebug() << "CFrmViewer::resizeEvent:" << event->size();
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
    case Original:
        dstRect = m_Desktop.rect();
        this->setGeometry(m_Desktop.rect());
        break;
    case OriginalCenter:
        dstRect.setLeft((rect().width() - m_Desktop.rect().width()) >> 1); 
        dstRect.setTop((rect().height() - m_Desktop.rect().height()) >> 1);
        dstRect.setWidth(m_Desktop.width());
        dstRect.setHeight(m_Desktop.height());
        break;
    case AspectRation:
    {
        //m_Image = m_Image.scaled(rect().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation); 
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
        break;
    }
    case Zoom:    
        break;
    case Auto:
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
    
    if(rect().width() > m_DesktopSize.width()
            && rect().height() > m_DesktopSize.height())
        paintDesktop();
    else
    {
        QPainter painter(this);
        // 设置平滑模式
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.drawImage(rect(), m_Desktop);
    }
}

void CFrmViewer::mousePressEvent(QMouseEvent *event)
{
    emit sigMousePressEvent(event);
}

void CFrmViewer::mouseReleaseEvent(QMouseEvent *event)
{
    emit sigMouseReleaseEvent(event);
}

void CFrmViewer::mouseMoveEvent(QMouseEvent *event)
{
    emit sigMouseMoveEvent(event);
}

void CFrmViewer::wheelEvent(QWheelEvent *event)
{
    emit sigWheelEvent(event);
}

void CFrmViewer::keyPressEvent(QKeyEvent *event)
{
    emit sigKeyPressEvent(event);
}

void CFrmViewer::keyReleaseEvent(QKeyEvent *event)
{
    emit sigKeyReleaseEvent(event);
}

void CFrmViewer::SetAdaptWindows(ADAPT_WINDOWS aw)
{
    m_AdaptWindows = aw;
}

void CFrmViewer::SetClipboard(bool enable)
{
    m_bClipboard = enable;
}

void CFrmViewer::slotConnect()
{
}

void CFrmViewer::slotDisconnect()
{
    m_Desktop = QImage();
}

void CFrmViewer::slotSetDesktopSize(int width, int height)
{
    m_DesktopSize.setWidth(width);
    m_DesktopSize.setHeight(height);
    m_Desktop = QImage(m_DesktopSize, QImage::Format_RGB32);
    resize(width, height);
    return;
    
    int w = this->width();
    int h = this->height();
    if(this->width() < width)
        w = width;
    if(this->height() < height)
        h = height;
    if(this->width() != w || this->height() != h)
        this->resize(w, h);
}

void CFrmViewer::slotSetName(const QString& szName)
{
    this->setWindowTitle(szName);
    emit sigSetWindowName(szName);
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
        painter.drawImage(r, image);
    }
    update();
}

void CFrmViewer::slotServerCutText(const QString& text)
{
    QClipboard *board = QApplication::clipboard();  
    board->setText(text);  
}
