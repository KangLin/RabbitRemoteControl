// Author: Kang Lin <kl222@126.com>

#include "FrmViewer.h"

#include <QPainter>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QCursor>
#include <QLoggingCategory>

#if defined(Q_OS_WIN)
    #include <Windows.h>
#elif ! (defined(Q_OS_ANDROID) || defined(Q_OS_WIN) || defined(Q_OS_APPLE) || defined(Q_OS_MACOS))
    #include <X11/Xlib.h>
    #include <X11/XKBlib.h>
    #define XK_MISCELLANY
    #include <X11/keysymdef.h>
#endif

#undef KeyPress

static Q_LOGGING_CATEGORY(log, "Client.FrmViewer")
static Q_LOGGING_CATEGORY(logKey, "Client.FrmViewer.Key")
static Q_LOGGING_CATEGORY(logMouse, "Client.FrmViewer.Mouse")

CFrmViewer::CFrmViewer(QWidget *parent)
    : QWidget(parent)
    , m_bRecordVideo(false)
{
    qDebug(log) << Q_FUNC_INFO;
    setAttribute(Qt::WA_DeleteOnClose);

    //qDebug(log) << "autoFillBackground:" << autoFillBackground();
    //setAutoFillBackground(true);
    //setAttribute(Qt::WA_OpaquePaintEvent);
    //setAttribute(Qt::WA_NoSystemBackground);

    slotSetAdaptWindows(ADAPT_WINDOWS::ZoomToWindow);
    slotSetZoomFactor(1);
    
    setMouseTracking(true);
    setFocusPolicy(Qt::WheelFocus);
    setFocus();

    // When the connecter is not connected, don't accept keyboard and mouse event
    // When the CConnecter::sigConnected() set true. accept keyboard and mouse event
    // \see CConnecter::sigConnected()
    setEnabled(false);

    bool check = connect(&m_TimerRecordVideo, SIGNAL(timeout()),
                         this, SLOT(slotRecordVideo()));
    Q_ASSERT(check);
}

CFrmViewer::~CFrmViewer()
{
    qDebug(log) << Q_FUNC_INFO;
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
    QRectF dstRect = rect();
    
    switch (m_AdaptWindows) {
    case ADAPT_WINDOWS::Disable:
    case ADAPT_WINDOWS::Auto:
    case ADAPT_WINDOWS::ZoomToWindow:
    case ADAPT_WINDOWS::Original:
        break;
    case ADAPT_WINDOWS::OriginalCenter:
        dstRect.setLeft((rect().width() - m_DesktopSize.width()) >> 1);
        dstRect.setTop((rect().height() - m_DesktopSize.height()) >> 1);
        dstRect.setWidth(m_DesktopSize.width());
        dstRect.setHeight(m_DesktopSize.height());
        break;
    case ADAPT_WINDOWS::KeepAspectRationToWindow:
    {
        dstRect = GetAspectRationRect();
        break;
    }
    default:
        break;
    }
    
    if(m_Desktop.isNull()) return;
    
    QPainter painter(this);
    // Clear background
    if(ADAPT_WINDOWS::KeepAspectRationToWindow == m_AdaptWindows)
    {
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
        painter.fillRect(rect(), QBrush(palette().color(QPalette::Window)));
#else
        painter.fillRect(rect(), QBrush(palette().color(QPalette::Background)));
#endif
    } //*/
    
    // 设置平滑模式
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawImage(dstRect, m_Desktop);
    
}

void CFrmViewer::paintEvent(QPaintEvent *event)
{
    //qqDebug(log) << "CFrmViewer::paintEvent";
    Q_UNUSED(event)

    paintDesktop();
}

