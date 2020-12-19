//! @author: Kang Lin(kl222@126.com)
//! 
#include <QImage>
#include "ConnectFreeRdp.h"
#include "RabbitCommonLog.h"
#include <freerdp/gdi/gdi.h>

CCursorFreeRdp::CCursorFreeRdp(CConnectFreeRdp *parent) : QObject(parent),
    m_pConnect(parent)
{
}

int CCursorFreeRdp::RegisterPointer(rdpGraphics *graphics)
{
    rdpPointer* pointer = NULL;
	if (!(pointer = (rdpPointer*)calloc(1, sizeof(rdpPointer))))
		return -1;

	pointer->size = sizeof(_Pointer);
	pointer->New = cb_Pointer_New;
	pointer->Free = cb_Pointer_Free;
	pointer->Set = cb_Pointer_Set;
	pointer->SetNull = cb_Pointer_SetNull;
	pointer->SetDefault = cb_Pointer_SetDefault;
	pointer->SetPosition = cb_Pointer_SetPosition;
	graphics_register_pointer(graphics, pointer);
	free(pointer);
    
    return 0;
}

BOOL CCursorFreeRdp::cb_Pointer_New(rdpContext *context, rdpPointer *pointer)
{
    //LOG_MODEL_DEBUG("FreeRdp", "cb_Pointer_New");
    CConnectFreeRdp* pThis = ((CConnectFreeRdp::ClientContext*)context)->pThis;
    return pThis->m_Cursor.onNew(context, pointer);
}

void CCursorFreeRdp::cb_Pointer_Free(rdpContext* context, rdpPointer* pointer)
{
    //LOG_MODEL_DEBUG("FreeRdp", "cb_Pointer_Free");
    CConnectFreeRdp* pThis = ((CConnectFreeRdp::ClientContext*)context)->pThis;
    pThis->m_Cursor.onFree(context, pointer);   
}

BOOL CCursorFreeRdp::cb_Pointer_Set(rdpContext *context, const rdpPointer *pointer)
{
    //LOG_MODEL_DEBUG("FreeRdp", "cb_Pointer_Set");
    CConnectFreeRdp* pThis = ((CConnectFreeRdp::ClientContext*)context)->pThis;
    return pThis->m_Cursor.onSet(context, pointer);
}

BOOL CCursorFreeRdp::cb_Pointer_SetNull(rdpContext *context)
{
    //LOG_MODEL_DEBUG("FreeRdp", "cb_Pointer_SetNull");
    CConnectFreeRdp* pThis = ((CConnectFreeRdp::ClientContext*)context)->pThis;
    return pThis->m_Cursor.onSetNull(context);
}

BOOL CCursorFreeRdp::cb_Pointer_SetDefault(rdpContext *context)
{
    //LOG_MODEL_DEBUG("FreeRdp", "cb_Pointer_SetDefault");
    CConnectFreeRdp* pThis = ((CConnectFreeRdp::ClientContext*)context)->pThis;
    return pThis->m_Cursor.onSetDefault(context);
    return TRUE;
}

BOOL CCursorFreeRdp::cb_Pointer_SetPosition(rdpContext *context, UINT32 x, UINT32 y)
{
    //LOG_MODEL_DEBUG("FreeRdp", "cb_Pointer_SetPosition");
    CConnectFreeRdp* pThis = ((CConnectFreeRdp::ClientContext*)context)->pThis;
    return pThis->m_Cursor.onSetPosition(context, x, y);
    return TRUE;
}

BOOL CCursorFreeRdp::onNew(rdpContext *context, rdpPointer *pointer)
{
    return TRUE;
    BOOL bRet = TRUE;
    m_Rect = QRect(pointer->xPos, pointer->yPos, pointer->width, pointer->height);
    QImage cursor(pointer->width, pointer->height, QImage::Format_ARGB32);
    /**
     * Drawing Monochrome Pointers:
     * http://msdn.microsoft.com/en-us/library/windows/hardware/ff556143/
     *
     * Drawing Color Pointers:
     * http://msdn.microsoft.com/en-us/library/windows/hardware/ff556138/
     */
    bRet = freerdp_image_copy_from_pointer_data(cursor.bits(),
                               CConnectFreeRdp::GetImageFormat(cursor.format()),
                               0,
                               0, 0, cursor.width(), cursor.height(),
                               pointer->xorMaskData,
                               pointer->lengthXorMask,
                               pointer->andMaskData,
                               pointer->lengthAndMask,
                               pointer->xorBpp,
                               &context->gdi->palette);
    if(bRet)
    {
        m_Cursor = cursor;
        emit m_pConnect->sigUpdateCursor(m_Rect, m_Cursor);
    }
    return bRet;
}

void CCursorFreeRdp::onFree(rdpContext* context, rdpPointer* pointer)
{}

BOOL CCursorFreeRdp::onSet(rdpContext *context, const rdpPointer *pointer)
{
    BOOL bRet = TRUE;
    m_Rect = QRect(pointer->xPos, pointer->yPos, pointer->width, pointer->height);
    QImage cursor(pointer->width, pointer->height, QImage::Format_ARGB32);
    /**
     * Drawing Monochrome Pointers:
     * http://msdn.microsoft.com/en-us/library/windows/hardware/ff556143/
     *
     * Drawing Color Pointers:
     * http://msdn.microsoft.com/en-us/library/windows/hardware/ff556138/
     */
    bRet = freerdp_image_copy_from_pointer_data(cursor.bits(),
                               CConnectFreeRdp::GetImageFormat(cursor.format()),
                               0,
                               0, 0, cursor.width(), cursor.height(),
                               pointer->xorMaskData,
                               pointer->lengthXorMask,
                               pointer->andMaskData,
                               pointer->lengthAndMask,
                               pointer->xorBpp,
                               &context->gdi->palette);
    if(bRet)
    {
        m_Cursor = cursor;
        emit m_pConnect->sigUpdateCursor(m_Rect, m_Cursor);
    }
    return bRet;
}

BOOL CCursorFreeRdp::onSetDefault(rdpContext *context)
{
    m_Cursor = QImage();
    emit m_pConnect->sigUpdateCursor(m_Rect, m_Cursor);
    return TRUE;
}

BOOL CCursorFreeRdp::onSetNull(rdpContext *context)
{
    m_Cursor = QImage();
    emit m_pConnect->sigUpdateCursor(m_Rect, m_Cursor);
    return TRUE;
}

BOOL CCursorFreeRdp::onSetPosition(rdpContext *context, UINT32 x, UINT32 y)
{
    m_Rect.moveTo(x, y);
    emit m_pConnect->sigUpdateCursor(m_Rect, m_Cursor);
    return TRUE;
}
