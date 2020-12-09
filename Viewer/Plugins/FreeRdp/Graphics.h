#ifndef CGRAPHICS_H
#define CGRAPHICS_H

#include <QObject>
#include "freerdp/freerdp.h"

class CGraphics : public QObject
{
    Q_OBJECT
public:
    explicit CGraphics(QObject *parent = nullptr);
    
    BOOL Registergraphics(rdpGraphics* graphics);
    
private:
    struct _BITMAP
    {
        rdpBitmap bitmap;
        CGraphics* pThis;
    };
    static BOOL cb_Bitmap_New(rdpContext* context, rdpBitmap* bitmap);
    static void cb_Bitmap_Free(rdpContext* context, rdpBitmap* bitmap);
    static BOOL cb_Bitmap_Paint(rdpContext* context, rdpBitmap* bitmap);
    static BOOL cb_Bitmap_SetSurface(rdpContext* context, rdpBitmap* bitmap, BOOL primary);

    /* Glyph Class */
    static BOOL cb_Glyph_New(rdpContext* context, const rdpGlyph* glyph);
    static void cb_Glyph_Free(rdpContext* context, rdpGlyph* glyph);
    static BOOL cb_Glyph_Draw(rdpContext* context, const rdpGlyph* glyph, INT32 x, INT32 y, INT32 w,
                              INT32 h, INT32 sx, INT32 sy, BOOL fOpRedundant);
    static BOOL cb_Glyph_BeginDraw(rdpContext* context, INT32 x, INT32 y, INT32 width, INT32 height,
                                   UINT32 bgcolor, UINT32 fgcolor, BOOL fOpRedundant);
    
    static BOOL cb_Glyph_EndDraw(rdpContext* context, INT32 x, INT32 y, INT32 width, INT32 height,
                                 UINT32 bgcolor, UINT32 fgcolor);
};

#endif // CGRAPHICS_H
