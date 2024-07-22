// Author: Kang Lin <kl222@126.com>

#include "FramePixelBuffer.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "VNC")

CFramePixelBuffer::CFramePixelBuffer(int width, int height, QObject *parent)
    : QObject(parent),
      rfb::FullFramePixelBuffer(rfb::PixelFormat(32, 24, false, true,
                                                 255, 255, 255, 16, 8, 0),
                                0, 0, NULL, 0),
      m_FrameBuffer(width, height, QImage::Format_RGB32)
{
    setBuffer(width, height, m_FrameBuffer.bits(),
              m_FrameBuffer.bytesPerLine() / (getPF().bpp/8));
}

CFramePixelBuffer::~CFramePixelBuffer()
{
    qDebug(log) << "CFramePixelBuffer::~CFramePixelBuffer()";
}

const QImage& CFramePixelBuffer::getImage()
{
    return m_FrameBuffer;
}
