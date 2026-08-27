// Author: Kang Lin <kl222@126.com>
//! 
#include <QImage>
#include <freerdp/gdi/gdi.h>
#include <QLoggingCategory>
#include "BackendFreeRDP.h"

static Q_LOGGING_CATEGORY(log, "FreeRDP.Cursor")
CCursorFreeRDP::CCursorFreeRDP(CBackendFreeRDP *parent)
    : QObject(parent),
    m_pConnect(parent)
{}

int CCursorFreeRDP::RegisterPointer(rdpGraphics *graphics)
{
    rdpPointer* pointer = NULL;
	if (!(pointer = (rdpPointer*)calloc(1, sizeof(rdpPointer))))
		return -1;
	pointer->size = sizeof(_Pointer);
    pointer->New = CbPointerNew;
    pointer->Free = CbPointerFree;
    pointer->Set = CbPointerSet;
    pointer->SetNull = CbPointerSetNull;
    pointer->SetDefault = CbPointerSetDefault;
    pointer->SetPosition = CbPointerSetPosition;
	graphics_register_pointer(graphics, pointer);
	free(pointer);
    return 0;
}

BOOL CCursorFreeRDP::CbPointerNew(rdpContext *context, rdpPointer *pointer)
{
    //qDebug(log) << "CbPointerNew";
    CBackendFreeRDP* pThis = ((CBackendFreeRDP::ClientContext*)context)->pThis;
    return pThis ? pThis->m_Cursor.OnNew(context, pointer) : false;
}

void CCursorFreeRDP::CbPointerFree(rdpContext* context, rdpPointer* pointer)
{
    //qDebug(log) << "CbPointerFree";
    CBackendFreeRDP* pThis = ((CBackendFreeRDP::ClientContext*)context)->pThis;
    if(pThis)
        pThis->m_Cursor.OnFree(context, pointer);
}

BOOL CCursorFreeRDP::CbPointerSet(rdpContext *context,
                                    #if FREERDP_VERSION_MAJOR >= 3
                                    rdpPointer* pointer
                                    #else
                                    const rdpPointer *pointer
                                    #endif
                                    )
{
    //qDebug(log) << "CbPointerSet";
    CBackendFreeRDP* pThis = ((CBackendFreeRDP::ClientContext*)context)->pThis;
    return pThis ? pThis->m_Cursor.OnSet(context, pointer) : false;
}

BOOL CCursorFreeRDP::CbPointerSetNull(rdpContext *context)
{
    //qDebug(log) << "CbPointerSetNull";
    CBackendFreeRDP* pThis = ((CBackendFreeRDP::ClientContext*)context)->pThis;
    return pThis ? pThis->m_Cursor.OnSetNull(context) : false;
}

BOOL CCursorFreeRDP::CbPointerSetDefault(rdpContext *context)
{
    //qDebug(log) << "CbPointerSetDefault";
    CBackendFreeRDP* pThis = ((CBackendFreeRDP::ClientContext*)context)->pThis;
    return pThis ? pThis->m_Cursor.OnSetDefault(context) : false;
}

BOOL CCursorFreeRDP::CbPointerSetPosition(rdpContext *context, UINT32 x, UINT32 y)
{
    //qDebug(log) << "CbPointerSetPosition:" << x << y;
    CBackendFreeRDP* pThis = ((CBackendFreeRDP::ClientContext*)context)->pThis;
    return pThis ? pThis->m_Cursor.OnSetPosition(context, x, y) : false;
}

BOOL CCursorFreeRDP::OnNew(rdpContext *context, rdpPointer *pointer)
{
    qDebug(log) << "onNew:" << pointer->xPos << pointer->yPos << pointer->width << pointer->height;
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
                               CBackendFreeRDP::GetImageFormat(cursor.format()),
                               0,
                               0, 0, cursor.width(), cursor.height(),
                               pointer->xorMaskData,
                               pointer->lengthXorMask,
                               pointer->andMaskData,
                               pointer->lengthAndMask,
                               pointer->xorBpp,
                               &context->gdi->palette);
    if(bRet) {
        m_Cursor = cursor;
        emit m_pConnect->sigUpdateCursor(
            QCursor(QPixmap::fromImage(m_Cursor), pointer->xPos, pointer->yPos));
    }
    return bRet;
}

void CCursorFreeRDP::OnFree(rdpContext* context, rdpPointer* pointer)
{
    qDebug(log) << "OnFree";
    Q_UNUSED(context)
    Q_UNUSED(pointer)
}

BOOL CCursorFreeRDP::OnSet(rdpContext *context, const rdpPointer *pointer)
{
    qDebug(log) << "OnSet:" << pointer->xPos << pointer->yPos << pointer->width << pointer->height;
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
                               CBackendFreeRDP::GetImageFormat(cursor.format()),
                               0,
                               0, 0, cursor.width(), cursor.height(),
                               pointer->xorMaskData,
                               pointer->lengthXorMask,
                               pointer->andMaskData,
                               pointer->lengthAndMask,
                               pointer->xorBpp,
                               &context->gdi->palette);
    if(bRet) {
        m_Cursor = cursor;
        emit m_pConnect->sigUpdateCursor(
            QCursor(QPixmap::fromImage(m_Cursor), pointer->xPos, pointer->yPos));
    }
    return bRet;
}

BOOL CCursorFreeRDP::OnSetDefault(rdpContext *context)
{
    qDebug(log) << "OnSetDefault";
    Q_UNUSED(context)
    m_Cursor = QImage();
    emit m_pConnect->sigUpdateCursor(QCursor());
    return TRUE;
}

BOOL CCursorFreeRDP::OnSetNull(rdpContext *context)
{
    qDebug(log) << "OnSetNull";
    Q_UNUSED(context)
    m_Cursor = QImage();
    emit m_pConnect->sigUpdateCursor(QCursor(Qt::BlankCursor));
    return TRUE;
}

BOOL CCursorFreeRDP::OnSetPosition(rdpContext *context, UINT32 x, UINT32 y)
{
    qDebug(log) << "OnSetPosition" << x << y;
    Q_UNUSED(context)
    emit m_pConnect->sigUpdateCursorPosition(QPoint(x, y));
    return TRUE;
}
