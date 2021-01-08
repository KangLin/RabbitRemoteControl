//! @author: Kang Lin(kl222@126.com)

#ifndef CCURSOR_H
#define CCURSOR_H

#include <QImage>
#include <QRect>
#include <QObject>
#include "freerdp/freerdp.h"

class CConnectFreeRdp;
class CCursorFreeRdp : public QObject
{
    Q_OBJECT
public:
    explicit CCursorFreeRdp(CConnectFreeRdp *parent = nullptr);
    
    int RegisterPointer(rdpGraphics* graphics);
    struct _Pointer{
        rdpPointer pointer;
        CCursorFreeRdp* pThis;
    };
    
    static BOOL cb_Pointer_New(rdpContext* context, rdpPointer* pointer);
    static void cb_Pointer_Free(rdpContext* context, rdpPointer* pointer);
    static BOOL cb_Pointer_Set(rdpContext* context, const rdpPointer* pointer);
    static BOOL cb_Pointer_SetNull(rdpContext* context);
    static BOOL cb_Pointer_SetDefault(rdpContext* context);
    static BOOL cb_Pointer_SetPosition(rdpContext* context, UINT32 x, UINT32 y);
    
    BOOL onNew(rdpContext* context, rdpPointer* pointer);
    void onFree(rdpContext* context, rdpPointer* pointer);
    BOOL onSet(rdpContext* context, const rdpPointer* pointer);
    BOOL onSetNull(rdpContext* context);
    BOOL onSetDefault(rdpContext* context);
    BOOL onSetPosition(rdpContext* context,  UINT32 x, UINT32 y);
    
private:
    CConnectFreeRdp* m_pConnect;
    
    QImage m_Cursor;
};

#endif // CCURSOR_H
