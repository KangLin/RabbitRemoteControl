#ifndef CPARAMETERAPP_H
#define CPARAMETERAPP_H

#include <QObject>
#include <QTabWidget>

class CParameterApp : public QObject
{
    Q_OBJECT

public:
    explicit CParameterApp(QObject *parent = nullptr);
    
    virtual int Load();
    virtual int Save();
    
public:
    bool GetScreenShot() const;
    void SetScreenShot(bool newScreenShot);
Q_SIGNALS:
    void sigScreenShotChanged();    
private:
    bool m_bScreenShot;
    Q_PROPERTY(bool ScreenShot READ GetScreenShot WRITE SetScreenShot NOTIFY sigScreenShotChanged)

public:
    enum ScreenShotEndAction {
        NoAction,
        OpenFolder,
        OpenFile
    };
    ScreenShotEndAction GetScreenShotEndAction() const;
    void SetScreenShotEndAction(ScreenShotEndAction newScreenShotEndAction);    
Q_SIGNALS:
    void sigScreenShotEndActionChanged();
private:
    ScreenShotEndAction m_ScreenShotEndAction;
    Q_PROPERTY(ScreenShotEndAction ScreenShotEndAction READ GetScreenShotEndAction WRITE SetScreenShotEndAction NOTIFY sigScreenShotEndActionChanged)
    
public:    
    bool GetReceiveShortCut() const;
    void SetReceiveShortCut(bool newReceiveShortCut);   
Q_SIGNALS:
    void sigReceiveShortCutChanged();  
private:
    bool m_bReceiveShortCut;
    Q_PROPERTY(bool ReceiveShortCut READ GetReceiveShortCut WRITE SetReceiveShortCut NOTIFY sigReceiveShortCutChanged)
    
public:
    bool GetSaveMainWindowStatus() const;
    void SetSaveMainWindowStatus(bool newSaveMainWindowStatus);

Q_SIGNALS:
    void sigSaveMainWindowStatusChanged();
    
private:
    bool m_bSaveMainWindowStatus;
    Q_PROPERTY(bool SaveMainWindowStatus READ GetSaveMainWindowStatus WRITE SetSaveMainWindowStatus NOTIFY sigSaveMainWindowStatusChanged)
    
public:
    const QTabWidget::TabPosition &GetTabPosition() const;
    void SetTabPosition(const QTabWidget::TabPosition &newTabPosition);
Q_SIGNALS:
    void sigTabPositionChanged();
private:    
    QTabWidget::TabPosition m_TabPosition;
    Q_PROPERTY(QTabWidget::TabPosition TabPosition READ GetTabPosition WRITE SetTabPosition NOTIFY sigTabPositionChanged)
};

#endif // CPARAMETERAPP_H
