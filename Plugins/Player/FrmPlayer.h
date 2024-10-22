#ifndef FRMPLAYER_H
#define FRMPLAYER_H

#include <QVideoWidget>
#include <QToolBar>
#include <QTimer>
#include <QTime>

class CFrmPlayer : public QVideoWidget
{
    Q_OBJECT
public:
    CFrmPlayer(QWidget *parent = nullptr);
    virtual ~CFrmPlayer();

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    virtual void enterEvent(QEnterEvent *event) override;
#else
    virtual  void enterEvent(QEvent *event) override;
#endif
    virtual void leaveEvent(QEvent *event) override;

private Q_SLOTS:
    void slotTimeout();

private:
    QToolBar m_ToolBar;
    QTimer m_Timer;
    int m_nInterval;
    QTime m_Start;

};

#endif // FRMPLAYER_H
