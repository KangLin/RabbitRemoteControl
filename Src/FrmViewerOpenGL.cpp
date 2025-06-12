// Author: Kang Lin <kl222@126.com>

#include "FrmViewer.h"
#include <QDebug>
#include <QPainter>
#include <QResizeEvent>
#include <QCursor>

class COpenGLInit
{
public:
    COpenGLInit()
    {
        QSurfaceFormat fmt;
        fmt.setDepthBufferSize(24);
        fmt.setVersion(3, 3);
        fmt.setProfile(QSurfaceFormat::CoreProfile);
        QSurfaceFormat::setDefaultFormat(fmt);
    }
};

COpenGLInit g_OpenGLInit;

const char* g_VertexSource =
        "#version 330\n"
        "layout(location = 0) in vec2 position;\n"
        "layout(location = 1) in vec2 texCoord;\n"
        "out vec4 texc;\n"
        "void main( void )\n"
        "{\n"
        " gl_Position = vec4(position, 0.0, 1.0);\n"
        " texc = vec4(texCoord, 0.0, 1.0);\n"
        "}\n";

const char* g_FragmentSource =
        "#version 330\n"
        "uniform sampler2D texture;\n"
        "in vec4 texc;\n"
        "out vec4 fragColor;\n"
        "void main( void )\n"
        "{\n"
        " fragColor = texture2D(texture, texc.st);\n"
        "}\n";

CFrmViewerOpenGL::CFrmViewerOpenGL(QWidget *parent)
    : QOpenGLWidget(parent), m_pTexture(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose);

    SetAdaptWindows(ZoomToWindow);
    SetZoomFactor(1);
    
    setMouseTracking(true);
    setFocusPolicy(Qt::WheelFocus);
    setFocus();

    // store triangle vertex coordinate & texCoord
    m_VertexData = { -1.0,  1.0, 0.0, 0.0,
                      1.0,  1.0, 1.0, 0.0,
                      1.0, -1.0, 1.0, 1.0,
                     -1.0, -1.0, 0.0, 1.0 };
}

CFrmViewerOpenGL::~CFrmViewerOpenGL()
{
    qDebug() << "CFrmViewerOpenGL::~CFrmViewerOpenGL()";
    if(m_pTexture)
    {
        delete m_pTexture;
        m_pTexture = nullptr;
    }
}

void CFrmViewerOpenGL::InitVertor()
{
    m_ShaderProgram.bind();

    // create the vertex array object
    if(!m_VaoQuad.isCreated())
        m_VaoQuad.create();
    m_VaoQuad.bind();

    // create the vertex buffer object
    if(!m_VboQuad.isCreated())
    {
        m_VboQuad.create();
        m_VboQuad.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }
    m_VboQuad.bind();
    m_VboQuad.allocate(m_VertexData.constData(),
                       m_VertexData.count() * sizeof(GLfloat));

    // connect the inputs to the shader program
    m_ShaderProgram.enableAttributeArray(0);
    m_ShaderProgram.enableAttributeArray(1);
    m_ShaderProgram.setAttributeBuffer(0, GL_FLOAT, 0, 2, 4 * sizeof(GLfloat));
    m_ShaderProgram.setAttributeBuffer(
                1, GL_FLOAT, 2 * sizeof(GLfloat), 2, 4 * sizeof(GLfloat));

    QString errorLog = m_ShaderProgram.log();
    //qDebug() << errorLog;

    m_VboQuad.release();
    m_VaoQuad.release();
    m_ShaderProgram.release();

    GLenum error = glGetError();
    Q_UNUSED(error);
}

void CFrmViewerOpenGL::initializeGL()
{
    qDebug() << "CFrmViewerOpenGL::initializeGL()";
    initializeOpenGLFunctions();
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // create the shader program
    bool success = m_ShaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, g_VertexSource);
    if(!success) {
        qDebug() << "failed!";
    }
    success = m_ShaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, g_FragmentSource);
    if(!success) {
        qDebug() << "failed!";
    }

    // bind location for the vertex shader
    m_ShaderProgram.bindAttributeLocation("position", 0);
    m_ShaderProgram.link();
    InitVertor();
}

