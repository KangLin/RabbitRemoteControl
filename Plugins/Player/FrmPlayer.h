// Author: Kang Lin <kl222@126.com>

#ifndef FRMPLAYER_H
#define FRMPLAYER_H

#include <QVideoWidget>
#include <QToolBar>
#include <QSlider>
#include <QProgressBar>
#include <QLabel>
#include "ParameterPlayer.h"

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
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;

private Q_SLOTS:
    void slotAudioMuted(bool bMuted);
    void slotAduioVolume(int volume);
    void slotStart(bool bStart);

private:
    QVideoWidget m_VideoWidget;
    QToolBar m_ToolBar;
    QSlider m_pbVideo;
    bool m_bMoveVideo;
    QSlider m_pbVolume;
    CParameterPlayer* m_pParameter;
    QLabel* m_pLabel;

    int AdjustCompone(const QSize &s);
};

#endif // FRMPLAYER_H
