#include "caret.h"

/** \brief
 *  Функция, выставляющая каретку в нулевую позицию
 * \param CARET* caret - структура каретки
 *
 */
void SetZeroCaretPosition(CARET* caret) {
     caret->horizontalPos = 0;
     caret->verticalPos = 0;
     caret->absoluteHorizontalPos = 0;
     caret->absoluteVerticalPos = 0;
}

/** \brief
 *  Функция, возвращающая каретку на последний символ строки, если при вертикальном перемещении она оказывается за ее пределами
 * \param CARET* caret - структура каретки
 * \param TEXT* text - структура текста
 * \param VIEW* view - структура отображения текста
 * \param SCROLLS* scrolls - структура скроллов
 * \param HWND hwnd - дескриптор окна
 *
 */
void FixCaretOutOfLineBounds(CARET* caret, TEXT* text, VIEW* view, SCROLLS* scrolls, HWND hwnd) {
    if (view->mode == USUAL && caret->absoluteHorizontalPos >
                                       text->linesPositions[caret->absoluteVerticalPos + 1] - text->linesPositions[caret->absoluteVerticalPos] - 2) {
        caret->absoluteHorizontalPos = text->linesPositions[caret->absoluteVerticalPos + 1] - text->linesPositions[caret->absoluteVerticalPos] - 2;
        caret->horizontalPos = (int)caret->absoluteHorizontalPos - (int)view->curPosition.curCharNum;
    }
    else if (view->mode == WITH_LAYOUT && caret->absoluteHorizontalPos >
              view->strPositionsWithLayout[caret->absoluteVerticalPos * 2 + 1] - view->strPositionsWithLayout[caret->absoluteVerticalPos * 2]) {
        caret->absoluteHorizontalPos =
              view->strPositionsWithLayout[caret->absoluteVerticalPos * 2 + 1] - view->strPositionsWithLayout[caret->absoluteVerticalPos* 2];
        caret->horizontalPos = (int)caret->absoluteHorizontalPos - (int)view->curPosition.curCharNum;
    }

    if (caret->horizontalPos <= 0) {
        view->curPosition.curCharNum = fmax((int)caret->absoluteHorizontalPos - (int)view->winW / (int)view->charW, 0);
        caret->horizontalPos = caret->absoluteHorizontalPos - view->curPosition.curCharNum;
        scrolls->horizontal.pos = scrolls->horizontal.coeff * view->curPosition.curCharNum;
        SetScrollPos(hwnd, SB_HORZ, scrolls->horizontal.pos, TRUE);
        InvalidateRect(hwnd, 0, TRUE);
        if (view->mode == WITH_LAYOUT && scrolls->horizontal.isActive && scrolls->horizontal.pos == 0) {
               EnableScrollBar(hwnd, SB_HORZ, ESB_DISABLE_BOTH);
               scrolls->horizontal.isActive = FALSE;
        }
    }
}

/** \brief
 *  Функция, проверяющая, находится ли каретка в пределах окна
 * \param CARET* caret - структура каретки
 * \param VIEW* view - структура отображения текста
 *
 */
boolean IsCaretOnScreen(CARET* caret, VIEW* view) {
    if (caret->verticalPos < 0 || caret->verticalPos > view->winH / view->charH - 1)
        return FALSE;
    if (caret->horizontalPos < 0 || caret->horizontalPos > view->winW / view->charW)
        return FALSE;
    return TRUE;
}

/** \brief
 *  Функция, перемещающая каретку на одну позицию вверх
 * \param CARET* caret - структура каретки
 * \param TEXT* text - структура текста
 * \param VIEW* view - структура отображения текста
 * \param SCROLLS* scrolls - структура скроллов
 * \param HWND hwnd - дескриптор окна
 *
 */