void CFrmViewerOpenGL::resizeGL(int width, int height)
{
    // only drawing the 2d surface so no need to modify any projections etc;
    qDebug() << "CFrmViewerOpenGL::resizeGL:" << width << height;
}

void CFrmViewerOpenGL::paintGL()
{
    //qDebug() << "CFrmViewerOpenGL::paintGL()";
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(m_Desktop.isNull() || !m_pTexture || !m_pTexture->isCreated()) return;
    
    m_ShaderProgram.bind();
    m_VaoQuad.bind();
    m_pTexture->bind();
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    m_pTexture->release();
    m_VaoQuad.release();
    m_ShaderProgram.release();

    QString errorLog = m_ShaderProgram.log();
    Q_UNUSED(errorLog);
    //qDebug() << errorLog;

    GLenum error = glGetError();
    Q_UNUSED(error);
}

double CFrmViewerOpenGL::GetZoomFactor() const
{
    return m_dbZoomFactor;
}

int CFrmViewerOpenGL::SetZoomFactor(double newZoomFactor)
{
    if(newZoomFactor < 0) return -1;
    if (qFuzzyCompare(m_dbZoomFactor, newZoomFactor))
        return 0;
    m_dbZoomFactor = newZoomFactor;
    return 0;
}

QSize CFrmViewerOpenGL::GetDesktopSize()
{
    return m_Desktop.size();
}

int CFrmViewerOpenGL::ReSize(int width, int height)
{
    int w = width * GetZoomFactor();
    int h = height * GetZoomFactor();
    resize(w, h);
    return 0;
}

void CFrmViewerOpenGL::SetAdaptWindows(ADAPT_WINDOWS aw)
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
            m_VertexData = { -1.0,  1.0, 0.0, 0.0,
                              1.0,  1.0, 1.0, 0.0,
                              1.0, -1.0, 1.0, 1.0,
                             -1.0, -1.0, 0.0, 1.0 };
            InitVertor();
            break;
        case ZoomToWindow:
        {
            m_VertexData = { -1.0,  1.0, 0.0, 0.0,
                              1.0,  1.0, 1.0, 0.0,
                              1.0, -1.0, 1.0, 1.0,
                             -1.0, -1.0, 0.0, 1.0 };
            InitVertor();
            break;
        }
        case KeepAspectRationToWindow:
        {
            QRectF r = GetAspectRationRect();
            m_VertexData = { -((float)r.width()/(float)width()), (float)r.height()/(float)height(), 0.0, 0.0,
                              (float)r.width()/(float)width(), (float)r.height()/(float)height(), 1.0, 0.0,
                              (float)r.width()/(float)width(), -((float)r.height()/(float)height()), 1.0, 1.0,
                             -((float)r.width()/(float)width()), -((float)r.height()/(float)height()), 0.0, 1.0 };
            InitVertor();
            break;
        }
        default:
            break;
        }
    }   
    update();
    //setFocus();
}

CFrmViewerOpenGL::ADAPT_WINDOWS CFrmViewerOpenGL::GetAdaptWindows()
{
    return m_AdaptWindows;
}

int CFrmViewerOpenGL::Load(QSettings &set)
{
    SetZoomFactor(set.value("Viewer/ZoomFactor", GetZoomFactor()).toDouble());
    SetAdaptWindows(static_cast<ADAPT_WINDOWS>(set.value("Viewer/AdaptType", GetAdaptWindows()).toInt()));
    return 0;
}

int CFrmViewerOpenGL::Save(QSettings &set)
{
    set.setValue("Viewer/ZoomFactor", GetZoomFactor());
    set.setValue("Viewer/AdaptType", GetAdaptWindows());
    return 0;
}

QImage CFrmViewerOpenGL::GrabImage(int x, int y, int w, int h)
{
    int width = w, height = h;
    if(-1 == w)
        width = m_Desktop.width();
    if(-1 == w)
        height = m_Desktop.height();
    return m_Desktop.copy(x, y, width, height);
}

