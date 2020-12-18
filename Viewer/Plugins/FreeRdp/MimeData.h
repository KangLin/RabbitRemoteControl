#ifndef CMIMEDATA_H
#define CMIMEDATA_H

#include <QMimeData>
#include <QMap>

#include "freerdp/freerdp.h"
#include "freerdp/client/cliprdr.h"

class CMimeData : public QMimeData
{
    Q_OBJECT
public:
    explicit CMimeData(CliprdrClientContext* pContext);
    virtual ~CMimeData();
    
    int AddFormat(int id, const QString& name);
    int SetData(char* data, int len);
    
signals:
    void sigContinue();
    
public:
    virtual bool hasFormat(const QString &mimetype) const;
    virtual QStringList formats() const;
    
protected:
    virtual QVariant retrieveData(const QString &mimetype, QVariant::Type preferredType) const;
    
private:
    CliprdrClientContext* m_pContext;
    QMap<int, QString> m_Fomats;
    QByteArray m_Data;
    
};

#endif // CMIMEDATA_H
