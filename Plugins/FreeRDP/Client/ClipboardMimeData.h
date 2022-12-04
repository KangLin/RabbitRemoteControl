// Author: Kang Lin <kl222@126.com>

#ifndef CMIMEDATA_H
#define CMIMEDATA_H

#include <QMimeData>
#include <QMultiMap>
#include <QVector>
#include <QFile>
#include <QSharedPointer>
#include <QLoggingCategory>
#include <QAtomicInteger>

#include "freerdp/client/cliprdr.h"
#include "winpr/clipboard.h"

#define MIME_TYPE_RABBITREMOTECONTROL_PLUGINS_FREERDP "rabbit/remotecontrol/plugins/freerdp"
class CClipboardFreeRDP;
class CClipboardMimeData : public QMimeData
{
    Q_OBJECT
public:
    explicit CClipboardMimeData(CliprdrClientContext* pContext);
    virtual ~CClipboardMimeData();
    
    const qint32 GetId() const;
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
    virtual QVariant retrieveData(const QString &mimeType, QMetaType preferredType) const override;
#else
    virtual QVariant retrieveData(const QString &mimeType, QVariant::Type preferredType) const override;
#endif

private:
    int AddFormat(UINT32 id, const char *name);

    bool isText(QString mimeType, bool bRegular = true) const;
    bool isHtml(QString mimeType, bool bRegular = true) const;
    bool isImage(QString mimeType, bool bRegular = true) const;
    bool isUrls(QString mimeType, bool bRegular = true) const;

Q_SIGNALS:
    void sigRequestFileFromServer(const QString& mimetype, const QString& valueName,
                                  const void* pData, const UINT32 nLen) const;
private Q_SLOTS:
    void slotRequestFileFromServer(const QString& mimeType, const QString& valueName,
                                   const void* pData, const UINT32 nLen);
private:
    UINT sendRequestFilecontents(UINT32 listIndex,
                                 UINT32 dwFlags,
                                 DWORD nPositionHigh,
                                 DWORD nPositionLow,
                                 UINT32 cbRequested);
private:
    friend CClipboardFreeRDP;
    QLoggingCategory m_Log;
    static QAtomicInteger<qint32> m_nId;
    qint32 m_Id;

    CliprdrClientContext* m_pContext;
    wClipboard* m_pClipboard; // Clipboard interface provided by winpr library

    QVector<_FORMAT> m_Formats; // Save server format
    QMultiMap<QString, _FORMAT> m_indexString;
    QMap<UINT32, _FORMAT> m_indexId;
    QStringList m_lstFormats; // Clipboard return farmat

    QVariant m_Variant;
    QVariant m_gnomeFiles;
    QVariant m_uriFiles;
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
