#include "caret.h"

/** \brief
 *  �������, ������������ ������� � ������� �������
 * \param CARET* caret - ��������� �������
 *
 */
void SetZeroCaretPosition(CARET* caret) {
     caret->horizontalPos = 0;
     caret->verticalPos = 0;
     caret->absoluteHorizontalPos = 0;
     caret->absoluteVerticalPos = 0;
}

/** \brief
 *  �������, ������������ ������� �� ��������� ������ ������, ���� ��� ������������ ����������� ��� ����������� �� �� ���������
 * \param CARET* caret - ��������� �������
 * \param TEXT* text - ��������� ������
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 * \param HWND hwnd - ���������� ����
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
 *  �������, �����������, ��������� �� ������� � �������� ����
 * \param CARET* caret - ��������� �������
 * \param VIEW* view - ��������� ����������� ������
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
 *  �������, ������������ ������� �� ���� ������� �����
 * \param CARET* caret - ��������� �������
 * \param TEXT* text - ��������� ������
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 * \param HWND hwnd - ���������� ����
 *
 */
void CaretUp(CARET* caret, TEXT* text, VIEW* view, SCROLLS* scrolls, HWND hwnd) {
    //���� ������� �� ��������� ���������, ���������� �� � ������� ������� �����
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
 *  �������, ������������ ������� �� ���� ������� ����
 * \param CARET* caret - ��������� �������
 * \param TEXT* text - ��������� ������
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 * \param HWND hwnd - ���������� ����
 *
 */
void CaretDown(CARET* caret, TEXT* text, VIEW* view, SCROLLS* scrolls, HWND hwnd) {
    //���� ������� �� ��������� ���������, ���������� �� � ������� ������� ������
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
 *  �������, ������������ ����� ������ � ��������� �������
 *  (������������ ��� ����������� ������� ������/�����, ���� ������� ��������� �� ��������� ������)
 * \param CARET* caret - ��������� �������
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 * \param HWND hwnd - ���������� ����
 *
 */
void ReturnTextToOldCaretPosition(CARET* caret, VIEW* view, SCROLLS* scrolls, HWND hwnd) {
    if (IsCaretOnScreen(caret, view))
        return;

    //������������� ������� � �������� ������ ������ ��� ��� ��������� ��������� ������� � ������
    view->curPosition.curLineNum = fmax((int)caret->absoluteVerticalPos - 1, 0);
    caret->verticalPos = (view->curPosition.curLineNum == 0) ? 0 : 1;
    if (view->curTextSize.totalLinesNum - view->curPosition.curLineNum < view->winH / view->charH) {
        view->curPosition.curLineNum = fmax((int)view->curTextSize.totalLinesNum - (int)view->winH / (int)view->charH, 0);
        caret->verticalPos = caret->absoluteVerticalPos - view->curPosition.curLineNum;
    }

    //������������� ����� � ������ � �������� � ������ ���� ��� ��� ��������� ��������� ������� � ������
    view->curPosition.curCharNum = fmax((int)caret->absoluteHorizontalPos - (int)view->winW / ((int)view->charW * 2), 0);
    caret->horizontalPos = caret->absoluteHorizontalPos - view->curPosition.curCharNum;
    if (view->curTextSize.totalCharsNum - view->curPosition.curCharNum < view->winW / view->charW) {
        view->curPosition.curCharNum = fmax((int)view->curTextSize.totalCharsNum - (int)view->winW / (int)view->charW, 0);
        caret->horizontalPos = caret->absoluteHorizontalPos - view->curPosition.curCharNum;
    }

    //���������� ��������� ��������
    scrolls->horizontal.pos = scrolls->horizontal.coeff * view->curPosition.curCharNum;
    SetScrollPos(hwnd, SB_HORZ, scrolls->horizontal.pos, TRUE);
    scrolls->vertical.pos = scrolls->vertical.coeff * view->curPosition.curLineNum;
    SetScrollPos(hwnd, SB_VERT, scrolls->vertical.pos, TRUE);
    InvalidateRect(hwnd, 0, TRUE);
}

/** \brief
 *  �������, ������������ ������� �� ���� ������� ������
 * \param CARET* caret - ��������� �������
 * \param TEXT* text - ��������� ������
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 * \param HWND hwnd - ���������� ����
 *
 */
void CaretRight(CARET* caret, TEXT* text, VIEW* view, SCROLLS* scrolls, HWND hwnd) {
    //���� ������� �� ��������� ���������, ������������ � �� ����������� ���������
    ReturnTextToOldCaretPosition(caret, view, scrolls, hwnd);

    //������������� � ������ ��������� ������, ���� ������� �����������
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
 *  �������, ������������ ������� �� ���� ������� �����
 * \param CARET* caret - ��������� �������
 * \param TEXT* text - ��������� ������
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 * \param HWND hwnd - ���������� ����
 *
 */
void CaretLeft(CARET* caret, TEXT* text, VIEW* view, SCROLLS* scrolls, HWND hwnd) {
    //���� ������� �� ��������� ���������, ������������ � �� ����������� ���������
    ReturnTextToOldCaretPosition(caret, view, scrolls, hwnd);

    //������������� � ����� ���������� ������, ���� ���� �� ������� �������
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
 *  �������, ������������ ��������� ������� ��� �������� � ������ ������ � ��������
 * \param CARET* caret - ��������� �������
 * \param TEXT* text - ��������� ������
 * \param VIEW* view - ��������� ����������� ������
 *
 */
void FindCaretPositionForLayout(CARET* caret, TEXT* text, VIEW* view) {
    unsigned int i = 0;
	unsigned int curAbsolutePosition = text->linesPositions[caret->absoluteVerticalPos] + caret->absoluteHorizontalPos;
	                 //����� �����, ����� ������� ��������� �������, � ����� ������� ������

    for (i = 0; i < view->curTextSize.totalLinesNum * 2 - 1; i += 2) {
        //���� � ����� ������� �������� �������
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
 *  �������, ������������ ���������� ������� ��� �������� � ������ ������ ��� �������
 * \param CARET* caret - ��������� �������
 * \param TEXT* text - ��������� ������
 * \param VIEW* view - ��������� ����������� ������
 *
 */
void FindCaretPositionForUsual(CARET* caret, TEXT* text, VIEW* view) {
    unsigned int i = 0;
    unsigned int curAbsolutePosition = view->strPositionsWithLayout[caret->absoluteVerticalPos * 2] + caret->absoluteHorizontalPos;
                     //����� �����, ����� ������� ��������� �������, � ����� ������� ������

    for (i = 0; i < text->linesNum; i++) {
        //���� � ����� ������� �������� �������
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
