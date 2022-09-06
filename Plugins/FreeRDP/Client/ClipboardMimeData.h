// Author: Kang Lin <kl222@126.com>

#ifndef CMIMEDATA_H
#define CMIMEDATA_H

#include <QMimeData>
#include <QMultiMap>
#include <QVector>
#include <QFile>
#include <QSharedPointer>

#include "freerdp/freerdp.h"
#include "freerdp/client/cliprdr.h"
#include "winpr/clipboard.h"

class CClipboardFreeRDP;
class CClipboardMimeData : public QMimeData
{
    Q_OBJECT
public:
    explicit CClipboardMimeData(CliprdrClientContext* pContext);
    virtual ~CClipboardMimeData();
    
    struct _FORMAT {
        UINT32 id;
        QString name;
        UINT32 localId;
    };
    
    int SetFormat(const CLIPRDR_FORMAT_LIST* pList);

Q_SIGNALS:
    void sigSendDataRequest(CliprdrClientContext* context,
                            UINT32 formatId) const;
    void sigContinue();
    
    
public:
    virtual bool hasFormat(const QString &mimetype) const override;
    virtual QStringList formats() const override;
    
public Q_SLOTS:
    void slotServerFormatData(const BYTE* pData, UINT32 nLen,
                             UINT32 id);
    void slotServerFileContentsRespose(UINT32 streamId, QByteArray& data);

protected:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    virtual QVariant retrieveData(const QString &mimetype, QMetaType preferredType) const override;
#else
    virtual QVariant retrieveData(const QString &mimetype, QVariant::Type preferredType) const override;
#endif

private:
    int AddFormat(UINT32 id, const char *name);

    bool isText(QString mimeType, bool bRegular = true) const;
    bool isHtml(QString mimeType, bool bRegular = true) const;
    bool isImage(QString mimeType, bool bRegular = true) const;
    bool isUrls(QString mimeType, bool bRegular = true) const;

Q_SIGNALS:
    void sigRequestFileFromServer(const QString& mimetype, void* pData, UINT32 nLen) const;
private Q_SLOTS:
    void slotRequestFileFromServer(const QString& mimetype, void* pData, UINT32 nLen);
private:
    UINT sendRequestFilecontents(ULONG index,
                                           UINT32 flag,
                                           DWORD positionhigh,
                                           DWORD positionlow,
                                           UINT32 nreq);
private:
    friend CClipboardFreeRDP;
    CliprdrClientContext* m_pContext;
    wClipboard* m_pClipboard; // Clipboard interface provided by winpr

    QVector<_FORMAT> m_Formats; // Save server format
    QMultiMap<QString, _FORMAT> m_indexString;
    QMap<UINT32, _FORMAT> m_indexId;
    QStringList m_lstFormats; // Clipboard return farmat

    QVariant m_Variant;
    bool m_bFile;
    bool m_bExit;
    
    struct _CliprdrFileStream
    {
        QFile m_File;
        QByteArray m_Data;
        bool m_Success;
    };
    QMap<UINT32, QSharedPointer<_CliprdrFileStream> > m_Stream;
};

#endif // CMIMEDATA_H
