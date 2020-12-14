#ifndef CCURSOR_H
#define CCURSOR_H

#include <QObject>
#include "freerdp/freerdp.h"

class CConnectFreeRdp;
class CCursor : public QObject
{
    Q_OBJECT
public:
    explicit CCursor(CConnectFreeRdp *parent = nullptr);
    
    int RegisterPointer(rdpGraphics* graphics);
    struct _Pointer{
        rdpPointer pointer;
        CCursor* pThis;
    };
    
    static BOOL cb_Pointer_New(rdpContext* context, rdpPointer* pointer);
    static void cb_Pointer_Free(rdpContext* context, rdpPointer* pointer);
    static BOOL cb_Pointer_Set(rdpContext* context, const rdpPointer* pointer);
    static BOOL cb_Pointer_SetNull(rdpContext* context);
    static BOOL cb_Pointer_SetDefault(rdpContext* context);
    static BOOL cb_Pointer_SetPosition(rdpContext* context, UINT32 x, UINT32 y);
    
private:
    CConnectFreeRdp* m_pConnect;
};

#endif // CCURSOR_H
