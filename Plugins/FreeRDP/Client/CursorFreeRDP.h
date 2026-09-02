// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QImage>
#include <QRect>
#include <QObject>
#include "freerdp/version.h"
#include "freerdp/freerdp.h"

class CBackendFreeRDP;
class CCursorFreeRDP : public QObject
{
    Q_OBJECT
public:
    explicit CCursorFreeRDP(CBackendFreeRDP *parent = nullptr);
    
    int RegisterPointer(rdpGraphics* graphics);
    struct _Pointer{
        rdpPointer pointer;
        CCursorFreeRDP* pThis;
    };
    
    static BOOL CbPointerNew(rdpContext* context, rdpPointer* pointer);
    static void CbPointerFree(rdpContext* context, rdpPointer* pointer);
    static BOOL CbPointerSet(rdpContext* context,
                           #if FREERDP_VERSION_MAJOR >= 3
                               rdpPointer* pointer
                           #else
                               const rdpPointer* pointer
                           #endif
                               );
    static BOOL CbPointerSetNull(rdpContext* context);
    static BOOL CbPointerSetDefault(rdpContext* context);
    static BOOL CbPointerSetPosition(rdpContext* context, UINT32 x, UINT32 y);
    
    BOOL OnNew(rdpContext* context, rdpPointer* pointer);
    void OnFree(rdpContext* context, rdpPointer* pointer);
    BOOL OnSet(rdpContext* context, const rdpPointer* pointer);
    BOOL OnSetNull(rdpContext* context);
    BOOL OnSetDefault(rdpContext* context);
    BOOL OnSetPosition(rdpContext* context,  UINT32 x, UINT32 y);
    
private:
    CBackendFreeRDP* m_pConnect;
    
    QImage m_Cursor;
};
