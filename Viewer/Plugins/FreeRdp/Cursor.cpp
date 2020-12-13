#include "Cursor.h"
#include <QImage>
#include "ConnectFreeRdp.h"

CCursor::CCursor(QObject *parent) : QObject(parent)
{
}

int CCursor::RegisterPointer(rdpGraphics *graphics)
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

BOOL CCursor::cb_Pointer_New(rdpContext *context, rdpPointer *pointer)
{
    QImage cursor(pointer->width, pointer->height, QImage::Format_ARGB32);
    /**
     * Drawing Monochrome Pointers:
     * http://msdn.microsoft.com/en-us/library/windows/hardware/ff556143/
     *
     * Drawing Color Pointers:
     * http://msdn.microsoft.com/en-us/library/windows/hardware/ff556138/
     */
    /*
    freerdp_image_copy_from_pointer_data(cursor.bits(),
                              CConnectFreeRdp::GetImageFormat(cursor.format()),
                                      0,
                              0, 0, cursor.width(), cursor.height(),
                              pointer->xorMaskData, pointer->lengthXorMask, pointer->andMaskData,
                pointer->lengthAndMask, pointer->xorBpp, cursor.colorCount());
                //*/
    return TRUE;
}

void CCursor::cb_Pointer_Free(rdpContext* context, rdpPointer* pointer)
{
    
}

BOOL CCursor::cb_Pointer_Set(rdpContext *context, const rdpPointer *pointer)
{
    return TRUE;
}

BOOL CCursor::cb_Pointer_SetNull(rdpContext *context)
{
    return TRUE;
}

BOOL CCursor::cb_Pointer_SetDefault(rdpContext *context)
{
    return TRUE;
}

BOOL CCursor::cb_Pointer_SetPosition(rdpContext *context, UINT32 x, UINT32 y)
{
    return TRUE;
}
