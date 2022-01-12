// Author: Kang Lin <kl222@126.com>

#ifndef CMIMEDATA_H
#define CMIMEDATA_H

#include <QMimeData>
#include <QMap>

#include "freerdp/freerdp.h"
#include "freerdp/client/cliprdr.h"

class CClipboardMimeData : public QMimeData
{
    Q_OBJECT
public:
    explicit CClipboardMimeData(CliprdrClientContext* pContext);
    virtual ~CClipboardMimeData();
    
    int AddFormat(int id, const QString& name);
    int SetData(const char *data, int len);
    
signals:
    void sigContinue();
    
public:
    virtual bool hasFormat(const QString &mimetype) const override;
    virtual QStringList formats() const override;
    
protected:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    virtual QVariant retrieveData(const QString &mimetype, QMetaType preferredType) const override;
#else
    virtual QVariant retrieveData(const QString &mimetype, QVariant::Type preferredType) const override;
#endif
private:
    CliprdrClientContext* m_pContext;
    QMap<int, QString> m_Fomats;
    QByteArray m_Data;
};

#endif // CMIMEDATA_H
