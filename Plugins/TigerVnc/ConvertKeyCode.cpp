//! Author: Kang Lin <kl222@126.com>

#include "ConvertKeyCode.h"

#if defined(HAVE_TigerVNC) || defined(HAVE_RabbitVNC)
// the X11 headers on some systems
#ifndef XK_VoidSymbol
#define XK_LATIN1
#define XK_MISCELLANY
#define XK_XKB_KEYS
#include <rfb/keysymdef.h>
#endif
#endif

#ifdef HAVE_LIBVNCSERVER
#include "rfb/rfbclient.h"
#endif

CConvertKeyCode::CConvertKeyCode(QObject *parent)
    : QObject{parent}
{}

/**
 * @brief CConvertKeyCode::QtToRfbKey
 * @param inkey
 * @param modifier
 * @return
 * @see https://github.com/rfbproto/rfbproto/blob/master/rfbproto.rst#keyevent
 */
quint32 CConvertKeyCode::QtToRfbKey(int inkey, bool modifier)
{
    quint32 k = 5000;

    switch (inkey)
    {
    case Qt::Key_Backspace: k = XK_BackSpace; break;
    case Qt::Key_Tab: k = XK_Tab; break;
    case Qt::Key_Clear: k = XK_Clear; break;
    case Qt::Key_Return: k = XK_Return; break;
    case Qt::Key_Pause: k = XK_Pause; break;
    case Qt::Key_Escape: k = XK_Escape; break;
    case Qt::Key_Space: k = XK_space; break;
    case Qt::Key_Delete: k = XK_Delete; break;
    case Qt::Key_Period: k = XK_period; break;

    /* International & multi-key character composition */
    case Qt::Key_Multi_key: k = XK_Multi_key; break;
#if defined(HAVE_TigerVNC) || defined(HAVE_RabbitVNC)
    case Qt::Key_Codeinput: k = XK_Codeinput; break;
#endif
    case Qt::Key_SingleCandidate: k = XK_SingleCandidate; break;
    case Qt::Key_MultipleCandidate: k = XK_MultipleCandidate; break;
    case Qt::Key_PreviousCandidate: k = XK_PreviousCandidate; break;

    /* Japanese keyboard support */
    case Qt::Key_Kanji: k = XK_Kanji; break;
    case Qt::Key_Muhenkan: k = XK_Muhenkan; break;
    case Qt::Key_Henkan: k = XK_Henkan; break;
    case Qt::Key_Romaji: k = XK_Romaji; break;
    case Qt::Key_Hiragana: k = XK_Hiragana; break;
    case Qt::Key_Katakana: k = XK_Katakana; break;
    case Qt::Key_Hiragana_Katakana:	k = XK_Hiragana_Katakana;break;
    case Qt::Key_Zenkaku: k = XK_Zenkaku; break;
    case Qt::Key_Hankaku: k = XK_Hankaku; break;
    case Qt::Key_Zenkaku_Hankaku: k = XK_Zenkaku_Hankaku; break;
    case Qt::Key_Touroku: k = XK_Touroku; break;
    case Qt::Key_Massyo: k = XK_Massyo; break;
    case Qt::Key_Kana_Lock: k = XK_Kana_Lock; break;
    case Qt::Key_Kana_Shift: k = XK_Kana_Shift; break;
    case Qt::Key_Eisu_Shift: k = XK_Eisu_Shift; break;
    case Qt::Key_Eisu_toggle: k = XK_Eisu_toggle; break;

    //special keyboard char
    case Qt::Key_Exclam: k = XK_exclam; break; //!
    case Qt::Key_QuoteDbl: k = XK_quotedbl; break; //"
    case Qt::Key_NumberSign: k = XK_numbersign; break; //#
    case Qt::Key_Percent: k = XK_percent; break; //%
    case Qt::Key_Dollar: k = XK_dollar; break;   //$
    case Qt::Key_Ampersand: k = XK_ampersand; break; //&
    case Qt::Key_Apostrophe: k = XK_apostrophe; break;//!
    case Qt::Key_ParenLeft: k = XK_parenleft; break; // (
    case Qt::Key_ParenRight: k = XK_parenright; break; // )

    case Qt::Key_Slash: k = XK_slash; break;    // /
    case Qt::Key_Asterisk: k = XK_asterisk; break;  //*
    case Qt::Key_Minus: k = XK_minus; break;    //-
    case Qt::Key_Plus: k = XK_plus; break;  //+
    case Qt::Key_Enter: k = XK_Return; break;   //
    case Qt::Key_Equal: k = XK_equal; break;    //=
    case Qt::Key_Comma: return XK_comma; //,

        case Qt::Key_Colon: k = XK_colon;break; // :
    case Qt::Key_Semicolon: k = XK_semicolon; break; //;
    case Qt::Key_Less: k = XK_less; break; // <
    case Qt::Key_Greater: k = XK_greater; break; // >
    case Qt::Key_Question: k = XK_question; break; //?
    case Qt::Key_At: k = XK_at; break; //@

    case Qt::Key_BracketLeft: k = XK_bracketleft; break;
    case Qt::Key_Backslash: k = XK_backslash;break;
    case Qt::Key_BracketRight: k = XK_bracketright;break;
    case Qt::Key_AsciiCircum: k = XK_asciicircum;break;
    case Qt::Key_Underscore: k = XK_underscore;break;
    case Qt::Key_QuoteLeft: k = XK_quoteleft;break;
    case Qt::Key_BraceLeft: k = XK_braceleft;break;
    case Qt::Key_Bar: k = XK_bar; break;
    case Qt::Key_BraceRight: k = XK_braceright;break;
    case Qt::Key_AsciiTilde: k = XK_asciitilde;break;
    case Qt::Key_nobreakspace: k = XK_nobreakspace;break;
    case Qt::Key_exclamdown: k = XK_exclamdown;break;
    case Qt::Key_cent: k = XK_cent;break;
    case Qt::Key_sterling: k = XK_sterling;break;
    case Qt::Key_currency: k = XK_currency;break;
    case Qt::Key_yen: k = XK_yen;break;
    case Qt::Key_brokenbar: k = XK_brokenbar;break;
    case Qt::Key_section: k = XK_section;break;
    case Qt::Key_diaeresis: k = XK_diaeresis;break;
    case Qt::Key_copyright: k = XK_copyright; break;
    case Qt::Key_ordfeminine: k = XK_ordfeminine; break;
    case Qt::Key_guillemotleft: k = XK_guillemotleft; break;
    case Qt::Key_guillemotright: k = XK_guillemotright; break;
    case Qt::Key_notsign: k = XK_notsign; break;
    case Qt::Key_hyphen: k = XK_hyphen; break;
    case Qt::Key_registered: k = XK_registered; break;

    case Qt::Key_Up: k = XK_Up; break;
    case Qt::Key_Down: k = XK_Down; break;
    case Qt::Key_Right: k = XK_Right; break;
    case Qt::Key_Left: k = XK_Left; break;
    case Qt::Key_Insert: k = XK_Insert; break;
    case Qt::Key_Home: k = XK_Home; break;
    case Qt::Key_End: k = XK_End; break;
    case Qt::Key_PageUp: k = XK_Page_Up; break;
    case Qt::Key_PageDown: k = XK_Page_Down; break;
    case Qt::Key_MediaPrevious: k = XK_Prior; break;
    case Qt::Key_MediaNext: k = XK_Next; break;
    case Qt::Key_MediaPlay: k = XK_Begin; break;

    /* Misc Functions */
    case Qt::Key_Select: k = XK_Select; break;
    case Qt::Key_Printer: k = XK_Print; break;
    case Qt::Key_Execute: k = XK_Execute; break;
    case Qt::Key_Undo: k = XK_Undo; break;
    case Qt::Key_Redo: k = XK_Redo; break;
    case Qt::Key_Menu: k = XK_Menu; break;
    case Qt::Key_Find: k = XK_Find; break;
    case Qt::Key_Exit:
    case Qt::Key_Cancel:
    case Qt::Key_Stop:
        k = XK_Cancel;
        break;
    case Qt::Key_Mode_switch: k = XK_Mode_switch; break;

    case Qt::Key_F1: k = XK_F1; break;
    case Qt::Key_F2: k = XK_F2; break;
    case Qt::Key_F3: k = XK_F3; break;
    case Qt::Key_F4: k = XK_F4; break;
    case Qt::Key_F5: k = XK_F5; break;
    case Qt::Key_F6: k = XK_F6; break;
    case Qt::Key_F7: k = XK_F7; break;
    case Qt::Key_F8: k = XK_F8; break;
    case Qt::Key_F9: k = XK_F9; break;
    case Qt::Key_F10: k = XK_F10; break;
    case Qt::Key_F11: k = XK_F11; break;
    case Qt::Key_F12: k = XK_F12; break;
    case Qt::Key_F13: k = XK_F13; break;
    case Qt::Key_F14: k = XK_F14; break;
    case Qt::Key_F15: k = XK_F15; break;
    case Qt::Key_F16: k = XK_F16; break;
    case Qt::Key_F17: k = XK_F17; break;
    case Qt::Key_F18: k = XK_F18; break;
    case Qt::Key_F19: k = XK_F19; break;
    case Qt::Key_F20: k = XK_F20; break;
    case Qt::Key_F21: k = XK_F21; break;
    case Qt::Key_F22: k = XK_F22; break;
    case Qt::Key_F23: k = XK_F23; break;
    case Qt::Key_F24: k = XK_F24; break;
    case Qt::Key_F25: k = XK_F25; break;
    case Qt::Key_F26: k = XK_F26; break;
    case Qt::Key_F27: k = XK_F27; break;
    case Qt::Key_F28: k = XK_F28; break;
    case Qt::Key_F29: k = XK_F29; break;
    case Qt::Key_F30: k = XK_F30; break;
    case Qt::Key_F31: k = XK_F31; break;
    case Qt::Key_F32: k = XK_F32; break;
    case Qt::Key_F33: k = XK_F33; break;
    case Qt::Key_F34: k = XK_F34; break;
    case Qt::Key_F35: k = XK_F35; break;

    case Qt::Key_NumLock: k = XK_Num_Lock; break;
    case Qt::Key_CapsLock: k = XK_Caps_Lock; break;
    case Qt::Key_ScrollLock: k = XK_Scroll_Lock; break;

    case Qt::Key_Shift: k = XK_Shift_L; break; //k = XK_Shift_R; break;
    case Qt::Key_Control: k = XK_Control_L; break;// k = XK_Control_R; break;
    case Qt::Key_Alt: k = XK_Alt_L; break;//k = XK_Alt_R; break;
    case Qt::Key_Meta: k = XK_Meta_L; break;//k = XK_Meta_R; break;*/

    case Qt::Key_Super_L: k = XK_Super_L; break;		/* left "windows" key */
    case Qt::Key_Super_R: k = XK_Super_R; break;		/* right "windows" key */

    case Qt::Key_Hyper_L: k = XK_Hyper_L; break;
    case Qt::Key_Hyper_R: k = XK_Hyper_R; break;

    case Qt::Key_Help: k = XK_Help; break;
    case Qt::Key_Print: k = XK_Print; break;
    case Qt::Key_SysReq: k = XK_Sys_Req; break;
    case Qt::Key_0: k = XK_0;break;
    case Qt::Key_1: k = XK_1;break;
    case Qt::Key_2: k = XK_2;break;
    case Qt::Key_3: k = XK_3;break;
    case Qt::Key_4: k = XK_4;break;
    case Qt::Key_5: k = XK_5;break;
    case Qt::Key_6: k = XK_6;break;
    case Qt::Key_7: k = XK_7;break;
    case Qt::Key_8: k = XK_8;break;
    case Qt::Key_9: k = XK_9;break;
    }

    if (k == 5000)
    {

        if (!modifier)
        {
            switch (inkey)
            {
            case Qt::Key_A: k = XK_a;break;
            case Qt::Key_B: k = XK_b;break;
            case Qt::Key_C: k = XK_c;break;
            case Qt::Key_D: k = XK_d;break;
            case Qt::Key_E: k = XK_e;break;
            case Qt::Key_F: k = XK_f;break;
            case Qt::Key_G: k = XK_g;break;
            case Qt::Key_H: k = XK_h;break;
            case Qt::Key_I: k = XK_i;break;
            case Qt::Key_J: k = XK_j;break;
            case Qt::Key_K: k = XK_k;break;
            case Qt::Key_L: k = XK_l;break;
            case Qt::Key_M: k = XK_m;break;
            case Qt::Key_N: k = XK_n;break;
            case Qt::Key_O: k = XK_o;break;
            case Qt::Key_P: k = XK_p;break;
            case Qt::Key_Q: k = XK_q;break;
            case Qt::Key_R: k = XK_r;break;
            case Qt::Key_S: k = XK_s;break;
            case Qt::Key_T: k = XK_t;break;
            case Qt::Key_U: k = XK_u;break;
            case Qt::Key_V: k = XK_v;break;
            case Qt::Key_W: k = XK_w;break;
            case Qt::Key_X: k = XK_x;break;
            case Qt::Key_Y: k = XK_y;break;
            case Qt::Key_Z: k = XK_z;break;
            }
        }
        else
        {
            switch (inkey)
            {
            case Qt::Key_A: k = XK_A;break;
            case Qt::Key_B: k = XK_B;break;
            case Qt::Key_C: k = XK_C;break;
            case Qt::Key_D: k = XK_D;break;
            case Qt::Key_E: k = XK_E;break;
            case Qt::Key_F: k = XK_F;break;
            case Qt::Key_G: k = XK_G;break;
            case Qt::Key_H: k = XK_H;break;
            case Qt::Key_I: k = XK_I;break;
            case Qt::Key_J: k = XK_J;break;
            case Qt::Key_K: k = XK_K;break;
            case Qt::Key_L: k = XK_L;break;
            case Qt::Key_M: k = XK_M;break;
            case Qt::Key_N: k = XK_N;break;
            case Qt::Key_O: k = XK_O;break;
            case Qt::Key_P: k = XK_P;break;
            case Qt::Key_Q: k = XK_Q;break;
            case Qt::Key_R: k = XK_R;break;
            case Qt::Key_S: k = XK_S;break;
            case Qt::Key_T: k = XK_T;break;
            case Qt::Key_U: k = XK_U;break;
            case Qt::Key_V: k = XK_V;break;
            case Qt::Key_W: k = XK_W;break;
            case Qt::Key_X: k = XK_X;break;
            case Qt::Key_Y: k = XK_Y;break;
            case Qt::Key_Z: k = XK_Z;break;
            }
        }
    }

    return k;

}
