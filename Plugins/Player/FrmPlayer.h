// Author: Kang Lin <kl222@126.com>

#ifndef FRMPLAYER_H
#define FRMPLAYER_H

#include <QVideoWidget>
#include <QToolBar>
#include <QSlider>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QMargins>
#include "ParameterPlayer.h"

#if !defined(Q_OS_LINUX) || defined(Q_OS_ANDROID)
    // 在　wayland 下，QVideoWidget 会覆盖顶层窗口（例如：全屏工具条）
    #define WITH_QVideoWidget
#endif
#ifndef WITH_QVideoWidget
    class QGraphicsVideoItem;
    class CVideoGraphicsView;
#endif

class CFrmPlayer : public QWidget
{
    Q_OBJECT

public:
    CFrmPlayer(QWidget *parent = nullptr);
    virtual ~CFrmPlayer();

    QVideoSink* videoSink();
    int SetParameter(CParameterPlayer* pParameter);

    QAction* m_paStart;
    QAction* m_paPause;
#if HAVE_QT6_RECORD
    QAction* m_paRecord;
    QAction* m_paRecordPause;
#endif
    QAction* m_paScreenShot;
    QAction* m_paMuted;
    QAction* m_paVolume;
    QAction* m_paSettings;

public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

public Q_SLOTS:
    void slotPositionChanged(qint64 pos, qint64 duration);

Q_SIGNALS:
    void sigChangePosition(qint64 pos);
    /*!
     * \~chinese 视图获得焦点
     * \~english The view is focus
     * \param pView
     */
    void sigViewerFocusIn(QWidget* pView);

protected:
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

    //! Full Screen
    Q_INVOKABLE int OnFullScreen(bool bFull);

private Q_SLOTS:
    void slotAudioMuted(bool bMuted);
    void slotAduioVolume(int volume);
    void slotStart(bool bStart);
    void slotTimeOut();
    void StartTimer();

private:
#ifdef WITH_QVideoWidget
    QVideoWidget* m_pVideoWidget;
#else
    CVideoGraphicsView *m_pGraphicsView;
    QGraphicsVideoItem* m_pVideoItem;
    void UpdateGraphicsVideoGeometry();
#endif
    QToolBar* m_pToolBar;
    QTimer tm_ToolBar;
    QSlider m_pbVideo;
    bool m_bMoveVideo;
    QSlider m_pbVolume;
    CParameterPlayer* m_pParameter;
    QLabel* m_pLabel;
    bool m_bFullScreen;
    QMargins m_Margins;
};

#endif // FRMPLAYER_H
