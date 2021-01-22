#ifndef CCONNECTERTERMINAL_H
#define CCONNECTERTERMINAL_H

#include "Connecter.h"
#include "qtermwidget.h"

class CConnecterTerminal : public CConnecter
{
    Q_OBJECT
public:
    explicit CConnecterTerminal(CPluginFactory *parent);
    virtual ~CConnecterTerminal();
    
    // CConnecter interface
public:
    QWidget* GetViewer() override;
    virtual qint16 Version() override;
    virtual int Load(QDataStream &d) override;
    virtual int Save(QDataStream &d) override;
    /**
     * @brief Open settings dialog
     * @param parent
     * @return DialogCode
     *   QDialog::Accepted
     *   QDialog::Rejected
     *   -1: error
     */
    virtual int OpenDialogSettings(QWidget* parent = nullptr) override;
    
public slots:
    virtual int Connect() override;
    virtual int DisConnect() override;

    void slotTerminalTitleChanged();
    
protected:
    virtual QDialog *GetDialogSettings(QWidget *parent) override;
    virtual int SetParamter();
    
private:
    QTermWidget* m_pConsole;
};

#endif // CCONNECTERTERMINAL_H
