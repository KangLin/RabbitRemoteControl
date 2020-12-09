#include "Graphics.h"

CGraphics::CGraphics(QObject *parent) : QObject(parent)
{
}

BOOL CGraphics::Registergraphics(rdpGraphics *graphics)
{
    rdpBitmap bitmap;
	rdpGlyph glyph;
    
    if (!graphics || !graphics->Bitmap_Prototype || !graphics->Glyph_Prototype)
		return FALSE;
    
    bitmap = *graphics->Bitmap_Prototype;
	glyph = *graphics->Glyph_Prototype;
    bitmap.size = sizeof(_BITMAP);
	bitmap.New = cb_Bitmap_New;
	bitmap.Free = cb_Bitmap_Free;
	bitmap.Paint = cb_Bitmap_Paint;
	bitmap.SetSurface = cb_Bitmap_SetSurface;
	graphics_register_bitmap(graphics, &bitmap);
	//glyph.size = sizeof(xfGlyph);
	glyph.New = cb_Glyph_New;
	glyph.Free = cb_Glyph_Free;
	glyph.Draw = cb_Glyph_Draw;
	glyph.BeginDraw = cb_Glyph_BeginDraw;
	glyph.EndDraw = cb_Glyph_EndDraw;
	graphics_register_glyph(graphics, &glyph);
	return TRUE;
}