int CFrmViewer::TranslationMousePoint(QPointF inPos, QPointF &outPos)
{
    qDebug(logMouse) << "TranslationPoint x:" << inPos.x() << ";y:" << inPos.y();

    switch (m_AdaptWindows) {
    case ADAPT_WINDOWS::Auto:
    case ADAPT_WINDOWS::Original:
    case ADAPT_WINDOWS::OriginalCenter:
        outPos = inPos;
        break;
    case ADAPT_WINDOWS::Zoom:
        outPos.setX(inPos.x() / GetZoomFactor());
        outPos.setY(inPos.y() / GetZoomFactor());
        break;
    case ADAPT_WINDOWS::ZoomToWindow:
        outPos.setX(m_DesktopSize.width() * inPos.x() / width());
        outPos.setY(m_DesktopSize.height() * inPos.y() / height());
        break;
    case ADAPT_WINDOWS::KeepAspectRationToWindow:
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
    QPointF pos =
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        event->position();
#else
        event->pos();
#endif

    if(TranslationMousePoint(pos, pos)) return;
    // event->buttons() 产生事件时，按键的状态
    // event->button() 触发当前事件的按键
    qDebug(logMouse) << "CFrmViewer::mousePressEvent"
                     << event << event->button() << event->buttons() << pos;
    emit sigMousePressEvent(event, QPoint(pos.x(), pos.y()));
    event->accept();

    if(testAttribute(Qt::WA_InputMethodEnabled)
        && event->button() == Qt::LeftButton)
        updateMicroFocus();
}

void CFrmViewer::mouseReleaseEvent(QMouseEvent *event)
{
    QPointF pos =
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        event->position();
#else
        event->pos();
#endif
    if(TranslationMousePoint(pos, pos)) return;
    // event->buttons() 产生事件时，按键的状态
    // event->button() 触发当前事件的按键
    qDebug(logMouse) << "CFrmViewer::mouseReleaseEvent"
                     << event << event->button() << event->buttons() << pos;
    emit sigMouseReleaseEvent(event, QPoint(pos.x(), pos.y()));
    event->accept();
}

void CFrmViewer::mouseMoveEvent(QMouseEvent *event)
{
    QPointF pos =
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        event->position();
#else
        event->pos();
#endif
    if(TranslationMousePoint(pos, pos)) return;
    // event->buttons() 产生事件时，按键的状态
    // event->button() 触发当前事件的按键
    qDebug(logMouse) << "CFrmViewer::mouseMoveEvent"
                     << event->button() << event->buttons() << pos;
    emit sigMouseMoveEvent(event, QPoint(pos.x(), pos.y()));
    emit sigMouseMoveEvent(event);
    event->accept();
}

void CFrmViewer::wheelEvent(QWheelEvent *event)
{
    QPointF pos =
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        event->position();
#else
        event->pos();
#endif
    if(TranslationMousePoint(pos, pos)) return;
    qDebug(logMouse) << "CFrmViewer::wheelEvent"
                     << event->buttons() << event->angleDelta() << pos;
    emit sigWheelEvent(event, QPoint(pos.x(), pos.y()));
    event->accept();
}

void CFrmViewer::keyPressEvent(QKeyEvent *event)
{
    qDebug(logKey) << "CFrmViewer::keyPressEvent" << event;
    emit sigKeyPressEvent(event);
    event->accept();
}

void CFrmViewer::keyReleaseEvent(QKeyEvent *event)
{
    qDebug(logKey) << "CFrmViewer::keyReleaseEvent" << event;
    emit sigKeyReleaseEvent(event);
    event->accept();
}

void CFrmViewer::inputMethodEvent(QInputMethodEvent *event)
{
    qDebug(logKey) << Q_FUNC_INFO << event;
    emit sigInputMethodEvent(event);
    event->accept();
    /*
    if(event->commitString().isEmpty()) return;
    QPoint p(cursor().pos().x() + fontMetrics().horizontalAdvance(event->commitString()), cursor().pos().y());
    qDebug(logKey) << cursor().pos() << fontMetrics().horizontalAdvance(event->commitString());
    slotUpdateCursorPosition(p);//*/
}

QVariant CFrmViewer::inputMethodQuery(Qt::InputMethodQuery query) const
{
    qDebug(logKey) << Q_FUNC_INFO << query;
    switch ( query )
    {
    case Qt::ImCursorRectangle: {
        QPoint p = mapFromGlobal(cursor().pos());
        QRect r(p.x(), p.y() , 1, fontMetrics().height());
        return r;
    }
    default:
        break;
    }
    QVariant r = QWidget::inputMethodQuery(query);
    qDebug(logKey) << Q_FUNC_INFO << query << r;
    return r;
}

QSize CFrmViewer::GetDesktopSize()
{
    return m_DesktopSize;
}

double CFrmViewer::GetZoomFactor() const
{
    return m_dbZoomFactor;
}

int CFrmViewer::slotSetZoomFactor(double newZoomFactor)
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

void CFrmViewer::slotSetAdaptWindows(ADAPT_WINDOWS aw)
{
    m_AdaptWindows = aw;
    if(!m_Desktop.isNull())
    {
        switch (m_AdaptWindows) {
        case ADAPT_WINDOWS::Original:
        case ADAPT_WINDOWS::OriginalCenter:
            slotSetZoomFactor(1);
        case ADAPT_WINDOWS::Zoom:
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
    
    if(ADAPT_WINDOWS::Original == m_AdaptWindows
        || ADAPT_WINDOWS::OriginalCenter == m_AdaptWindows
        || ADAPT_WINDOWS::Zoom == m_AdaptWindows)
        ReSize(width, height);

    return;
}

void CFrmViewer::slotRunning()
{
    setEnabled(true);
}

void CFrmViewer::slotEnableInputMethod(bool bEnable)
{
    setAttribute(Qt::WA_InputMethodEnabled, bEnable);
}

void CFrmViewer::slotSetName(const QString& szName)
{
    this->setWindowTitle(szName);
    emit sigServerName(szName);
}

void CFrmViewer::slotUpdateRect(const QImage& image)
{
    //qDebug(log) << "void CFrmViewer::slotUpdateRect(const QImage& image)" << image;
    m_Desktop = image;

    if(m_bRecordVideo)
        emit sigRecordVideo(m_Desktop);

    update();
    return;
}

void CFrmViewer::slotUpdateRect(const QRect& r, const QImage& image)
{
#if DEBUG
    if(r.width() != image.rect().width() || r.height() != image.rect().height())
    {
        qWarning(log) << "Image is error";
    }
#endif
    //qDebug(log) << "void CFrmViewer::slotUpdateRect(const QRect& r, const QImage& image)" << r << image;
    if(m_Desktop.isNull() || m_Desktop.rect() == r)
    {
        m_Desktop = image;
        //qDebug(log) << "Update image size is same old image size";
    } else {
        QPainter painter(&m_Desktop);
        painter.drawImage(r, image);
        //qDebug(log) << "Update image size isn't same old image size" << r << image.rect() << image;
    }

    if(m_bRecordVideo)
        emit sigRecordVideo(m_Desktop);

    update();
}

void CFrmViewer::slotUpdateCursor(const QCursor& cursor)
{
    //qDebug(log) << Q_FUNC_INFO << cursor;
    setCursor(cursor);
    if(testAttribute(Qt::WA_InputMethodEnabled))
        updateMicroFocus();
}

void CFrmViewer::slotUpdateCursorPosition(const QPoint& pos)
{
    //qDebug(log) << Q_FUNC_INFO << pos;
    cursor().setPos(pos);
    if(testAttribute(Qt::WA_InputMethodEnabled))
        updateMicroFocus();
}

#if ! (defined(Q_OS_WIN) || defined(Q_OS_APPLE) || defined(Q_OS_ANDROID) || defined(Q_OS_APPLE) || defined(Q_OS_MACOS))
unsigned int getModifierMask(unsigned int keysym)
{
    XkbDescPtr xkb;
    unsigned int mask, keycode;
    XkbAction *act;

    mask = 0;
    Display *dpy = XOpenDisplay(0);
    xkb = XkbGetMap(dpy, XkbAllComponentsMask, XkbUseCoreKbd);
    if (xkb == nullptr)
        return 0;

    for (keycode = xkb->min_key_code; keycode <= xkb->max_key_code; keycode++) {
        unsigned int state_out;
        KeySym ks;

        XkbTranslateKeyCode(xkb, keycode, 0, &state_out, &ks);
        if (ks == NoSymbol)
            continue;

        if (ks == keysym)
            break;
    }

    // KeySym not mapped?
    if (keycode > xkb->max_key_code)
        goto out;

    act = XkbKeyAction(xkb, keycode, 0);
    if (act == nullptr)
        goto out;
    if (act->type != XkbSA_LockMods)
        goto out;

    if (act->mods.flags & XkbSA_UseModMapMods)
        mask = xkb->map->modmap[keycode];
    else
        mask = act->mods.mask;

out:
    XkbFreeKeyboard(xkb, XkbAllComponentsMask, True);

    return mask;
}
#endif

void CFrmViewer::slotUpdateLedState(unsigned int state)
{    
    qDebug(log, "Got server LED state: 0x%08x", state);

    if (!hasFocus())
        return;

    //TODO: test led

#if defined(WIN32)
    INPUT input[6];
    UINT count;
    UINT ret;
    
    memset(input, 0, sizeof(input));
    count = 0;
    
    if (!!(state & CapsLock) != !!(GetKeyState(VK_CAPITAL) & 0x1)) {
        input[count].type = input[count+1].type = INPUT_KEYBOARD;
        input[count].ki.wVk = input[count+1].ki.wVk = VK_CAPITAL;
        //input[count].ki.wScan = input[count+1].ki.wScan = SCAN_FAKE;
        input[count].ki.dwFlags = 0;
        input[count+1].ki.dwFlags = KEYEVENTF_KEYUP;
        count += 2;
    }
    
    if (!!(state & NumLock) != !!(GetKeyState(VK_NUMLOCK) & 0x1)) {
        input[count].type = input[count+1].type = INPUT_KEYBOARD;
        input[count].ki.wVk = input[count+1].ki.wVk = VK_NUMLOCK;
        //input[count].ki.wScan = input[count+1].ki.wScan = SCAN_FAKE;
        input[count].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
        input[count+1].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_EXTENDEDKEY;
        count += 2;
    }
    
    if (!!(state & ScrollLock) != !!(GetKeyState(VK_SCROLL) & 0x1)) {
        input[count].type = input[count+1].type = INPUT_KEYBOARD;
        input[count].ki.wVk = input[count+1].ki.wVk = VK_SCROLL;
        //input[count].ki.wScan = input[count+1].ki.wScan = SCAN_FAKE;
        input[count].ki.dwFlags = 0;
        input[count+1].ki.dwFlags = KEYEVENTF_KEYUP;
        count += 2;
    }
    
    if (count == 0)
        return;
    
    ret = SendInput(count, input, sizeof(*input));
    if (ret < count)
        qCritical(log) << "Failed to update keyboard LED state:" << GetLastError();
#elif defined(Q_OS_APPLE)

    // No support for Scroll Lock //
    ;

#elif defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID) && !defined(Q_OS_APPLE)
    unsigned int affect, values;
    unsigned int mask;

    Bool ret;

    affect = values = 0;

    affect |= LockMask;
    if (state & CapsLock)
        values |= LockMask;

    mask = getModifierMask(XK_Num_Lock);
    affect |= mask;
    if (state & NumLock)
        values |= mask;

    mask = getModifierMask(XK_Scroll_Lock);
    affect |= mask;
    if (state & ScrollLock)
        values |= mask;
    Display *dpy = XOpenDisplay(0);
    ret = XkbLockModifiers(dpy, XkbUseCoreKbd, affect, values);
    if (!ret)
        qCritical(log) << tr("Failed to update keyboard LED state");
    XFlush(dpy);
    XCloseDisplay(dpy);
#endif
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

void CFrmViewer::slotRecordVideo(bool bRecord, qreal nRate)
{
    m_bRecordVideo = bRecord;
    if(0 == nRate) {
        qWarning(log) << "The video frame rate is 0. it is not record static remote desktop."
                      << "If need record static remote desktop,"
                      << "please 'Record->Video->Encoding mode' don't select 'Constant quality' in settings dialog,"
                      << "then set 'Record->Video->Video frame'";
        return;
    }

    m_bRecordVideo = false;
    if(bRecord) {
        qreal ms = 1000 / nRate;
        //qDebug(log) << "Rate:" << nRate << ms << "ms";
        m_TimerRecordVideo.start(ms);
    }
    else
        m_TimerRecordVideo.stop();
}

void CFrmViewer::slotRecordVideo()
{
    emit sigRecordVideo(m_Desktop);
}

void CFrmViewer::focusInEvent(QFocusEvent *event)
{
    qDebug(log) << Q_FUNC_INFO << event << this;
    emit sigViewerFocusIn(this);
}

void CFrmViewer::focusOutEvent(QFocusEvent *event)
{
    qDebug(log) << Q_FUNC_INFO << event << this;
}
