#ifndef CCONNECTERTERMINAL_H
#define CCONNECTERTERMINAL_H

#include "Connecter.h"
#include "qtermwidget.h"
#include "ParameterTerminalAppearance.h"

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
    
public Q_SLOTS:
    virtual int Connect() override;
    virtual int DisConnect() override;

private Q_SLOTS:
    void slotTerminalTitleChanged();
    
protected:
    virtual QDialog* GetDialogSettings(QWidget *parent);
    virtual int SetParamter();

    virtual int OnConnect();
    virtual int OnDisConnect();
    virtual int OnLoad(QDataStream& d);
    virtual int OnSave(QDataStream& d);
    
private:
    QTermWidget* m_pConsole;
    
    CParameterTerminalAppearance m_Para;
};

#endif // CCONNECTERTERMINAL_H
