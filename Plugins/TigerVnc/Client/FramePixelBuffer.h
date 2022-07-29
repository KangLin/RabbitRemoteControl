// Author: Kang Lin <kl222@126.com>

#ifndef CFRAMEPIXELBUFFER_H
#define CFRAMEPIXELBUFFER_H

#include <rfb/PixelBuffer.h>
#include <QImage>
#include <QObject>

class CFramePixelBuffer : public QObject,
        public rfb::FullFramePixelBuffer
{
    Q_OBJECT
    
public:
    explicit CFramePixelBuffer(int width, int height, QObject *parent = nullptr);
    virtual ~CFramePixelBuffer();
    
    const QImage& getImage();
    
private:
    QImage m_FrameBuffer;
};

#endif // CFRAMEPIXELBUFFER_H
