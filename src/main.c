#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include "mymenu.h"
#include "text_processing.h"
#include "view_processing.h"
#include "scroll.h"
#include "caret.h"

#define FONT_SIZE 30 //размер шрифта

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("TextViewer");

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow) {
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the window class */

    /* The Window structure */
    wincl.hInstance = hThisInstance; /* This is the handle for our programm */
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;      /* Catch double-clicks | сохран€ть контекст устройства */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (hThisInstance, MAKEINTRESOURCE(IDM_ICON));
    wincl.hIconSm = LoadIcon (hThisInstance, MAKEINTRESOURCE(IDM_ICON));
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);

    wincl.lpszMenuName = (LPCSTR)MAKEINTRESOURCEW(IDM_MENU);      /* Menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */

    wincl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH); //белый цвет фона окна

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("TextViewer"),    /* Title Text */
           WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           544,                 /* The programs width */
           375,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           lpszArgument         /* Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0)) {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

/** \brief
 *  ‘ункци€, мен€юща€ шрифт на моноширинный и сохран€юща€ его размеры
 * \param HWND hwnd - дескриптор окна
 * \param VIEW* view - структура отображени€ текста
 *
 */
void ChangeFont(HFONT* font, HWND hwnd, VIEW* view) {
     TEXTMETRIC tm;
     HDC hdc = GetDC(hwnd);
     *font = CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "Courier New");

     SelectObject(hdc, *font);
     GetTextMetrics(hdc, &tm);
     view->charW = tm.tmAveCharWidth;
     view->charH = tm.tmHeight + tm.tmExternalLeading;
     ReleaseDC(hwnd, hdc);
}

/** \brief
 *  ‘ункци€, инициализирующа€ окно выбора файла
 * \param HWND hwnd - дескриптор окна
 * \param PTCHAR name - им€ файла
 *
 */
BOOL PopUpFileInitialize(HWND hwnd, PTCHAR name) {
    OPENFILENAME ofn;

    ZeroMemory(&ofn, sizeof(ofn));
    ZeroMemory(name, sizeof(TCHAR) * MAX_PATH);

    ofn.lStructSize = sizeof(ofn);                        //размер структуры
    ofn.hwndOwner = hwnd;                                 //владелец окна диалога
    ofn.lpstrFile = name;                                 //указатель на буфер дл€ имени файла
    ofn.nMaxFile = MAX_PATH;                              //размер буфера
    ofn.lpstrFilter = "All Files\0*.TXT;*.C;*.H;*.CPP\0"; //фильтр отбора файлов
    ofn.lpstrTitle = TEXT("Please, select the file");     //заголовок окна диалога
    ofn.lpstrInitialDir = NULL;                           //директори€ при открытии
    ofn.lpstrCustomFilter = NULL;                         //буфер дл€ определени€ шаблона
    ofn.nMaxCustFilter = 0;                               //размер буфера
    ofn.nFilterIndex = 0;                                 //текущий фильтр дл€ выбора типа файлов
    ofn.lpstrFileTitle = NULL;                            //буфер дл€ краткого имени файла
    ofn.nMaxFileTitle = 0;                                //размер буфера
    ofn.nFileOffset = 0;                                  //сдвиг от пути до краткого имени файла
    ofn.nFileExtension = 0;                               //сдвиг от пути до расширени€ файла
    ofn.lpstrDefExt = "txt";                              //расширение по умолчанию
    ofn.lCustData = 0L;                                   //данные, передающиес€ перехватывающей процедуре
    ofn.lpfnHook = NULL;                                  //перехватывающа€ процедура
    ofn.hInstance = NULL;                                 //модуль, в котором находитс€ шаблон
    ofn.lpTemplateName = NULL;                            //им€ шаблона
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
                                                          //параметры вида диалога

    return GetOpenFileName(&ofn);                         //вызов диалога дл€ выбора файла
}

