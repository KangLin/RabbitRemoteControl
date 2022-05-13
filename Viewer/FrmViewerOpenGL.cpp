// Author: Kang Lin <kl222@126.com>

#include "FrmViewerOpenGL.h"
#include <QDebug>
#include <QPainter>
#include "RabbitCommonLog.h"

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
    : QOpenGLWidget(parent)
{
    // store triangle vertex coordinate & texCoord
    m_VertexData = { -1.0,  1.0, 0.0, 0.0,
                      1.0,  1.0, 1.0, 0.0,
                      1.0, -1.0, 1.0, 1.0,
                     -1.0, -1.0, 0.0, 1.0 };
}

CFrmViewerOpenGL::~CFrmViewerOpenGL()
{
    LOG_MODEL_DEBUG("CFrmViewerOpenGL", "CFrmViewerOpenGL::~CFrmViewerOpenGL()");    
}

//void CFrmViewerOpenGL::setImage(QImage img)
//{
//    m_Desktop = img;
//    m_Texture = new QOpenGLTexture(m_Desktop);
//    update();
//}

void CFrmViewerOpenGL::initializeGL()
{
    qDebug() << "CFrmViewerOpenGL::initializeGL()";
    initializeOpenGLFunctions();
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // create the shader program
    m_ShaderProgram = new QOpenGLShaderProgram;
    bool success = m_ShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, g_VertexSource);
    if(!success) {
        qDebug() << "failed!";
    }
    success = m_ShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, g_FragmentSource);
    if(!success) {
        qDebug() << "failed!";
    }

    // bind location for the vertex shader
    m_ShaderProgram->bindAttributeLocation("position", 0);
    m_ShaderProgram->link();
    m_ShaderProgram->bind();

    // create the vertex array object
    m_VaoQuad.create();
    m_VaoQuad.bind();

    // create the vertex buffer object
    m_VboQuad.create();
    m_VboQuad.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_VboQuad.bind();
    m_VboQuad.allocate(m_VertexData.constData(), m_VertexData.count() * sizeof(GLfloat));

    // connect the inputs to the shader program
    m_ShaderProgram->enableAttributeArray(0);
    m_ShaderProgram->enableAttributeArray(1);
    m_ShaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 4*sizeof(GLfloat));
    m_ShaderProgram->setAttributeBuffer(1, GL_FLOAT, 2*sizeof(GLfloat), 2, 4*sizeof(GLfloat));

    QString errorLog = m_ShaderProgram->log();
    qDebug() << errorLog;
    
    m_Texture = new QOpenGLTexture(m_Desktop);
    
    m_VaoQuad.release();
    m_VboQuad.release();
    m_ShaderProgram->release();

    GLenum error = glGetError();
}

void CFrmViewerOpenGL::resizeGL(int width, int height)
{
    // only drawing the 2d surface so no need to modify any projections etc;
}

void CFrmViewerOpenGL::paintGL()
{
    qDebug() << "CFrmViewerOpenGL::paintGL()";
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(m_Desktop.isNull()) { return; }
    m_ShaderProgram->bind();
    m_VaoQuad.bind();
    if(m_Texture->isCreated()) m_Texture->bind();
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    m_VaoQuad.release();
    m_ShaderProgram->release();

    QString errorLog = m_ShaderProgram->log();
    qDebug() << errorLog;

    GLenum error = glGetError();
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
            break;
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
        m_Texture = new QOpenGLTexture(m_Desktop);
        update();
    }
}

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
