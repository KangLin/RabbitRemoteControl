// Author: Kang Lin <kl222@126.com>

#ifndef CMIMEDATA_H
#define CMIMEDATA_H

#include <QMimeData>
#include <QMap>
#include <QVector>

#include "freerdp/freerdp.h"
#include "freerdp/client/cliprdr.h"
#include "winpr/clipboard.h"

class CClipboardMimeData : public QMimeData
{
    Q_OBJECT
public:
    explicit CClipboardMimeData(CliprdrClientContext* pContext);
    virtual ~CClipboardMimeData();
    
    struct _FORMAT {
        UINT32 id;
        QString name;
    };
    
    int SetFormat(const CLIPRDR_FORMAT_LIST* pList);
    int AddFormat(UINT32 id, const char *name);
    
Q_SIGNALS:
    void sigSendDataRequest(CliprdrClientContext* context,
                            UINT32 formatId, QString formatName) const;
    void sigContinue();
    
public:
    virtual bool hasFormat(const QString &mimetype) const override;
    virtual QStringList formats() const override;
    
public Q_SLOTS:
    int slotServerFormatData(const BYTE* pData, UINT32 nLen,
                             UINT32 id, QString name);

protected:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    virtual QVariant retrieveData(const QString &mimetype, QMetaType preferredType) const override;
    QVariant GetValue(QString mimeType, QMetaType preferredType) const;
#else
    virtual QVariant retrieveData(const QString &mimetype, QVariant::Type preferredType) const override;
    QVariant GetValue(QString mimeType, QVariant::Type preferredType) const;
#endif

private:
    bool isText(QString mimeType) const;
    bool isHtml(QString mimeType) const;
    bool isImage(QString mimeType) const;

private:
    CliprdrClientContext* m_pContext;
    wClipboard* m_pClipboard; // Clipboard interface provided by winpr
    
    QVector<_FORMAT> m_Formats;
    QMap<QString, UINT32> m_outFormats;
    QStringList m_lstFormats;

    QMap<QString, QVariant> m_Variant;
    bool m_bExit;
    
};

#endif // CMIMEDATA_H
