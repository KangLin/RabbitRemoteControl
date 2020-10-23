#ifndef CFRAMEPIXELBUFFER_H
#define CFRAMEPIXELBUFFER_H

#include <rfb/PixelBuffer.h>
#include <QImage>

class CFramePixelBuffer : public QObject,
        public rfb::FullFramePixelBuffer
{
    Q_OBJECT
    
public:
    explicit CFramePixelBuffer(int width, int height, QObject *parent = nullptr);
    
    const QImage& getImage();
    
private:
    QImage m_FrameBuffer;
};

#endif // CFRAMEPIXELBUFFER_H
