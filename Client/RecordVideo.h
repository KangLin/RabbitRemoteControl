#ifndef RECORDVIDEO_H
#define RECORDVIDEO_H

#include <QObject>
#include <QImage>
#include "client_export.h"

class CLIENT_EXPORT CRecordVideo : public QObject
{
    Q_OBJECT
public:
    explicit CRecordVideo(QObject *parent = nullptr);

    int Start(const QString& szFile);
    int Stop();

public Q_SLOTS:
    void slotUpdate(QImage img);

Q_SIGNALS:
    void sigUpdate(QImage img);
};

#endif // RECORDVIDEO_H
