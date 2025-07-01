// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QPoint>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QMimeData>
#include <QMessageBox>
#if HAVE_QT6_MULTIMEDIA
#include <QMediaCaptureSession>
#endif
#if HAVE_QT6_RECORD
#include <QVideoFrameInput>
#include <QAudioBufferInput>
#include <QAudioBufferOutput>
#include <QMediaRecorder>
#include "ParameterRecord.h"
#endif

#include "FrmViewer.h"
#include "OperateDesktop.h"

class QRecordVideoEvent;

/*!
 * \~chinese
 * \brief 远程桌面接口。它由协议插件实现。
 *
 * \~english
 * \brief Remote desktop interface. It is implemented by the Protocol plugin.
 *
 * \~
 * \see COperateDesktop CFrmViewer
 * \ingroup PLUGIN_API
 */
class PLUGIN_EXPORT CBackendDesktop : public CBackend
{
    Q_OBJECT
public:
    explicit CBackendDesktop(COperateDesktop *pOperate = nullptr,
                             bool bDirectConnection = true);
    virtual ~CBackendDesktop();

public Q_SLOTS:
#if HAVE_QT6_RECORD
    virtual int Stop() override;
#endif
    /*!
     * \~chinese 当剪切板发生改变时调用
     * \~english Be called when the clip board change
     */
    virtual void slotClipBoardChanged() = 0;
    
private:
    int SetConnect(COperateDesktop* pOperate);
    int SetViewer(CFrmViewer* pView,
                  COperateDesktop* pOperate, bool bDirectConnection);
    
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
     * \~chinese 唤醒后台线程
     * \~english Wake up Backend thread(background thread)
     */
    virtual int WakeUp() override;
    // \~chinese 以下函数在后台线程中调用
    // \~english The following functions are called in the Backend thread(background thread)
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    
    // QObject interface
public:
    virtual bool event(QEvent *event) override;

#if HAVE_QT6_RECORD
    ///////// Record video /////////
private Q_SLOTS:
    // menu
    virtual void slotRecord(bool bRecord);
    virtual void slotRecordPause(bool bPause);
    // CFrmView
    void slotRecordVideo(const QImage& img);
Q_SIGNALS:
    void sigRecordVideo(bool bRecord, qreal nRate = 0);
private:
    void RecordVideo(QRecordVideoEvent *event);
    CParameterRecord* m_pParameterRecord;
    
protected:
    QVideoFrameInput m_VideoFrameInput;
    QAudioBufferInput m_AudioBufferInput;
    QAudioBufferOutput m_AudioBufferOutput;
    QMediaRecorder m_Recorder;
#endif
#if HAVE_QT6_MULTIMEDIA
    QMediaCaptureSession m_CaptureSession;
#endif
};