void CFrmViewerOpenGL::slotSetDesktopSize(int width, int height)
{
    m_Desktop = QImage(width, height, QImage::Format_RGB32);
    
    if(Original == m_AdaptWindows
            || OriginalCenter == m_AdaptWindows
            || Zoom == m_AdaptWindows)
        ReSize(width, height);

    return;
}

void CFrmViewerOpenGL::slotSetName(const QString& szName)
{
    this->setWindowTitle(szName);
}

void CFrmViewerOpenGL::slotUpdateRect(const QRect& r, const QImage& image)
{
    if(m_Desktop.isNull() || m_Desktop.rect() == r)
    {
        m_Desktop = image;
        if(m_pTexture)
        {
            delete m_pTexture;
            m_pTexture = nullptr;
        }
        m_pTexture = new QOpenGLTexture(m_Desktop);
        update();
    }
}

void CFrmViewerOpenGL::slotUpdateRect(QSharedPointer<CImage> image)
{}

void CFrmViewerOpenGL::slotUpdateCursor(const QCursor& cursor)
{
    setCursor(cursor);
}

void CFrmViewerOpenGL::slotUpdateCursorPosition(const QPoint& pos)
{
    cursor().setPos(pos);
}

void CFrmViewerOpenGL::slotSystemCombination()
{
    // Send ctl+alt+del
    emit sigKeyPressEvent(Qt::Key_Control, Qt::NoModifier);
    emit sigKeyPressEvent(Qt::Key_Alt, Qt::NoModifier);
    emit sigKeyPressEvent(Qt::Key_Delete, Qt::NoModifier);
    emit sigKeyPressEvent(Qt::Key_Control, Qt::NoModifier);
    emit sigKeyPressEvent(Qt::Key_Alt, Qt::NoModifier);
    emit sigKeyPressEvent(Qt::Key_Delete, Qt::NoModifier);
}

QRectF CFrmViewerOpenGL::GetAspectRationRect()
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

int CFrmViewerOpenGL::TranslationMousePoint(QPointF inPos, QPointF &outPos)
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

void CFrmViewerOpenGL::mousePressEvent(QMouseEvent *event)
{
    QPointF pos = event->pos();
    if(TranslationMousePoint(event->pos(), pos)) return;
    //qDebug() << "CFrmViewer::mousePressEvent" << event->button() << event->buttons();
    emit sigMousePressEvent(event->buttons(), QPoint(pos.x(), pos.y()));
    event->accept();
}

void CFrmViewerOpenGL::mouseReleaseEvent(QMouseEvent *event)
{
    QPointF pos = event->pos();
    if(TranslationMousePoint(event->pos(), pos)) return;
    //qDebug() << "CFrmViewer::mouseReleaseEvent" << event->button() << event->buttons();
    emit sigMouseReleaseEvent(event->button(), QPoint(pos.x(), pos.y()));
    event->accept();
}

void CFrmViewerOpenGL::mouseMoveEvent(QMouseEvent *event)
{
    QPointF pos = event->pos();
    if(TranslationMousePoint(event->pos(), pos)) return;
    emit sigMouseMoveEvent(event->buttons(), QPoint(pos.x(), pos.y()));
    emit sigMouseMoveEvent(event);
    event->accept();
}

void CFrmViewerOpenGL::wheelEvent(QWheelEvent *event)
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

void CFrmViewerOpenGL::keyPressEvent(QKeyEvent *event)
{
    //qDebug("keyPressEvent key:%d;modifiers:%d", event->key(), event->modifiers());
    emit sigKeyPressEvent(event->key(), event->modifiers());
    event->accept();
}

void CFrmViewerOpenGL::keyReleaseEvent(QKeyEvent *event)
{
    //qDebug("keyPressEvent key:%d;modifiers:%d", event->key(), event->modifiers());
    emit sigKeyReleaseEvent(event->key(), event->modifiers());
    event->accept();
}
