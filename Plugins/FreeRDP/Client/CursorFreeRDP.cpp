// Author: Kang Lin <kl222@126.com>
//! 
#include <QImage>
#include "ConnectFreeRDP.h"
#include <freerdp/gdi/gdi.h>

CCursorFreeRDP::CCursorFreeRDP(CConnectFreeRDP *parent) : QObject(parent),
    m_pConnect(parent),
    m_Logger("FreeRDP.Cursor")
{}

int CCursorFreeRDP::RegisterPointer(rdpGraphics *graphics)
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

BOOL CCursorFreeRDP::cb_Pointer_New(rdpContext *context, rdpPointer *pointer)
{
    //qDebug(m_Logger) << "cb_Pointer_New";
    CConnectFreeRDP* pThis = ((CConnectFreeRDP::ClientContext*)context)->pThis;
    return pThis->m_Cursor.onNew(context, pointer);
}

void CCursorFreeRDP::cb_Pointer_Free(rdpContext* context, rdpPointer* pointer)
{
    //qDebug(m_Logger) << "cb_Pointer_Free";
    CConnectFreeRDP* pThis = ((CConnectFreeRDP::ClientContext*)context)->pThis;
    pThis->m_Cursor.onFree(context, pointer);   
}

BOOL CCursorFreeRDP::cb_Pointer_Set(rdpContext *context,
                                    #if FreeRDP_VERSION_MAJOR >= 3
                                    rdpPointer* pointer
                                    #else
                                    const rdpPointer *pointer
                                    #endif
                                    )
{
    //qDebug(m_Logger) << "cb_Pointer_Set";
    CConnectFreeRDP* pThis = ((CConnectFreeRDP::ClientContext*)context)->pThis;
    return pThis->m_Cursor.onSet(context, pointer);
}

BOOL CCursorFreeRDP::cb_Pointer_SetNull(rdpContext *context)
{
    //qDebug(m_Logger) << "cb_Pointer_SetNull";
    CConnectFreeRDP* pThis = ((CConnectFreeRDP::ClientContext*)context)->pThis;
    return pThis->m_Cursor.onSetNull(context);
}

BOOL CCursorFreeRDP::cb_Pointer_SetDefault(rdpContext *context)
{
    //qDebug(m_Logger) << "cb_Pointer_SetDefault";
    CConnectFreeRDP* pThis = ((CConnectFreeRDP::ClientContext*)context)->pThis;
    return pThis->m_Cursor.onSetDefault(context);
    return TRUE;
}

BOOL CCursorFreeRDP::cb_Pointer_SetPosition(rdpContext *context, UINT32 x, UINT32 y)
{
    //qDebug(m_Logger) << "cb_Pointer_SetPosition";
    CConnectFreeRDP* pThis = ((CConnectFreeRDP::ClientContext*)context)->pThis;
    return pThis->m_Cursor.onSetPosition(context, x, y);
    return TRUE;
}

BOOL CCursorFreeRDP::onNew(rdpContext *context, rdpPointer *pointer)
{
    return TRUE;
    BOOL bRet = TRUE;
    QImage cursor(pointer->width, pointer->height, QImage::Format_ARGB32);
    /**
     * Drawing Monochrome Pointers:
     * http://msdn.microsoft.com/en-us/library/windows/hardware/ff556143/
     *
     * Drawing Color Pointers:
     * http://msdn.microsoft.com/en-us/library/windows/hardware/ff556138/
     */
    bRet = freerdp_image_copy_from_pointer_data(cursor.bits(),
                               CConnectFreeRDP::GetImageFormat(cursor.format()),
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
        emit m_pConnect->sigUpdateCursor(QCursor(QPixmap::fromImage(m_Cursor),
                                                 pointer->xPos, pointer->yPos));
    }
    return bRet;
}

void CCursorFreeRDP::onFree(rdpContext* context, rdpPointer* pointer)
{}

BOOL CCursorFreeRDP::onSet(rdpContext *context, const rdpPointer *pointer)
{
    BOOL bRet = TRUE;
    QImage cursor(pointer->width, pointer->height, QImage::Format_ARGB32);
    /**
     * Drawing Monochrome Pointers:
     * http://msdn.microsoft.com/en-us/library/windows/hardware/ff556143/
     *
     * Drawing Color Pointers:
     * http://msdn.microsoft.com/en-us/library/windows/hardware/ff556138/
     */
    bRet = freerdp_image_copy_from_pointer_data(cursor.bits(),
                               CConnectFreeRDP::GetImageFormat(cursor.format()),
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
        emit m_pConnect->sigUpdateCursor(QCursor(QPixmap::fromImage(m_Cursor),
                                                 pointer->xPos, pointer->yPos));
    }
    return bRet;
}

BOOL CCursorFreeRDP::onSetDefault(rdpContext *context)
{
    m_Cursor = QImage();
    emit m_pConnect->sigUpdateCursor(QCursor());
    return TRUE;
}

BOOL CCursorFreeRDP::onSetNull(rdpContext *context)
{
    m_Cursor = QImage();
    emit m_pConnect->sigUpdateCursor(QCursor(Qt::BlankCursor));
    return TRUE;
}

BOOL CCursorFreeRDP::onSetPosition(rdpContext *context, UINT32 x, UINT32 y)
{
    emit m_pConnect->sigUpdateCursor(QCursor(QPixmap::fromImage(m_Cursor),
                                                                   x, y));
    return TRUE;
}