void CaretUp(CARET* caret, TEXT* text, VIEW* view, SCROLLS* scrolls, HWND hwnd) {
    //если каретка за пределами видимости, возвращаем ее в видимую область снизу
    if (!IsCaretOnScreen(caret, view)) {
        caret->verticalPos = fmin((int)view->winH / (int)view->charH - 1, (int)view->curTextSize.totalLinesNum - 1);
        caret->absoluteVerticalPos = view->curPosition.curLineNum + caret->verticalPos;
        if (caret->absoluteVerticalPos >= view->curTextSize.totalLinesNum) {
            caret->absoluteVerticalPos = view->curTextSize.totalLinesNum - 1;
            caret->verticalPos = (int)caret->absoluteVerticalPos - (int)view->curPosition.curLineNum;
        }
        caret->horizontalPos = view->winW / (view->charW * 2);
        caret->absoluteHorizontalPos = view->curPosition.curCharNum + caret->horizontalPos;
    }
    else if (caret->absoluteVerticalPos <= 0) {
        return;
    }
    else if (caret->verticalPos == 0) {
        caret->absoluteVerticalPos--;
        ScrollLineUp(view, scrolls);
        SetScrollPos(hwnd, SB_VERT, scrolls->vertical.pos, TRUE);
        InvalidateRect(hwnd, 0, TRUE);
    }
    else {
        caret->verticalPos--;
        caret->absoluteVerticalPos--;
    }
    FixCaretOutOfLineBounds(caret, text, view, scrolls, hwnd);
}

/** \brief
 *  Функция, перемещающая каретку на одну позицию вниз
 * \param CARET* caret - структура каретки
 * \param TEXT* text - структура текста
 * \param VIEW* view - структура отображения текста
 * \param SCROLLS* scrolls - структура скроллов
 * \param HWND hwnd - дескриптор окна
 *
 */
void CaretDown(CARET* caret, TEXT* text, VIEW* view, SCROLLS* scrolls, HWND hwnd) {
    //если каретка за пределами видимости, возвращаем ее в видимую область сверху
    if (!IsCaretOnScreen(caret, view)) {
        caret->verticalPos = 0;
        caret->absoluteVerticalPos = view->curPosition.curLineNum;
        caret->horizontalPos = view->winW / (view->charW * 2);
        caret->absoluteHorizontalPos = view->curPosition.curCharNum + caret->horizontalPos;
    }
    else if (caret->absoluteVerticalPos >= view->curTextSize.totalLinesNum - 1) {
        return;
    }
    else if (caret->verticalPos == view->winH / view->charH - 1) {
        caret->absoluteVerticalPos++;
        ScrollLineDown(view, scrolls);
        SetScrollPos(hwnd, SB_VERT, scrolls->vertical.pos, TRUE);
        InvalidateRect(hwnd, 0, TRUE);
    }
    else {
        caret->verticalPos++;
        caret->absoluteVerticalPos++;
    }
    FixCaretOutOfLineBounds(caret, text, view, scrolls, hwnd);
}

/** \brief
 *  Функция, перемещающая вывод текста к положению каретки
 *  (используется при перемещении каретки вправо/влево, если каретка находится за пределами текста)
 * \param CARET* caret - структура каретки
 * \param VIEW* view - структура отображения текста
 * \param SCROLLS* scrolls - структура скроллов
 * \param HWND hwnd - дескриптор окна
 *
 */
void ReturnTextToOldCaretPosition(CARET* caret, VIEW* view, SCROLLS* scrolls, HWND hwnd) {
    if (IsCaretOnScreen(caret, view))
        return;

    //устанавливаем строчку с кареткой второй сверху или как позволяет положение каретки в тексте
    view->curPosition.curLineNum = fmax((int)caret->absoluteVerticalPos - 1, 0);
    caret->verticalPos = (view->curPosition.curLineNum == 0) ? 0 : 1;
    if (view->curTextSize.totalLinesNum - view->curPosition.curLineNum < view->winH / view->charH) {
        view->curPosition.curLineNum = fmax((int)view->curTextSize.totalLinesNum - (int)view->winH / (int)view->charH, 0);
        caret->verticalPos = caret->absoluteVerticalPos - view->curPosition.curLineNum;
    }

    //устанавливаем место в тексте с кареткой в центре окна или как позволяет положение каретки в тексте
    view->curPosition.curCharNum = fmax((int)caret->absoluteHorizontalPos - (int)view->winW / ((int)view->charW * 2), 0);
    caret->horizontalPos = caret->absoluteHorizontalPos - view->curPosition.curCharNum;
    if (view->curTextSize.totalCharsNum - view->curPosition.curCharNum < view->winW / view->charW) {
        view->curPosition.curCharNum = fmax((int)view->curTextSize.totalCharsNum - (int)view->winW / (int)view->charW, 0);
        caret->horizontalPos = caret->absoluteHorizontalPos - view->curPosition.curCharNum;
    }

    //поправляем положение скроллов
    scrolls->horizontal.pos = scrolls->horizontal.coeff * view->curPosition.curCharNum;
    SetScrollPos(hwnd, SB_HORZ, scrolls->horizontal.pos, TRUE);
    scrolls->vertical.pos = scrolls->vertical.coeff * view->curPosition.curLineNum;
    SetScrollPos(hwnd, SB_VERT, scrolls->vertical.pos, TRUE);
    InvalidateRect(hwnd, 0, TRUE);
}

