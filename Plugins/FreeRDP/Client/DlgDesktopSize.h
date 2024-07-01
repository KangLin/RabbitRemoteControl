#ifndef DLGDESKTOPSIZE_H
#define DLGDESKTOPSIZE_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class CDlgDesktopSize;
}

class CDlgDesktopSize : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgDesktopSize(QWidget *parent = nullptr);
    virtual ~CDlgDesktopSize();
    
    void SetDesktopSizes(const QStringList& lstSize);
    const QStringList GetDesktopSize();
    
private slots:
    void on_pbAdd_clicked();
    void on_pbRemove_clicked();
    void slotItemChanged(QStandardItem *item);
    
private:
    bool checkFormat(QString size);

private:
    Ui::CDlgDesktopSize *ui;
    
    QStringList m_lstDesktopSize;
    QStandardItemModel* m_pModel;
};

#endif // DLGDESKTOPSIZE_H
