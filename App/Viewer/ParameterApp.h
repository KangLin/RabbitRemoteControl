#ifndef CPARAMETERAPP_H
#define CPARAMETERAPP_H

#include <QObject>

class CParameterApp : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool ReceiveShortCut READ GetReceiveShortCut WRITE SetReceiveShortCut NOTIFY sigReceiveShortCutChanged)
    Q_PROPERTY(bool ScreenSlot READ GetScreenSlot WRITE SetScreenSlot NOTIFY sigScreenSlotChanged)

public:
    explicit CParameterApp(QObject *parent = nullptr);
    
    bool GetReceiveShortCut() const;
    void SetReceiveShortCut(bool newReceiveShortCut);
    
    bool GetSlotScreen() const;
    void SetSlotScreen(bool newSlotScreen);
    
    bool GetScreenSlot() const;
    void SetScreenSlot(bool newScreenSlot);
    
signals:
    void sigReceiveShortCutChanged();
    void sigScreenSlotChanged();
    
private:
    bool m_bReceiveShortCut;
    bool m_bScreenSlot;
};

#endif // CPARAMETERAPP_H
