// Author: Kang Lin <kl222@126.com>
//! 
#include "ConvertKeyCode.h"

CConvertKeyCode::CConvertKeyCode()
{}

/**
 * @brief CConvertKeyCode::QtToScanCode
 * @param key
 * @return 
 * @see [MS-RDPBCGR]: Remote Desktop Protocol: Basic Connectivity and Graphics
 *       Remoting 
 *           2.2.8.1.1.3.1.1.1
 *       https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpbcgr/7acaec9f-c8a6-4ee9-87d6-d9b89cf56489
 */
UINT32 CConvertKeyCode::QtToScanCode(int key, Qt::KeyboardModifiers modifiers)
{
    UINT32 k = RDP_SCANCODE_UNKNOWN;
    switch (key)
    {
    case Qt::Key_Escape: return RDP_SCANCODE_ESCAPE;
    case Qt::Key_Tab: return RDP_SCANCODE_TAB;
    case Qt::Key_Backtab: return k;
    case Qt::Key_Backspace: return RDP_SCANCODE_BACKSPACE;
    case Qt::Key_Return: return RDP_SCANCODE_RETURN; //大键盘Enter对应Qt::Key_Return
    case Qt::Key_Enter: return RDP_SCANCODE_RETURN; //小键盘Enter对应Qt::Key_Enter
    case Qt::Key_Insert: return RDP_SCANCODE_INSERT;
    case Qt::Key_Delete: return RDP_SCANCODE_DELETE;
    case Qt::Key_Pause: return RDP_SCANCODE_PAUSE;
    case Qt::Key_Print: return RDP_SCANCODE_PRINTSCREEN;
    case  Qt::Key_SysReq: return RDP_SCANCODE_SYSREQ;
        //case Qt::Key_Clear: return RDP_SCANCODE_UNKNOWN;
    case Qt::Key_Home: return RDP_SCANCODE_HOME;
    case Qt::Key_End: return RDP_SCANCODE_END;
    case Qt::Key_Left: return RDP_SCANCODE_LEFT;
    case Qt::Key_Up: return RDP_SCANCODE_UP;
    case Qt::Key_Right: return RDP_SCANCODE_RIGHT;
    case Qt::Key_Down: return RDP_SCANCODE_DOWN;
    case Qt::Key_PageUp: return RDP_SCANCODE_PRIOR;
    case Qt::Key_PageDown: return RDP_SCANCODE_NEXT;
    case Qt::Key_Shift: return RDP_SCANCODE_LSHIFT;
    case Qt::Key_Control: return RDP_SCANCODE_LCONTROL;
        //case Qt::Key_Meta: return RDP_SCANCODE_UNKNOWN;
    case Qt::Key_Alt: return RDP_SCANCODE_LMENU;
    case Qt::Key_AltGr: return RDP_SCANCODE_RMENU;
    case Qt::Key_CapsLock: return RDP_SCANCODE_CAPSLOCK;
    case Qt::Key_NumLock: return RDP_SCANCODE_NUMLOCK;
    case Qt::Key_ScrollLock: return RDP_SCANCODE_SCROLLLOCK;
    case Qt::Key_F1: return RDP_SCANCODE_F1;
    case Qt::Key_F2: return RDP_SCANCODE_F2;
    case Qt::Key_F3: return RDP_SCANCODE_F3;
    case Qt::Key_F4: return RDP_SCANCODE_F4;
    case Qt::Key_F5: return RDP_SCANCODE_F5;
    case Qt::Key_F6: return RDP_SCANCODE_F6;
    case Qt::Key_F7: return RDP_SCANCODE_F7;
    case Qt::Key_F8: return RDP_SCANCODE_F8;
    case Qt::Key_F9: return RDP_SCANCODE_F9;
    case Qt::Key_F10: return RDP_SCANCODE_F10;
    case Qt::Key_F11: return RDP_SCANCODE_F11;
    case Qt::Key_F12: return RDP_SCANCODE_F12;
    case Qt::Key_F13: return RDP_SCANCODE_F13;
    case Qt::Key_F14: return RDP_SCANCODE_F14;
    case Qt::Key_F15: return RDP_SCANCODE_F15;
    case Qt::Key_F16: return RDP_SCANCODE_F16;
    case Qt::Key_F17: return RDP_SCANCODE_F17;
    case Qt::Key_F18: return RDP_SCANCODE_F18;
    case Qt::Key_F19: return RDP_SCANCODE_F19;
    case Qt::Key_F20: return RDP_SCANCODE_F20;
    case Qt::Key_F21: return RDP_SCANCODE_F20;
    case Qt::Key_F22: return RDP_SCANCODE_F21;
    case Qt::Key_F23: return RDP_SCANCODE_F22;
    case Qt::Key_F24: return RDP_SCANCODE_F23;
        /*
        Qt::Key_F25
        Qt::Key_F26
        Qt::Key_F27
        Qt::Key_F28
        Qt::Key_F29
        Qt::Key_F30
        Qt::Key_F31
        Qt::Key_F32
        Qt::Key_F33
        Qt::Key_F34
        Qt::Key_F35
        */
    case Qt::Key_Super_L: return RDP_SCANCODE_LWIN;
    case Qt::Key_Super_R: return RDP_SCANCODE_RWIN;
    case Qt::Key_Menu: return RDP_SCANCODE_LMENU;
        // Qt::Key_Hyper_L
        // Qt::Key_Hyper_R
    case Qt::Key_Help: return RDP_SCANCODE_HELP;
        // Qt::Key_Direction_L
        // Qt::Key_Direction_R
    case Qt::Key_Space: return RDP_SCANCODE_SPACE;
        //case Qt::Key_Any: return RDP_SCANCODE_SPACE;
    case Qt::Key_Exclam: return RDP_SCANCODE_KEY_1;       // !
    case Qt::Key_QuoteDbl:return RDP_SCANCODE_OEM_7;      // "
    case Qt::Key_NumberSign: return RDP_SCANCODE_KEY_3;   // #
    case Qt::Key_Dollar: return RDP_SCANCODE_KEY_4;       // $
    case Qt::Key_Percent: return RDP_SCANCODE_KEY_5;      // %
    case Qt::Key_Ampersand: return RDP_SCANCODE_KEY_7;    // &
    case Qt::Key_Apostrophe: return RDP_SCANCODE_OEM_7;   // '
    case Qt::Key_ParenLeft: return RDP_SCANCODE_KEY_9;    // (
    case Qt::Key_ParenRight: return RDP_SCANCODE_KEY_0;   // )
    case Qt::Key_Asterisk:                                // *
    {
        if(modifiers & Qt::ShiftModifier)
            return RDP_SCANCODE_KEY_8;
        return RDP_SCANCODE_MULTIPLY;  
    }
    case Qt::Key_Plus:                                    // +
    {
        if(modifiers & Qt::ShiftModifier)
            return RDP_SCANCODE_OEM_PLUS;
        return RDP_SCANCODE_ADD;
    }
    case Qt::Key_Comma: return RDP_SCANCODE_OEM_COMMA;    // ,
    case Qt::Key_Minus: return RDP_SCANCODE_OEM_MINUS;    // -
    case Qt::Key_Period: return RDP_SCANCODE_OEM_PERIOD;  // .
    case Qt::Key_Slash: return RDP_SCANCODE_OEM_2;        // '/'

    case Qt::Key_0: return RDP_SCANCODE_KEY_0;
    case Qt::Key_1: return RDP_SCANCODE_KEY_1;
    case Qt::Key_2: return RDP_SCANCODE_KEY_2;
    case Qt::Key_3: return RDP_SCANCODE_KEY_3;
    case Qt::Key_4: return RDP_SCANCODE_KEY_4;
    case Qt::Key_5: return RDP_SCANCODE_KEY_5;
    case Qt::Key_6: return RDP_SCANCODE_KEY_6;
    case Qt::Key_7: return RDP_SCANCODE_KEY_7;
    case Qt::Key_8: return RDP_SCANCODE_KEY_8;
    case Qt::Key_9: return RDP_SCANCODE_KEY_9;
        
    case Qt::Key_Colon: return RDP_SCANCODE_OEM_1;        // :
    case Qt::Key_Semicolon: return RDP_SCANCODE_OEM_1;    // ;
    case Qt::Key_Less: return RDP_SCANCODE_OEM_COMMA;     // <
    case Qt::Key_Equal: return RDP_SCANCODE_OEM_PLUS;     // =
    case Qt::Key_Greater: return RDP_SCANCODE_OEM_PERIOD; // >
    case Qt::Key_Question: return RDP_SCANCODE_OEM_2;     // ?
    case Qt::Key_At: return RDP_SCANCODE_KEY_2;           // @
        
    case Qt::Key_A: return RDP_SCANCODE_KEY_A;
    case Qt::Key_B: return RDP_SCANCODE_KEY_B;
    case Qt::Key_C: return RDP_SCANCODE_KEY_C;
    case Qt::Key_D: return RDP_SCANCODE_KEY_D;
    case Qt::Key_E: return RDP_SCANCODE_KEY_E;
    case Qt::Key_F: return RDP_SCANCODE_KEY_F;
    case Qt::Key_G: return RDP_SCANCODE_KEY_G;
    case Qt::Key_H: return RDP_SCANCODE_KEY_H;
    case Qt::Key_I: return RDP_SCANCODE_KEY_I;
    case Qt::Key_J: return RDP_SCANCODE_KEY_J;
    case Qt::Key_K: return RDP_SCANCODE_KEY_K;
    case Qt::Key_L: return RDP_SCANCODE_KEY_L;
    case Qt::Key_M: return RDP_SCANCODE_KEY_M;
    case Qt::Key_N: return RDP_SCANCODE_KEY_N;
    case Qt::Key_O: return RDP_SCANCODE_KEY_O;
    case Qt::Key_P: return RDP_SCANCODE_KEY_P;
    case Qt::Key_Q: return RDP_SCANCODE_KEY_Q;
    case Qt::Key_R: return RDP_SCANCODE_KEY_R;
    case Qt::Key_S: return RDP_SCANCODE_KEY_S;
    case Qt::Key_T: return RDP_SCANCODE_KEY_T;
    case Qt::Key_U: return RDP_SCANCODE_KEY_U;
    case Qt::Key_V: return RDP_SCANCODE_KEY_V;
    case Qt::Key_W: return RDP_SCANCODE_KEY_W;
    case Qt::Key_X: return RDP_SCANCODE_KEY_X;
    case Qt::Key_Y: return RDP_SCANCODE_KEY_Y;
    case Qt::Key_Z: return RDP_SCANCODE_KEY_Z;
    case Qt::Key_BracketLeft: return RDP_SCANCODE_OEM_4;
    case Qt::Key_Backslash: return RDP_SCANCODE_OEM_5;
    case Qt::Key_BracketRight: return RDP_SCANCODE_OEM_6;
    case Qt::Key_AsciiCircum: return RDP_SCANCODE_KEY_6;
    case Qt::Key_Underscore: return RDP_SCANCODE_OEM_MINUS;
    case Qt::Key_QuoteLeft: return RDP_SCANCODE_OEM_3;
    case Qt::Key_BraceLeft: return RDP_SCANCODE_OEM_4;
    case Qt::Key_Bar: return RDP_SCANCODE_OEM_5;
    case Qt::Key_BraceRight: return RDP_SCANCODE_OEM_6;
    case Qt::Key_AsciiTilde: return RDP_SCANCODE_OEM_3;
        /* Qt::Key_nobreakspace
        Qt::Key_exclamdown
        Qt::Key_cent
        Qt::Key_sterling
        Qt::Key_currency
        Qt::Key_yen
        Qt::Key_brokenbar
        Qt::Key_section
        Qt::Key_diaeresis
        Qt::Key_copyright
        Qt::Key_ordfeminine
        Qt::Key_guillemotleft
        Qt::Key_notsign
        Qt::Key_hyphen
        Qt::Key_registered
        Qt::Key_macron
        Qt::Key_degree
        Qt::Key_plusminus
        Qt::Key_twosuperior
        Qt::Key_threesuperior
        Qt::Key_acute
        Qt::Key_mu
        Qt::Key_paragraph
        Qt::Key_periodcentered
        Qt::Key_cedilla
        Qt::Key_onesuperior
        Qt::Key_masculine
        Qt::Key_guillemotright
        Qt::Key_onequarter
        Qt::Key_onehalf
        Qt::Key_threequarters
        Qt::Key_questiondown
        Qt::Key_Agrave
        Qt::Key_Aacute
        Qt::Key_Acircumflex
        Qt::Key_Atilde
        Qt::Key_Adiaeresis
        Qt::Key_Aring
        Qt::Key_AE
        Qt::Key_Ccedilla
        Qt::Key_Egrave
        Qt::Key_Eacute
        Qt::Key_Ecircumflex
        Qt::Key_Ediaeresis
        Qt::Key_Igrave
        Qt::Key_Iacute
        Qt::Key_Icircumflex
        Qt::Key_Idiaeresis
        Qt::Key_ETH
        Qt::Key_Ntilde
        Qt::Key_Ograve
        Qt::Key_Oacute
        Qt::Key_Ocircumflex
        Qt::Key_Otilde
        Qt::Key_Odiaeresis
        Qt::Key_multiply
        Qt::Key_Ooblique
        Qt::Key_Ugrave
        Qt::Key_Uacute
        Qt::Key_Ucircumflex
        Qt::Key_Udiaeresis
        Qt::Key_Yacute
        Qt::Key_THORN
        Qt::Key_ssharp
        Qt::Key_division
        Qt::Key_ydiaeresis
        Qt::Key_Multi_key
        Qt::Key_Codeinput
        Qt::Key_SingleCandidate
        Qt::Key_MultipleCandidate
        Qt::Key_PreviousCandidate
        Qt::Key_Mode_switch
        Qt::Key_Kanji
        Qt::Key_Muhenkan
        Qt::Key_Henkan
        Qt::Key_Romaji
        Qt::Key_Hiragana
        Qt::Key_Katakana
        Qt::Key_Hiragana_Katakana
        Qt::Key_Zenkaku
        Qt::Key_Hankaku
        Qt::Key_Zenkaku_Hankaku
        Qt::Key_Touroku
        Qt::Key_Massyo
        Qt::Key_Kana_Lock
        Qt::Key_Kana_Shift
        Qt::Key_Eisu_Shift
        Qt::Key_Eisu_toggle
        Qt::Key_Hangul
        Qt::Key_Hangul_Start
        Qt::Key_Hangul_End
        Qt::Key_Hangul_Hanja
        Qt::Key_Hangul_Jamo
        Qt::Key_Hangul_Romaja
        Qt::Key_Hangul_Jeonja
        Qt::Key_Hangul_Banja
        Qt::Key_Hangul_PreHanja
        Qt::Key_Hangul_PostHanja
        Qt::Key_Hangul_Special
        Qt::Key_Dead_Grave
        Qt::Key_Dead_Acute
        Qt::Key_Dead_Circumflex
        Qt::Key_Dead_Tilde
        Qt::Key_Dead_Macron
        Qt::Key_Dead_Breve
        Qt::Key_Dead_Abovedot
        Qt::Key_Dead_Diaeresis
        Qt::Key_Dead_Abovering
        Qt::Key_Dead_Doubleacute
        Qt::Key_Dead_Caron
        Qt::Key_Dead_Cedilla
        Qt::Key_Dead_Ogonek
        Qt::Key_Dead_Iota
        Qt::Key_Dead_Voiced_Sound
        Qt::Key_Dead_Semivoiced_Sound
        Qt::Key_Dead_Belowdot
        Qt::Key_Dead_Hook
        Qt::Key_Dead_Horn
        Qt::Key_Dead_Stroke
        Qt::Key_Dead_Abovecomma
        Qt::Key_Dead_Abovereversedcomma
        Qt::Key_Dead_Doublegrave
        Qt::Key_Dead_Belowring
        Qt::Key_Dead_Belowmacron
        Qt::Key_Dead_Belowcircumflex
        Qt::Key_Dead_Belowtilde
        Qt::Key_Dead_Belowbreve
        Qt::Key_Dead_Belowdiaeresis
        Qt::Key_Dead_Invertedbreve
        Qt::Key_Dead_Belowcomma
        Qt::Key_Dead_Currency
        Qt::Key_Dead_a
        Qt::Key_Dead_A
        Qt::Key_Dead_e
        Qt::Key_Dead_E
        Qt::Key_Dead_i
        Qt::Key_Dead_I
        Qt::Key_Dead_o
        Qt::Key_Dead_O
        Qt::Key_Dead_u
        Qt::Key_Dead_U
        Qt::Key_Dead_Small_Schwa
        Qt::Key_Dead_Capital_Schwa
        Qt::Key_Dead_Greek
        Qt::Key_Dead_Lowline
        Qt::Key_Dead_Aboveverticalline
        Qt::Key_Dead_Belowverticalline
        Qt::Key_Dead_Longsolidusoverlay
        Qt::Key_Back
        Qt::Key_Forward
        Qt::Key_Stop
        Qt::Key_Refresh
        Qt::Key_VolumeDown
        Qt::Key_VolumeMute
        Qt::Key_VolumeUp
        Qt::Key_BassBoost
        Qt::Key_BassUp
        Qt::Key_BassDown
        Qt::Key_TrebleUp
        Qt::Key_TrebleDown
        Qt::Key_MediaPlay
        Qt::Key_MediaStop
        Qt::Key_MediaPrevious
        Qt::Key_MediaNext
        Qt::Key_MediaRecord
        Qt::Key_MediaPause
        Qt::Key_MediaTogglePlayPause
        Qt::Key_HomePage
        Qt::Key_Favorites
        Qt::Key_Search
        Qt::Key_Standby
        Qt::Key_OpenUrl
        Qt::Key_LaunchMail
        Qt::Key_LaunchMedia
        Qt::Key_Launch0
        Qt::Key_Launch1
        Qt::Key_Launch2
        Qt::Key_Launch3
        Qt::Key_Launch4
        Qt::Key_Launch5
        Qt::Key_Launch6
        Qt::Key_Launch7
        Qt::Key_Launch8
        Qt::Key_Launch9
        Qt::Key_LaunchA
        Qt::Key_LaunchB
        Qt::Key_LaunchC
        Qt::Key_LaunchD
        Qt::Key_LaunchE
        Qt::Key_LaunchF
        Qt::Key_LaunchG
        Qt::Key_LaunchH
        Qt::Key_MonBrightnessUp
        Qt::Key_MonBrightnessDown
        Qt::Key_KeyboardLightOnOff
        Qt::Key_KeyboardBrightnessUp
        Qt::Key_KeyboardBrightnessDown
        Qt::Key_PowerOff
        Qt::Key_WakeUp
        Qt::Key_Eject
        Qt::Key_ScreenSaver
        Qt::Key_WWW
        Qt::Key_Memo
        Qt::Key_LightBulb
        Qt::Key_Shop
        Qt::Key_History
        Qt::Key_AddFavorite
        Qt::Key_HotLinks
        Qt::Key_BrightnessAdjust
        Qt::Key_Finance
        Qt::Key_Community
        Qt::Key_AudioRewind
        Qt::Key_BackForward
        Qt::Key_ApplicationLeft
        Qt::Key_ApplicationRight
        Qt::Key_Book
        Qt::Key_CD
        Qt::Key_Calculator
        Qt::Key_ToDoList
        Qt::Key_ClearGrab
        Qt::Key_Close
        Qt::Key_Copy
        Qt::Key_Cut
        Qt::Key_Display
        Qt::Key_DOS
        Qt::Key_Documents
        Qt::Key_Excel
        Qt::Key_Explorer
        Qt::Key_Game
        Qt::Key_Go
        Qt::Key_iTouch
        Qt::Key_LogOff
        Qt::Key_Market
        Qt::Key_Meeting
        Qt::Key_MenuKB
        Qt::Key_MenuPB
        Qt::Key_MySites
        Qt::Key_News
        Qt::Key_OfficeHome
        Qt::Key_Option
        Qt::Key_Paste
        Qt::Key_Phone
        Qt::Key_Calendar
        Qt::Key_Reply
        Qt::Key_Reload
        Qt::Key_RotateWindows
        Qt::Key_RotationPB
        Qt::Key_RotationKB
        Qt::Key_Save
        Qt::Key_Send
        Qt::Key_Spell
        Qt::Key_SplitScreen
        Qt::Key_Support
        Qt::Key_TaskPane
        Qt::Key_Terminal
        Qt::Key_Tools
        Qt::Key_Travel
        Qt::Key_Video
        Qt::Key_Word
        Qt::Key_Xfer
        Qt::Key_ZoomIn
        Qt::Key_ZoomOut
        Qt::Key_Away
        Qt::Key_Messenger
        Qt::Key_WebCam
        Qt::Key_MailForward
        Qt::Key_Pictures
        Qt::Key_Music
        Qt::Key_Battery
        Qt::Key_Bluetooth
        Qt::Key_WLAN
        Qt::Key_UWB
        Qt::Key_AudioForward
        Qt::Key_AudioRepeat
        Qt::Key_AudioRandomPlay
        Qt::Key_Subtitle
        Qt::Key_AudioCycleTrack
        Qt::Key_Time
        Qt::Key_Hibernate
        Qt::Key_View
        Qt::Key_TopMenu
        Qt::Key_PowerDown
        Qt::Key_Suspend
        Qt::Key_ContrastAdjust
        Qt::Key_TouchpadToggle
        Qt::Key_TouchpadOn
        Qt::Key_TouchpadOff
        Qt::Key_MicMute
        Qt::Key_Red
        Qt::Key_Green
        Qt::Key_Yellow
        Qt::Key_Blue
        Qt::Key_ChannelUp
        Qt::Key_ChannelDown
        Qt::Key_Guide
        Qt::Key_Info
        Qt::Key_Settings
        Qt::Key_MicVolumeUp
        Qt::Key_MicVolumeDown
        Qt::Key_New
        Qt::Key_Open
        Qt::Key_Find
        Qt::Key_Undo
        Qt::Key_Redo
        Qt::Key_MediaLast
        Qt::Key_unknown
        Qt::Key_Call
        Qt::Key_Camera
        Qt::Key_CameraFocus
        Qt::Key_Context1
        Qt::Key_Context2
        Qt::Key_Context3
        Qt::Key_Context4
        Qt::Key_Flip
        Qt::Key_Hangup
        Qt::Key_No
        Qt::Key_Select
        Qt::Key_Yes
        Qt::Key_ToggleCallHangup
        Qt::Key_VoiceDial
        Qt::Key_LastNumberRedial
        Qt::Key_Execute
        Qt::Key_Printer
        Qt::Key_Play
        Qt::Key_Sleep
        Qt::Key_Zoom
        Qt::Key_Exit
        Qt::Key_Cancel
        //*/
        //case Qt::Key_Clear: k = XK_Clear; break;
    }
    return k;
}