/** \brief
 *  Функция, перемещающая каретку на одну позицию вправо
 * \param CARET* caret - структура каретки
 * \param TEXT* text - структура текста
 * \param VIEW* view - структура отображения текста
 * \param SCROLLS* scrolls - структура скроллов
 * \param HWND hwnd - дескриптор окна
 *
 */
void CaretRight(CARET* caret, TEXT* text, VIEW* view, SCROLLS* scrolls, HWND hwnd) {
    //если каретка за пределами видимости, возвращаемся к ее предыдущему положению
    ReturnTextToOldCaretPosition(caret, view, scrolls, hwnd);

    //перескакиваем в начало следующей строки, если текущая закончилась
    if ((view->mode == USUAL
       && caret->absoluteHorizontalPos >= text->linesPositions[caret->absoluteVerticalPos + 1] - text->linesPositions[caret->absoluteVerticalPos] - 2) ||
        (view->mode == WITH_LAYOUT
       && caret->absoluteHorizontalPos >= view->strPositionsWithLayout[caret->absoluteVerticalPos * 2 + 1] - view->strPositionsWithLayout[caret->absoluteVerticalPos * 2])) {

        if (caret->absoluteVerticalPos == view->curTextSize.totalLinesNum - 1)
            return;

        if (caret->verticalPos >= view->winH / view->charH - 1) {
            caret->absoluteVerticalPos++;
            ScrollLineDown(view, scrolls);
            SetScrollPos(hwnd, SB_VERT, scrolls->vertical.pos, TRUE);
            InvalidateRect(hwnd, 0, TRUE);
        }
        else {
            caret->verticalPos++;
            caret->absoluteVerticalPos++;
        }
        caret->horizontalPos = 0;
        caret->absoluteHorizontalPos = 0;
        ScrollToLeftEdge(view, scrolls);
        SetScrollPos(hwnd, SB_HORZ, scrolls->horizontal.pos, TRUE);
        InvalidateRect(hwnd, 0, TRUE);
    }
    else if (caret->horizontalPos >= view->winW / view->charW) {
        caret->absoluteHorizontalPos++;
        ScrollLineRight(view, scrolls);
        SetScrollPos(hwnd, SB_HORZ, scrolls->horizontal.pos, TRUE);
        InvalidateRect(hwnd, 0, TRUE);
    }
    else {
	    caret->horizontalPos++;
        caret->absoluteHorizontalPos++;
    }


    if (view->mode == WITH_LAYOUT && scrolls->horizontal.isActive && scrolls->horizontal.pos == 0) {
              EnableScrollBar(hwnd, SB_HORZ, ESB_DISABLE_BOTH);
              scrolls->horizontal.isActive = FALSE;
    }
}

/** \brief
 *  Функция, перемещающая каретку на одну позицию влево
 * \param CARET* caret - структура каретки
 * \param TEXT* text - структура текста
 * \param VIEW* view - структура отображения текста
 * \param SCROLLS* scrolls - структура скроллов
 * \param HWND hwnd - дескриптор окна
 *
 */
