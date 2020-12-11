#include "ConvertKeyCode.h"
#include <QKeyEvent>

CConvertKeyCode::CConvertKeyCode()
{
    
}

/**
 * @brief CConvertKeyCode::QtToScanCode
 * @param key
 * @return 
 * @see [MS-RDPBCGR]: Remote Desktop Protocol: Basic Connectivity and Graphics
 *       Remoting 
 *           2.2.8.1.1.3.1.1.1
 *       https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpbcgr/5073f4ed-1e93-45e1-b039-6e30c385867c?redirectedfrom=MSDN
 */
UINT32 CConvertKeyCode::QtToScanCode(int key)
{
    UINT32 k = RDP_SCANCODE_UNKNOWN;
    switch (key)
    {
        case Qt::Key_Backspace: k = RDP_SCANCODE_BACKSPACE; break;
        case Qt::Key_Tab: k = RDP_SCANCODE_TAB;break;
        //case Qt::Key_Clear: k = XK_Clear; break;
        case Qt::Key_Return: k = RDP_SCANCODE_RETURN; break;
        //case Qt::Key_Pause: k = XK_Pause; break;
        case Qt::Key_Escape: k = RDP_SCANCODE_ESCAPE; break;
        case Qt::Key_Space: k = RDP_SCANCODE_SPACE; break;
        case Qt::Key_Delete: k = RDP_SCANCODE_DELETE; break;
        case Qt::Key_Period: k = RDP_SCANCODE_OEM_PERIOD; break;

        //special keyboard char
        /*
        case Qt::Key_Exclam: k = XK_exclam;break; //!
        case Qt::Key_QuoteDbl: k = XK_quotedbl;break; //?
        case Qt::Key_NumberSign: k = XK_numbersign;break; //#
        case Qt::Key_Percent: k = XK_percent;break; //%
        case Qt::Key_Dollar: k = XK_dollar;break;   //$
        case Qt::Key_Ampersand: k = XK_ampersand;break; //&
        case Qt::Key_Apostrophe: k = XK_apostrophe;break;//!
        case Qt::Key_ParenLeft: k = XK_parenleft;break;
        case Qt::Key_ParenRight: k = XK_parenright;break;

        case Qt::Key_Slash: k = XK_slash; break;    ///
        case Qt::Key_Asterisk: k = XK_asterisk; break;  //*
        case Qt::Key_Minus: k = XK_minus; break;    //-
        case Qt::Key_Plus: k = XK_plus; break;  //+
        case Qt::Key_Enter: k = XK_Return; break;   //
        case Qt::Key_Equal: k = XK_equal; break;    //=

        case Qt::Key_Colon: k = XK_colon;break;
        case Qt::Key_Semicolon: k = XK_semicolon; break;
        case Qt::Key_Greater: k = XK_greater; break;
        case Qt::Key_Question: k = XK_question; break;
        */
        case Qt::Key_At: k = RDP_SCANCODE_LMENU; break;

        /*
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
        case  Qt::Key_registered: k = XK_registered; break;
*/
        case Qt::Key_Up: k = XK_Up; break;
        case Qt::Key_Down: k = XK_Down; break;
        case Qt::Key_Right: k = XK_Right; break;
        case Qt::Key_Left: k = XK_Left; break;
        case Qt::Key_Insert: k = XK_Insert; break;
        case Qt::Key_Home: k = XK_Home; break;
        case Qt::Key_End: k = XK_End; break;
        case Qt::Key_PageUp: k = XK_Page_Up; break;
        case Qt::Key_PageDown: k = XK_Page_Down; break;
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
        case Qt::Key_F12: k =  XK_F12; break;
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
        case Qt::Key_Shift: k = XK_Shift_R; break; //k = XK_Shift_L; break;
        case Qt::Key_Control: k = XK_Control_R; break;// k = XK_Control_L; break;
        case Qt::Key_Alt: k = XK_Alt_R; break;//k = XK_Alt_L; break;
        case Qt::Key_Meta: k = XK_Meta_R; break;//k = XK_Meta_L; break;*/

        case Qt::Key_Super_L: k = XK_Super_L; break;		/* left "windows" key */
        case Qt::Key_Super_R: k = XK_Super_R; break;		/* right "windows" key */

        case Qt::Key_Mode_switch: k = XK_Mode_switch; break;
        case Qt::Key_Help: k = XK_Help; break;
        case Qt::Key_Print: k = XK_Print; break;
        case Qt::Key_SysReq: k = XK_Sys_Req; break;
        case Qt::Key_0: k = RDP_SCANCODE_KEY_0;break;
        case Qt::Key_1: k = RDP_SCANCODE_KEY_1;break;
        case Qt::Key_2: k = RDP_SCANCODE_KEY_2;break;
        case Qt::Key_3: k = RDP_SCANCODE_KEY_3;break;
        case Qt::Key_4: k = RDP_SCANCODE_KEY_4;break;
        case Qt::Key_5: k = RDP_SCANCODE_KEY_5;break;
        case Qt::Key_6: k = RDP_SCANCODE_KEY_6;break;
        case Qt::Key_7: k = RDP_SCANCODE_KEY_7;break;
        case Qt::Key_8: k = RDP_SCANCODE_KEY_8;break;
        case Qt::Key_9: k = RDP_SCANCODE_KEY_9;break;
    }
    return k;
}
