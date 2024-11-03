// Author: Kang Lin <kl222@126.com>
  
#ifndef __CCONNECTDESKTOP_H_2024_09_27__
#define __CCONNECTDESKTOP_H_2024_09_27__

#pragma once

#include <QPoint>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QMimeData>
#include <QMessageBox>

#if HAVE_QT6_MULTIMEDIA
    #include <QMediaCaptureSession>
    #include <QMediaRecorder>
#endif
#if HAVE_QT6_RECORD
    #include <QVideoFrameInput>
    #include <QAudioBufferInput>    
#endif
#include "ParameterRecord.h"

#include "FrmViewer.h"
#include "Connect.h"

class QRecordVideoEvent;

/*!
 * \~chinese
 * \brief 远程桌面连接接口。它由协议插件实现。
 *
 * \~english
 * \brief Remote desktop connect interface. It is implemented by the Protocol plugin.
 *
 * \~
 * \see CConnecterThread CFrmViewer
 * \ingroup CLIENT_PLUGIN_API
 */
class CLIENT_EXPORT CConnectDesktop : public CConnect
{
    Q_OBJECT

public:
    /*!
     * \~chinese
     * \param pConnecter
     * \param bDirectConnection:
     *        \li true: 当阻塞事件循环时，可能会造成延迟，所以直接连接信号
     *        \li false: 当一个非阻塞事件循环时
     *
     * \~english
     * \param pConnecter
     * \param bDirectConnection:
     *        \li true: when connect is non-Qt event and it can block event loop
     *        \li false: The connect is Qt event and it isn't block event loop
     */
    explicit CConnectDesktop(CConnecter* pConnecter,
                             bool bDirectConnection = true);
    virtual ~CConnectDesktop() override;

public Q_SLOTS:
    virtual int Disconnect() override;
    /*!
     * \~chinese 当剪切板发生改变时调用
     * \~english Be called when the clip board change
     */
    virtual void slotClipBoardChanged() = 0;

private:
    int SetConnecter(CConnecter* pConnecter);
    int SetViewer(CFrmViewer* pView, bool bDirectConnection);

Q_SIGNALS:
    void sigSetDesktopSize(int width, int height);
    void sigServerName(const QString& szName);
    
    /*!
     * \~chinese 通知视图，图像更新
     * \param r: 更新图像的矩形
     * \param image: 包含更新矩形的图像
     *
     * \~english Notify the CFrmView update image
     * \param r: update rectangle
     * \param image: An image that contains an update rectangle
     */
    void sigUpdateRect(const QRect& r, const QImage& image);
    /*!
     * \brief Notify the CFrmView update image
     * \param image
     */
    void sigUpdateRect(const QImage& image);
    void sigUpdateCursor(const QCursor& cursor);
    void sigUpdateCursorPosition(const QPoint& pos);
    void sigUpdateLedState(unsigned int state);
    void sigSetClipboard(QMimeData* data);

public Q_SLOTS:
    // \~chinese 以下函数在 CFrmView 线程（主线程）中调用
    // \~english The following functions are called in the CFrmView thread(main thread)
    virtual void slotMousePressEvent(QMouseEvent* event, QPoint pos);
    virtual void slotMouseReleaseEvent(QMouseEvent* event, QPoint pos);
    virtual void slotMouseMoveEvent(QMouseEvent* event, QPoint pos);
    virtual void slotWheelEvent(QWheelEvent* event, QPoint pos);
    virtual void slotKeyPressEvent(QKeyEvent *event);
    virtual void slotKeyReleaseEvent(QKeyEvent *event);

protected:
    /*!
     * \~chinese 唤醒连接线程（后台线程）
     * \~english Wake up Connect thread(background thread)
     */
    virtual int WakeUp();
    // \~chinese 以下函数在 Connect 线程（后台线程）中调用
    // \~english The following functions are called in the Connect thread(background thread)
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    
    // QObject interface
public:
    virtual bool event(QEvent *event) override;

    ///////// Record video /////////
private Q_SLOTS:
    // connect menu
    virtual void slotRecord(bool bRecord);
    virtual void slotRecordPause(bool bPause);
    // connect CFrmView
    void slotRecordVideo(const QImage& img);
Q_SIGNALS:
    void sigRecordVideo(bool bRecord);
private:
    void RecordVideo(QRecordVideoEvent *event);
    CParameterRecord* m_pParameterRecord;

protected:
#if HAVE_QT6_MULTIMEDIA
    QMediaCaptureSession m_CaptureSession;
    QMediaRecorder m_Recorder;
#endif
#if HAVE_QT6_RECORD
    QVideoFrameInput m_VideoFrameInput;
    QAudioBufferInput m_AudioBufferInput;
#endif
};

#endif // __CCONNECTDESKTOP_H_2024_09_27__