void CaretLeft(CARET* caret, TEXT* text, VIEW* view, SCROLLS* scrolls, HWND hwnd) {
    //если каретка за пределами видимости, возвращаемся к ее предыдущему положению
    ReturnTextToOldCaretPosition(caret, view, scrolls, hwnd);

    //перескакиваем в конец предыдущей строки, если были на нулевой позиции
    if (caret->absoluteHorizontalPos <= 0) {
        if (caret->absoluteVerticalPos == 0)
            return;
        if (caret->verticalPos <= 0) {
            caret->absoluteVerticalPos--;
            ScrollLineUp(view, scrolls);
            SetScrollPos(hwnd, SB_VERT, scrolls->vertical.pos, TRUE);
            InvalidateRect(hwnd, 0, TRUE);
        }
        else {
            caret->verticalPos--;
            caret->absoluteVerticalPos--;
        }

        if (view->mode == USUAL)
            caret->absoluteHorizontalPos = text->linesPositions[caret->absoluteVerticalPos + 1]
                                              - text->linesPositions[caret->absoluteVerticalPos] - 2;
        else if (view->mode == WITH_LAYOUT)
            caret->absoluteHorizontalPos = view->strPositionsWithLayout[caret->absoluteVerticalPos * 2 + 1]
                                              - view->strPositionsWithLayout[caret->absoluteVerticalPos * 2];

        view->curPosition.curCharNum = fmax((int)caret->absoluteHorizontalPos - (int)view->winW / (int)view->charW, 0);
        caret->horizontalPos = caret->absoluteHorizontalPos - view->curPosition.curCharNum;
        scrolls->horizontal.pos = scrolls->horizontal.coeff * view->curPosition.curCharNum;
        SetScrollPos(hwnd, SB_HORZ, scrolls->horizontal.pos, TRUE);
        InvalidateRect(hwnd, 0, TRUE);
    }
    else if (caret->horizontalPos <= 0) {
        caret->absoluteHorizontalPos--;
        ScrollLineLeft(view, scrolls);
        SetScrollPos(hwnd, SB_HORZ, scrolls->horizontal.pos, TRUE);
        InvalidateRect(hwnd, 0, TRUE);
    }
    else {
        caret->horizontalPos--;
        caret->absoluteHorizontalPos--;
    }

    if (view->mode == WITH_LAYOUT && scrolls->horizontal.isActive && scrolls->horizontal.pos == 0) {
              EnableScrollBar(hwnd, SB_HORZ, ESB_DISABLE_BOTH);
              scrolls->horizontal.isActive = FALSE;
    }
}

/** \brief
 *  Функция, определяющая положение каретки при переходе к режиму вывода с версткой
 * \param CARET* caret - структура каретки
 * \param TEXT* text - структура текста
 * \param VIEW* view - структура отображения текста
 *
 */
void FindCaretPositionForLayout(CARET* caret, TEXT* text, VIEW* view) {
    unsigned int i = 0;
	unsigned int curAbsolutePosition = text->linesPositions[caret->absoluteVerticalPos] + caret->absoluteHorizontalPos;
	                 //номер буквы, перед которой находится каретка, в общем массиве текста

    for (i = 0; i < view->curTextSize.totalLinesNum * 2 - 1; i += 2) {
        //ищем в какую строчку попадает каретка
        if (curAbsolutePosition >= view->strPositionsWithLayout[(int)fmax(i - 1, 0)] &&
                       curAbsolutePosition <= view->strPositionsWithLayout[i + 1]) {
            caret->absoluteVerticalPos = i / 2;
            break;
        }
    }
    caret->absoluteHorizontalPos = (int)curAbsolutePosition - (int)view->strPositionsWithLayout[i];
    caret->verticalPos = (int)caret->absoluteVerticalPos - (int)view->curPosition.curLineNum;
    caret->horizontalPos = (int)caret->absoluteHorizontalPos - (int)view->curPosition.curCharNum;
}

/** \brief
 *  Функция, определяющая пооложение каретки при переходе к режиму вывода без верстки
 * \param CARET* caret - структура каретки
 * \param TEXT* text - структура текста
 * \param VIEW* view - структура отображения текста
 *
 */
void FindCaretPositionForUsual(CARET* caret, TEXT* text, VIEW* view) {
    unsigned int i = 0;
    unsigned int curAbsolutePosition = view->strPositionsWithLayout[caret->absoluteVerticalPos * 2] + caret->absoluteHorizontalPos;
                     //номер буквы, перед которой находится каретка, в общем массиве текста

    for (i = 0; i < text->linesNum; i++) {
        //ищем в какую строчку попадает каретка
        if (curAbsolutePosition >= text->linesPositions[i] &&
                       curAbsolutePosition < text->linesPositions[i + 1]) {
            caret->absoluteVerticalPos = i;
            break;
        }
    }
    caret->absoluteHorizontalPos = (int)curAbsolutePosition - (int)text->linesPositions[i];
    caret->verticalPos = (int)caret->absoluteVerticalPos - (int)view->curPosition.curLineNum;
    caret->horizontalPos = (int)caret->absoluteHorizontalPos - (int)view->curPosition.curCharNum;
}