/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static TEXT text;
    static VIEW view;
    static SCROLLS scrolls;
    static CARET caret;
    static ERROR_STATE error = SUCCESS;
    static HFONT font;

    switch (message) {                 /* handle the messages */
        case WM_DESTROY: {
            FreeAllMemory(&view, &text);
            DeleteObject((HGDIOBJ)(HFONT)(font));
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        }

        case WM_CREATE: {
            char* fileName = NULL;

            ChangeFont(&font, hwnd, &view);
            InitializeText(&text);
            InitializeView(hwnd, &view);
            InitializeScrolls(hwnd, &scrolls);
            SetZeroCaretPosition(&caret);

            CREATESTRUCT* myStruct = (CREATESTRUCT*)lParam;
            fileName = (char*)myStruct->lpCreateParams;
            view.textIsLoaded = FALSE;
            if (strlen(fileName)) {
                ReadText(&text, fileName, &error);
                ProcessErrorState(hwnd, szClassName, error);
                view.textIsLoaded = TRUE;
                UpdateViewForUsualMode(&view, &text);
                ResetScrollsPosition(hwnd, &view, &scrolls);

                SetCaretPos(view.charW * (HORZ_OFFSET_MULT + caret.horizontalPos), view.charH * caret.verticalPos);
                ShowCaret(hwnd);
            }
            break;
        }

        case WM_COMMAND: {
            HMENU hMenu = GetMenu(hwnd);
            switch(LOWORD(wParam)){
                case IDM_OPEN: {
                    char fileName[MAX_PATH];
                    if (!PopUpFileInitialize(hwnd, fileName)) {
                        SetCaretPos(view.charW * (HORZ_OFFSET_MULT + caret.horizontalPos), view.charH * caret.verticalPos);
                        break;
                    }
                    FreeAllMemory(&view, &text);
                    ReadText(&text, fileName, &error);
                    ProcessErrorState(hwnd, szClassName, error);
                    view.textIsLoaded = TRUE;

                    SetZeroTextPosition(&view);
                    if (view.mode == WITH_LAYOUT)
                        UpdateInfoForLayout(&text, &view, &error);
                    else
                        UpdateViewForUsualMode(&view, &text);

                    ResetScrollsPosition(hwnd, &view, &scrolls);
                    SetZeroCaretPosition(&caret);
                    SetCaretPos(view.charW * (HORZ_OFFSET_MULT + caret.horizontalPos), view.charH * caret.verticalPos);
                    ShowCaret(hwnd);
                    InvalidateRect(hwnd, 0, TRUE);
                    break;
                }
                case IDM_EXIT: {
                    SendMessage(hwnd, WM_CLOSE, 0, 0L);
                    break;
                }
                case IDM_USUAL: {
                    CheckMenuItem(hMenu, IDM_WITH_LAYOUT, MF_UNCHECKED);
                    CheckMenuItem(hMenu, IDM_USUAL, MF_CHECKED);
                    EnableMenuItem(hMenu, IDM_WITH_LAYOUT, MF_ENABLED);
                    EnableMenuItem(hMenu, IDM_USUAL, MF_DISABLED | MF_GRAYED);

                    view.mode = USUAL;
                    if (text.textStr) {
                        FindTextPositionForUsual(&text, &view);
                        FindCaretPositionForUsual(&caret, &text, &view);
                        SetCaretPos(view.charW * (HORZ_OFFSET_MULT + caret.horizontalPos), view.charH * caret.verticalPos);
                        UpdateViewForUsualMode(&view, &text);
                        ResetScrollsPosition(hwnd, &view, &scrolls);
                        InvalidateRect(hwnd, 0, TRUE);
                    }
                    break;
                }
                case IDM_WITH_LAYOUT: {
                    CheckMenuItem(hMenu, IDM_USUAL, MF_UNCHECKED);
                    CheckMenuItem(hMenu, IDM_WITH_LAYOUT, MF_CHECKED);
                    EnableMenuItem(hMenu, IDM_USUAL, MF_ENABLED);
                    EnableMenuItem(hMenu, IDM_WITH_LAYOUT, MF_DISABLED | MF_GRAYED);

                    view.mode = WITH_LAYOUT;
                    if (text.textStr) {
                        FillInfoForLayout(&text, &view, &error);
                        ProcessErrorState(hwnd, szClassName, error);

                        FindTextPositionForLayout(&text, &view);
                        ResetScrollsPosition(hwnd, &view, &scrolls);

                        FindCaretPositionForLayout(&caret, &text, &view);
                        SetCaretPos(view.charW * (HORZ_OFFSET_MULT + caret.horizontalPos), view.charH * caret.verticalPos);

                        InvalidateRect(hwnd, 0, TRUE);
                    }
                    break;
                }
                case IDM_ABOUT: {
                    MessageBox(hwnd, "This program is created by Vedeneeva Ekaterina,\n3rd year student of Applied Mathematics and Computer Science",
                               szClassName,  MB_OK | MB_ICONINFORMATION);
                    break;
                }
                case IDM_HELP: {
                    MessageBox(hwnd, "This is a text viewer program. You can select file and switch modes for viewing: usual and with layout.",
                               szClassName, MB_OK | MB_ICONQUESTION);
                    break;
                }
                default:
                    break;
            }
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            if (text.textStr && text.textLength != 0) {
                PrintText(hdc, &text, &view);
            }

            EndPaint(hwnd, &ps);
            break;
        }

        case WM_SIZE: {
            UpdateVeiwWindowSize(hwnd, &view);
            if (text.textStr) {
                if (view.mode == WITH_LAYOUT) {
                    UpdateInfoForLayoutAfterResize(&text, &view, &caret, &error);
                    ProcessErrorState(hwnd, szClassName, error);
                }

                unsigned int prevTextPosChar = view.curPosition.curCharNum;
                unsigned int prevTextPosLine = view.curPosition.curLineNum;
                MoveWhileResize(&view);

                caret.horizontalPos += (int)prevTextPosChar - (int)view.curPosition.curCharNum;
                caret.verticalPos += (int)prevTextPosLine - (int)view.curPosition.curLineNum;
                SetCaretPos(view.charW * (HORZ_OFFSET_MULT + caret.horizontalPos), view.charH * caret.verticalPos);
                ResetScrollsPosition(hwnd, &view, &scrolls);
                InvalidateRect(hwnd, 0, TRUE);
            }
            break;
        }

        case WM_VSCROLL: {
             if(!view.textIsLoaded)
                break;
             unsigned int prevTextPos = view.curPosition.curLineNum;

             switch (LOWORD(wParam)) {
                case SB_LINEUP:
                    ScrollLineUp(&view, &scrolls);
                    break;
                case SB_LINEDOWN:
                    ScrollLineDown(&view, &scrolls);
                    break;

                case SB_PAGEUP:
                    ScrollPageUp(&view, &scrolls);
                    break;
                case SB_PAGEDOWN:
                    ScrollPageDown(&view, &scrolls);
                    break;

                case SB_TOP:
		            ScrollToTop(&view, &scrolls);
		            break;
		        case SB_BOTTOM:
		            ScrollToBottom(&view, &scrolls);
		            break;

                case SB_THUMBTRACK:
                    ScrollThumbVertical(&view, &scrolls, HIWORD(wParam));
                    break;
                default:
                    break;
             }
             SetScrollPos(hwnd, SB_VERT, scrolls.vertical.pos, TRUE);
             caret.verticalPos += (int)prevTextPos - (int)view.curPosition.curLineNum;
             SetCaretPos(view.charW * (HORZ_OFFSET_MULT + caret.horizontalPos), view.charH * caret.verticalPos);
             InvalidateRect(hwnd, 0, TRUE);
             break;
        }

        case WM_HSCROLL: {
             if(!view.textIsLoaded)
                break;
             unsigned int prevTextPos = view.curPosition.curCharNum;

             switch (LOWORD(wParam)) {
                case SB_LINELEFT:
                    ScrollLineLeft(&view, &scrolls);
                    break;
                case SB_LINERIGHT:
                    ScrollLineRight(&view, &scrolls);
                    break;

                case SB_PAGELEFT:
                    ScrollPageLeft(&view, &scrolls);
                    break;
	    	    case SB_PAGERIGHT:
                    ScrollPageRight(&view, &scrolls);
                    break;

                case SB_LEFT:
                    ScrollToLeftEdge(&view, &scrolls);
                    break;
                case SB_RIGHT:
                    ScrollToRightEdge(&view, &scrolls);
                    break;

                case SB_THUMBTRACK:
                    ScrollThumbHorizontal(&view, &scrolls, HIWORD(wParam));
                    break;
                default:
                    break;
             }
             if (view.mode == WITH_LAYOUT && scrolls.horizontal.pos == 0) {
                    EnableScrollBar(hwnd, SB_HORZ, ESB_DISABLE_BOTH);
                    scrolls.horizontal.isActive = FALSE;
             }
             SetScrollPos(hwnd, SB_HORZ, scrolls.horizontal.pos, TRUE);
             caret.horizontalPos += (int)prevTextPos - (int)view.curPosition.curCharNum;
             SetCaretPos(view.charW * (HORZ_OFFSET_MULT + caret.horizontalPos), view.charH * caret.verticalPos);
             InvalidateRect(hwnd, 0, TRUE);
             break;
        }

        case WM_KEYDOWN: {
            if(!view.textIsLoaded)
                break;
            switch (LOWORD(wParam)) {
		        case VK_UP:
                    CaretUp(&caret, &text, &view, &scrolls, hwnd);
                    break;
		        case VK_DOWN:
                    CaretDown(&caret, &text, &view, &scrolls, hwnd);
			        break;
                case VK_PRIOR:
			        SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
			        break;
		        case VK_NEXT:
		        	SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
			        break;

		        case VK_LEFT:
                    CaretLeft(&caret, &text, &view, &scrolls, hwnd);
		        	break;
		        case VK_RIGHT:
			        CaretRight(&caret, &text, &view, &scrolls, hwnd);
			        break;
                default:
                    break;
	    	}
            SetCaretPos(view.charW * (HORZ_OFFSET_MULT + caret.horizontalPos), view.charH * caret.verticalPos);
            break;
        }

         case WM_LBUTTONDOWN: {
             float hCaretPos = (LOWORD(lParam) - 0.5 * view.charW) / (float)view.charW;
             float vCaretPos = HIWORD(lParam) / (float)view.charH;
             caret.horizontalPos = (hCaretPos - (int)hCaretPos < 0.5) ? (int)hCaretPos : (int)hCaretPos + 1;
             caret.verticalPos = (int)vCaretPos;
             caret.absoluteHorizontalPos = view.curPosition.curCharNum + caret.horizontalPos;
             caret.absoluteVerticalPos = view.curPosition.curLineNum + caret.verticalPos;

             if (caret.absoluteVerticalPos >= view.curTextSize.totalLinesNum) {
                    caret.absoluteVerticalPos = view.curTextSize.totalLinesNum - 1;
                    caret.verticalPos = (int)caret.absoluteVerticalPos - (int)view.curPosition.curLineNum;
             }
             FixCaretOutOfLineBounds(&caret, &text, &view, &scrolls, hwnd);
             SetCaretPos(view.charW * (HORZ_OFFSET_MULT + caret.horizontalPos), view.charH * caret.verticalPos);
             break;
         }

         case WM_SETFOCUS: {
            CreateCaret(hwnd, NULL, 1, FONT_SIZE);
            if (view.textIsLoaded)
                ShowCaret(hwnd);
            break;
        }

        case WM_KILLFOCUS: {
            HideCaret(hwnd);
            DestroyCaret();
            break;
        }

        default:                      /* for messages that we don't deal with */
            return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}
