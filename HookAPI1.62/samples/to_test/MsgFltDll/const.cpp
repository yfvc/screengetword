#include "stdafx.h"
#include "const.h"

MSG_INFO g_msg_info[] =
{
    { "WM_NULL", WM_NULL},                                   // 0x0000
    { "WM_CREATE", WM_CREATE},                               // 0x0001
    { "WM_DESTROY", WM_DESTROY},                             // 0x0002
    { "WM_MOVE", WM_MOVE},                                   // 0x0003
    { "WM_SIZE", WM_SIZE},                                   // 0x0005
    { "WM_ACTIVATE", WM_ACTIVATE},                           // 0x0006
    { "WM_SETFOCUS", WM_SETFOCUS},                           // 0x0007
    { "WM_KILLFOCUS", WM_KILLFOCUS},                         // 0x0008
    { "WM_ENABLE", WM_ENABLE},                               // 0x000A
    { "WM_SETREDRAW", WM_SETREDRAW},                         // 0x000B
    { "WM_SETTEXT", WM_SETTEXT},                             // 0x000C
    { "WM_GETTEXT", WM_GETTEXT},                             // 0x000D
    { "WM_GETTEXTLENGTH", WM_GETTEXTLENGTH},                 // 0x000E
    { "WM_PAINT", WM_PAINT},                                 // 0x000F
    { "WM_CLOSE", WM_CLOSE},                                 // 0x0010
    { "WM_QUERYENDSESSION", WM_QUERYENDSESSION},             // 0x0011
    { "WM_QUIT", WM_QUIT},                                   // 0x0012
    { "WM_QUERYOPEN", WM_QUERYOPEN},                         // 0x0013
    { "WM_ERASEBKGND", WM_ERASEBKGND},                       // 0x0014
    { "WM_SYSCOLORCHANGE", WM_SYSCOLORCHANGE},               // 0x0015
    { "WM_ENDSESSION", WM_ENDSESSION},                       // 0x0016
    { "WM_SHOWWINDOW", WM_SHOWWINDOW},                       // 0x0018
    { "WM_WININICHANGE", WM_WININICHANGE},                   // 0x001A
    { "WM_DEVMODECHANGE", WM_DEVMODECHANGE},                 // 0x001B
    { "WM_ACTIVATEAPP", WM_ACTIVATEAPP},                     // 0x001C
    { "WM_FONTCHANGE", WM_FONTCHANGE},                       // 0x001D
    { "WM_TIMECHANGE", WM_TIMECHANGE},                       // 0x001E
    { "WM_CANCELMODE", WM_CANCELMODE},                       // 0x001F
    { "WM_SETCURSOR", WM_SETCURSOR},                         // 0x0020
    { "WM_MOUSEACTIVATE", WM_MOUSEACTIVATE},                 // 0x0021
    { "WM_CHILDACTIVATE", WM_CHILDACTIVATE},                 // 0x0022
    { "WM_QUEUESYNC", WM_QUEUESYNC},                         // 0x0023
    { "WM_GETMINMAXINFO", WM_GETMINMAXINFO},                 // 0x0024
    { "WM_PAINTICON", WM_PAINTICON},                         // 0x0026
    { "WM_ICONERASEBKGND", WM_ICONERASEBKGND},               // 0x0027
    { "WM_NEXTDLGCTL", WM_NEXTDLGCTL},                       // 0x0028
    { "WM_SPOOLERSTATUS", WM_SPOOLERSTATUS},                 // 0x002A
    { "WM_DRAWITEM", WM_DRAWITEM},                           // 0x002B
    { "WM_MEASUREITEM", WM_MEASUREITEM},                     // 0x002C
        
    { "WM_DELETEITEM", WM_DELETEITEM},                       // 0x002D
        
    { "WM_VKEYTOITEM", WM_VKEYTOITEM},                       // 0x002E
     
    { "WM_CHARTOITEM", WM_CHARTOITEM},                       // 0x002F
     
    { "WM_SETFONT", WM_SETFONT},                             // 0x0030
        
    { "WM_GETFONT", WM_GETFONT},                             // 0x0031
        
    { "WM_SETHOTKEY", WM_SETHOTKEY},                         // 0x0032
     
    { "WM_GETHOTKEY", WM_GETHOTKEY},                         // 0x0033
     
    { "WM_QUERYDRAGICON", WM_QUERYDRAGICON},                 // 0x0037
        
    { "WM_COMPAREITEM", WM_COMPAREITEM},                     // 0x0039
        
    { "WM_COMPACTING", WM_COMPACTING},                       // 0x0041
        
    { "WM_WINDOWPOSCHANGING", WM_WINDOWPOSCHANGING},         // 0x0046
        
    { "WM_WINDOWPOSCHANGED", WM_WINDOWPOSCHANGED},           // 0x0047
        
    { "WM_POWER", WM_POWER},                                 // 0x0048
        
    { "WM_COPYDATA", WM_COPYDATA},                           // 0x004A
        
    { "WM_CANCELJOURNAL", WM_CANCELJOURNAL},                 // 0x004B
        
    { "WM_NCCREATE", WM_NCCREATE},                           // 0x0081
    
    { "WM_NCDESTROY", WM_NCDESTROY},                         // 0x0082
    
    { "WM_NCCALCSIZE", WM_NCCALCSIZE},                       // 0x0083
    
    { "WM_NCHITTEST", WM_NCHITTEST},                         // 0x0084
    
    { "WM_NCPAINT", WM_NCPAINT},                             // 0x0085
    
    { "WM_NCACTIVATE", WM_NCACTIVATE},                       // 0x0086
    
    { "WM_GETDLGCODE", WM_GETDLGCODE},                       // 0x0087
        
    { "WM_NCMOUSEMOVE", WM_NCMOUSEMOVE},                     // 0x00A0
    
    { "WM_NCLBUTTONDOWN", WM_NCLBUTTONDOWN},                 // 0x00A1
    
    { "WM_NCLBUTTONUP", WM_NCLBUTTONUP},                     // 0x00A2
    
    { "WM_NCLBUTTONDBLCLK", WM_NCLBUTTONDBLCLK},             // 0x00A3
    
    { "WM_NCRBUTTONDOWN", WM_NCRBUTTONDOWN},                 // 0x00A4
    
    { "WM_NCRBUTTONUP", WM_NCRBUTTONUP},                     // 0x00A5
    
    { "WM_NCRBUTTONDBLCLK", WM_NCRBUTTONDBLCLK},             // 0x00A6
    
    { "WM_NCMBUTTONDOWN", WM_NCMBUTTONDOWN},                 // 0x00A7
    
    { "WM_NCMBUTTONUP", WM_NCMBUTTONUP},                     // 0x00A8
    
    { "WM_NCMBUTTONDBLCLK", WM_NCMBUTTONDBLCLK},             // 0x00A9
    
    { "EM_GETSEL", EM_GETSEL},                               // 0x00B0
    
    { "EM_SETSEL", EM_SETSEL},                               // 0x00B1
    
    { "EM_GETRECT", EM_GETRECT},                             // 0x00B2
    
    { "EM_SETRECT", EM_SETRECT},                             // 0x00B3
    
    { "EM_SETRECTNP", EM_SETRECTNP},                         // 0x00B4
    
    { "EM_SCROLL", EM_SCROLL},                               // 0x00B5
    
    { "EM_LINESCROLL", EM_LINESCROLL},                       // 0x00B6
    
    { "EM_SCROLLCARET", EM_SCROLLCARET},                     // 0x00B7
    
    { "EM_GETMODIFY", EM_GETMODIFY},                         // 0x00B8
    
    { "EM_SETMODIFY", EM_SETMODIFY},                         // 0x00B9
    
    { "EM_GETLINECOUNT", EM_GETLINECOUNT},                   // 0x00BA
    
    { "EM_LINEINDEX", EM_LINEINDEX},                         // 0x00BB
    
    { "EM_SETHANDLE", EM_SETHANDLE},                         // 0x00BC
    
    { "EM_GETHANDLE", EM_GETHANDLE},                         // 0x00BD
    
    { "EM_GETTHUMB", EM_GETTHUMB},                           // 0x00BE
    
    { "EM_LINELENGTH", EM_LINELENGTH},                       // 0x00C1
    
    { "EM_REPLACESEL", EM_REPLACESEL},                       // 0x00C2
    
    { "EM_GETLINE", EM_GETLINE},                             // 0x00C4
    
    { "EM_LIMITTEXT", EM_LIMITTEXT},                         // 0x00C5
    
    { "EM_CANUNDO", EM_CANUNDO},                             // 0x00C6
    
    { "EM_UNDO", EM_UNDO},                                   // 0x00C7
    
    { "EM_FMTLINES", EM_FMTLINES},                           // 0x00C8
    
    { "EM_LINEFROMCHAR", EM_LINEFROMCHAR},                   // 0x00C9
    
    { "EM_SETTABSTOPS", EM_SETTABSTOPS},                     // 0x00CB
    
    { "EM_SETPASSWORDCHAR", EM_SETPASSWORDCHAR},             // 0x00CC
    
    { "EM_EMPTYUNDOBUFFER", EM_EMPTYUNDOBUFFER},             // 0x00CD
    
    { "EM_GETFIRSTVISIBLELINE", EM_GETFIRSTVISIBLELINE},     // 0x00CE
    
    { "EM_SETREADONLY", EM_SETREADONLY},                     // 0x00CF
    
    { "EM_SETWORDBREAKPROC", EM_SETWORDBREAKPROC},           // 0x00D0
    
    { "EM_GETWORDBREAKPROC", EM_GETWORDBREAKPROC},           // 0x00D1
    
    { "EM_GETPASSWORDCHAR", EM_GETPASSWORDCHAR},             // 0x00D2
    
    { "SBM_SETPOS", SBM_SETPOS},                             // 0x00E0
        
    { "SBM_GETPOS", SBM_GETPOS},                             // 0x00E1
        
    { "SBM_SETRANGE", SBM_SETRANGE},                         // 0x00E2
        
    { "SBM_GETRANGE", SBM_GETRANGE},                         // 0x00E3
        
    { "SBM_ENABLE_ARROWS", SBM_ENABLE_ARROWS},               // 0x00E4
        
    { "SBM_SETRANGEREDRAW", SBM_SETRANGEREDRAW},             // 0x00E6
        
    { "BM_GETCHECK", BM_GETCHECK},                           // 0x00F0
    
    { "BM_SETCHECK", BM_SETCHECK},                           // 0x00F1
    
    { "BM_GETSTATE", BM_GETSTATE},                           // 0x00F2
    
    { "BM_SETSTATE", BM_SETSTATE},                           // 0x00F3
    
    { "BM_SETSTYLE", BM_SETSTYLE},                           // 0x00F4
    
    { "WM_KEYDOWN", WM_KEYDOWN},                             // 0x0100
     
    { "WM_KEYUP", WM_KEYUP},                                 // 0x0101
     
    { "WM_CHAR", WM_CHAR},                                   // 0x0102
     
    { "WM_DEADCHAR", WM_DEADCHAR},                           // 0x0103
     
    { "WM_SYSKEYDOWN", WM_SYSKEYDOWN},                       // 0x0104
     
    { "WM_SYSKEYUP", WM_SYSKEYUP},                           // 0x0105
     
    { "WM_SYSCHAR", WM_SYSCHAR},                             // 0x0106
     
    { "WM_SYSDEADCHAR", WM_SYSDEADCHAR},                     // 0x0107
     
    { "WM_WNT_CONVERTREQUESTEX", 0x0109},   // 0x0109
    
    { "WM_CONVERTREQUEST", 0x010A},               // 0x010A
    
    { "WM_CONVERTRESULT", 0x010B},                 // 0x010B
    
    { "WM_INTERIM", 0x010C},                                   // 0x010C
    
    { "WM_IME_STARTCOMPOSITION", WM_IME_STARTCOMPOSITION},   // 0x010D
    
    { "WM_IME_ENDCOMPOSITION",   WM_IME_ENDCOMPOSITION},     // 0x010E
    
    { "WM_IME_COMPOSITION",      WM_IME_COMPOSITION},        // 0x010F
    
    { "WM_INITDIALOG", WM_INITDIALOG},                       // 0x0110
        
    { "WM_COMMAND", WM_COMMAND},                             // 0x0111
        
    { "WM_SYSCOMMAND", WM_SYSCOMMAND},                       // 0x0112
        
    { "WM_TIMER", WM_TIMER},                                 // 0x0113
        
    { "WM_HSCROLL", WM_HSCROLL},                             // 0x0114
        
    { "WM_VSCROLL", WM_VSCROLL},                             // 0x0115
        
    { "WM_INITMENU", WM_INITMENU},                           // 0x0116
        
    { "WM_INITMENUPOPUP", WM_INITMENUPOPUP},                 // 0x0117
        
    { "WM_MENUSELECT", WM_MENUSELECT},                       // 0x011F
        
    { "WM_MENUCHAR", WM_MENUCHAR},                           // 0x0120
        
    { "WM_ENTERIDLE", WM_ENTERIDLE},                         // 0x0121
        
    { "WM_CTLCOLORMSGBOX", WM_CTLCOLORMSGBOX},               // 0x0132
        
    { "WM_CTLCOLOREDIT", WM_CTLCOLOREDIT},                   // 0x0133
        
    { "WM_CTLCOLORLISTBOX", WM_CTLCOLORLISTBOX},             // 0x0134
        
    { "WM_CTLCOLORBTN", WM_CTLCOLORBTN},                     // 0x0135
        
    { "WM_CTLCOLORDLG", WM_CTLCOLORDLG},                     // 0x0136
        
    { "WM_CTLCOLORSCROLLBAR", WM_CTLCOLORSCROLLBAR},         // 0x0137
        
    { "WM_CTLCOLORSTATIC", WM_CTLCOLORSTATIC},               // 0x0138
        
    { "CB_GETEDITSEL", CB_GETEDITSEL},                       // 0x0140
    
    { "CB_LIMITTEXT", CB_LIMITTEXT},                         // 0x0141
    
    { "CB_SETEDITSEL", CB_SETEDITSEL},                       // 0x0142
    
    { "CB_ADDSTRING", CB_ADDSTRING},                         // 0x0143
    
    { "CB_DELETESTRING", CB_DELETESTRING},                   // 0x0144
    
    { "CB_DIR", CB_DIR},                                     // 0x0145
    
    { "CB_GETCOUNT", CB_GETCOUNT},                           // 0x0146
    
    { "CB_GETCURSEL", CB_GETCURSEL},                         // 0x0147
    
    { "CB_GETLBTEXT", CB_GETLBTEXT},                         // 0x0148
    
    { "CB_GETLBTEXTLEN", CB_GETLBTEXTLEN},                   // 0x0149
    
    { "CB_INSERTSTRING", CB_INSERTSTRING},                   // 0x014A
    
    { "CB_RESETCONTENT", CB_RESETCONTENT},                   // 0x014B
    
    { "CB_FINDSTRING", CB_FINDSTRING},                       // 0x014C
    
    { "CB_SELECTSTRING", CB_SELECTSTRING},                   // 0x014D
    
    { "CB_SETCURSEL", CB_SETCURSEL},                         // 0x014E
    
    { "CB_SHOWDROPDOWN", CB_SHOWDROPDOWN},                   // 0x014F
    
    { "CB_GETITEMDATA", CB_GETITEMDATA},                     // 0x0150
    
    { "CB_SETITEMDATA", CB_SETITEMDATA},                     // 0x0151
    
    { "CB_GETDROPPEDCONTROLRECT", CB_GETDROPPEDCONTROLRECT}, // 0x0152
    
    { "CB_SETITEMHEIGHT", CB_SETITEMHEIGHT},                 // 0x0153
    
    { "CB_GETITEMHEIGHT", CB_GETITEMHEIGHT},                 // 0x0154
    
    { "CB_SETEXTENDEDUI", CB_SETEXTENDEDUI},                 // 0x0155
    
    { "CB_GETEXTENDEDUI", CB_GETEXTENDEDUI},                 // 0x0156
    
    { "CB_GETDROPPEDSTATE", CB_GETDROPPEDSTATE},             // 0x0157
    
    { "CB_FINDSTRINGEXACT", CB_FINDSTRINGEXACT},             // 0x0158
    
    { "CB_SETLOCALE", CB_SETLOCALE},                         // 0x0159
    
    { "CB_GETLOCALE", CB_GETLOCALE},                         // 0x015A
    
    { "STM_SETICON", STM_SETICON},                           // 0x0170
    
    { "STM_GETICON", STM_GETICON},                           // 0x0171
    
    { "LB_ADDSTRING", LB_ADDSTRING},                         // 0x0180
    
    { "LB_INSERTSTRING", LB_INSERTSTRING},                   // 0x0181
    
    { "LB_DELETESTRING", LB_DELETESTRING},                   // 0x0182
    
    { "LB_SELITEMRANGEEX", LB_SELITEMRANGEEX},               // 0x0183
    
    { "LB_RESETCONTENT", LB_RESETCONTENT},                   // 0x0184
    
    { "LB_SETSEL", LB_SETSEL},                               // 0x0185
    
    { "LB_SETCURSEL", LB_SETCURSEL},                         // 0x0186
    
    { "LB_GETSEL", LB_GETSEL},                               // 0x0187
    
    { "LB_GETCURSEL", LB_GETCURSEL},                         // 0x0188
    
    { "LB_GETTEXT", LB_GETTEXT},                             // 0x0189
    
    { "LB_GETTEXTLEN", LB_GETTEXTLEN},                       // 0x018A
    
    { "LB_GETCOUNT", LB_GETCOUNT},                           // 0x018B
    
    { "LB_SELECTSTRING", LB_SELECTSTRING},                   // 0x018C
    
    { "LB_DIR", LB_DIR},                                     // 0x018D
    
    { "LB_GETTOPINDEX", LB_GETTOPINDEX},                     // 0x018E
    
    { "LB_FINDSTRING", LB_FINDSTRING},                       // 0x018F
    
    { "LB_GETSELCOUNT", LB_GETSELCOUNT},                     // 0x0190
    
    { "LB_GETSELITEMS", LB_GETSELITEMS},                     // 0x0191
    
    { "LB_SETTABSTOPS", LB_SETTABSTOPS},                     // 0x0192
    
    { "LB_GETHORIZONTALEXTENT", LB_GETHORIZONTALEXTENT},     // 0x0193
    
    { "LB_SETHORIZONTALEXTENT", LB_SETHORIZONTALEXTENT},     // 0x0194
    
    { "LB_SETCOLUMNWIDTH", LB_SETCOLUMNWIDTH},               // 0x0195
    
    { "LB_ADDFILE", LB_ADDFILE},                             // 0x0196
    
    { "LB_SETTOPINDEX", LB_SETTOPINDEX},                     // 0x0197
    
    { "LB_GETITEMRECT", LB_GETITEMRECT},                     // 0x0198
    
    { "LB_GETITEMDATA", LB_GETITEMDATA},                     // 0x0199
    
    { "LB_SETITEMDATA", LB_SETITEMDATA},                     // 0x019A
    
    { "LB_SELITEMRANGE", LB_SELITEMRANGE},                   // 0x019B
    
    { "LB_SETANCHORINDEX", LB_SETANCHORINDEX},               // 0x019C
    
    { "LB_GETANCHORINDEX", LB_GETANCHORINDEX},               // 0x019D
    
    { "LB_SETCARETINDEX", LB_SETCARETINDEX},                 // 0x019E
    
    { "LB_GETCARETINDEX", LB_GETCARETINDEX},                 // 0x019F
    
    { "LB_SETITEMHEIGHT", LB_SETITEMHEIGHT},                 // 0x01A0
    
    { "LB_GETITEMHEIGHT", LB_GETITEMHEIGHT},                 // 0x01A1
    
    { "LB_FINDSTRINGEXACT", LB_FINDSTRINGEXACT},             // 0x01A2
    
    { "LB_SETLOCALE", LB_SETLOCALE},                         // 0x01A5
    
    { "LB_GETLOCALE", LB_GETLOCALE},                         // 0x01A6
    
    { "LB_SETCOUNT", LB_SETCOUNT},                           // 0x01A7
    
    { "WM_MOUSEMOVE", WM_MOUSEMOVE},                         // 0x0200
    
    { "WM_LBUTTONDOWN", WM_LBUTTONDOWN},                     // 0x0201
    
    { "WM_LBUTTONUP", WM_LBUTTONUP},                         // 0x0202
    
    { "WM_LBUTTONDBLCLK", WM_LBUTTONDBLCLK},                 // 0x0203
    
    { "WM_RBUTTONDOWN", WM_RBUTTONDOWN},                     // 0x0204
    
    { "WM_RBUTTONUP", WM_RBUTTONUP},                         // 0x0205
    
    { "WM_RBUTTONDBLCLK", WM_RBUTTONDBLCLK},                 // 0x0206
    
    { "WM_MBUTTONDOWN", WM_MBUTTONDOWN},                     // 0x0207
    
    { "WM_MBUTTONUP", WM_MBUTTONUP},                         // 0x0208
    
    { "WM_MBUTTONDBLCLK", WM_MBUTTONDBLCLK},                 // 0x0209
    
    { "WM_PARENTNOTIFY", WM_PARENTNOTIFY},                   // 0x0210
    
    { "WM_ENTERMENULOOP", WM_ENTERMENULOOP},                 // 0x0211
        
    { "WM_EXITMENULOOP", WM_EXITMENULOOP},                   // 0x0212
        
    { "WM_MDICREATE", WM_MDICREATE},                         // 0x0220
        
    { "WM_MDIDESTROY", WM_MDIDESTROY},                       // 0x0221
        
    { "WM_MDIACTIVATE", WM_MDIACTIVATE},                     // 0x0222
        
    { "WM_MDIRESTORE", WM_MDIRESTORE},                       // 0x0223
        
    { "WM_MDINEXT", WM_MDINEXT},                             // 0x0224
        
    { "WM_MDIMAXIMIZE", WM_MDIMAXIMIZE},                     // 0x0225
        
    { "WM_MDITILE", WM_MDITILE},                             // 0x0226
        
    { "WM_MDICASCADE", WM_MDICASCADE},                       // 0x0227
        
    { "WM_MDIICONARRANGE", WM_MDIICONARRANGE},               // 0x0228
        
    { "WM_MDIGETACTIVE", WM_MDIGETACTIVE},                   // 0x0229
        
    { "WM_MDISETMENU", WM_MDISETMENU},                       // 0x0230
        
    { "WM_ENTERSIZEMOVE", WM_ENTERSIZEMOVE},                 // 0x0231
        
    { "WM_EXITSIZEMOVE", WM_EXITSIZEMOVE},                   // 0x0232
        
    { "WM_DROPFILES", WM_DROPFILES},                         // 0x0233
        
    { "WM_MDIREFRESHMENU", WM_MDIREFRESHMENU},               // 0x0234
        
    { "WM_IME_REPORT", 0x0280},                              // 0x0280
    { "WM_IME_SETCONTEXT",      WM_IME_SETCONTEXT},          // 0x0281
    
    { "WM_IME_NOTIFY",          WM_IME_NOTIFY},              // 0x0282
    
    { "WM_IME_CONTROL",         WM_IME_CONTROL},             // 0x0283
    
    { "WM_IME_COMPOSITIONFULL", WM_IME_COMPOSITIONFULL},     // 0x0284
    
    { "WM_IME_SELECT",          WM_IME_SELECT},              // 0x0285
    
    { "WM_IME_CHAR",            WM_IME_CHAR},                // 0x0286
    
    { "WM_IMEKEYDOWN", 0x0290},                       // 0x0290
    { "WM_IMEKEYUP", 0x0291},                           // 0x0291
    { "WM_CUT", WM_CUT},                                     // 0x0300
    
    { "WM_COPY", WM_COPY},                                   // 0x0301
    
    { "WM_PASTE", WM_PASTE},                                 // 0x0302
    
    { "WM_CLEAR", WM_CLEAR},                                 // 0x0303
    
    { "WM_UNDO", WM_UNDO},                                   // 0x0304
    
    { "WM_RENDERFORMAT", WM_RENDERFORMAT},                   // 0x0305
    
    { "WM_RENDERALLFORMATS", WM_RENDERALLFORMATS},           // 0x0306
    
    { "WM_DESTROYCLIPBOARD", WM_DESTROYCLIPBOARD},           // 0x0307
    
    { "WM_DRAWCLIPBOARD", WM_DRAWCLIPBOARD},                 // 0x0308
    
    { "WM_PAINTCLIPBOARD", WM_PAINTCLIPBOARD},               // 0x0309
    
    { "WM_VSCROLLCLIPBOARD", WM_VSCROLLCLIPBOARD},           // 0x030A
    
    { "WM_SIZECLIPBOARD", WM_SIZECLIPBOARD},                 // 0x030B
    
    { "WM_ASKCBFORMATNAME", WM_ASKCBFORMATNAME},             // 0x030C
    
    { "WM_CHANGECBCHAIN", WM_CHANGECBCHAIN},                 // 0x030D
    
    { "WM_HSCROLLCLIPBOARD", WM_HSCROLLCLIPBOARD},           // 0x030E
    
    { "WM_QUERYNEWPALETTE", WM_QUERYNEWPALETTE},             // 0x030F
        
    { "WM_PALETTEISCHANGING", WM_PALETTEISCHANGING},         // 0x0310
        
    { "WM_PALETTECHANGED", WM_PALETTECHANGED},               // 0x0311
        
    { "WM_HOTKEY", WM_HOTKEY},                               // 0x0312
     
    { "WM_DDE_INITIATE", 0x03E0},                   // 0x03E0
     
    { "WM_DDE_TERMINATE", 0x03E1},                 // 0x03E1
     
    { "WM_DDE_ADVISE", 0x03E2},                       // 0x03E2
     
    { "WM_DDE_UNADVISE", 0x03E3},                   // 0x03E3
     
    { "WM_DDE_ACK", 0x03E4},                             // 0x03E4
     
    { "WM_DDE_DATA", 0x03E5},                           // 0x03E5
     
    { "WM_DDE_REQUEST", 0x03E6},                     // 0x03E6
     
    { "WM_DDE_POKE", 0x03E7},                           // 0x03E7
     
    { "WM_DDE_EXECUTE", 0x03E8},                     // 0x03E8

};

int g_msg_count =sizeof(g_msg_info)/sizeof(MSG_INFO);
