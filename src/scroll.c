#include "scroll.h"

/** \brief
 *  �������, ���������������� �������
 * \param HWND hwnd - ���������� ����
 * \param SCROLLS* scrolls - ��������� ��������
 * \param VIEW* view - ��������� ����������� ������
 *
 */
void InitializeScrolls(HWND hwnd, SCROLLS *scrolls) {
    scrolls->horizontal.pos = 0;
    scrolls->vertical.pos = 0;
    EnableScrollBar(hwnd, SB_HORZ, ESB_DISABLE_BOTH);
    scrolls->horizontal.isActive = FALSE;
    EnableScrollBar(hwnd, SB_VERT, ESB_DISABLE_BOTH);
    scrolls->vertical.isActive = FALSE;
}

/** \brief
 *  �������, ����������� ��������� �������
 * \param HWND hwnd - ���������� ����
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 *
 */
void ResetScrollsPosition(HWND hwnd, VIEW* view, SCROLLS* scrolls) {
    unsigned int linesNumOnScreen = view->winH / view->charH;
    unsigned int charInLineNumOnScreen = view->winW / view->charW;
    int rangeMax = 0;

    //������������ ������
    if (view->curTextSize.totalLinesNum <= linesNumOnScreen) {
         EnableScrollBar(hwnd, SB_VERT, ESB_DISABLE_BOTH);
         scrolls->vertical.isActive = FALSE;
    }
    else {
         EnableScrollBar(hwnd, SB_VERT, ESB_ENABLE_BOTH);
         scrolls->vertical.isActive = TRUE;

         rangeMax = view->curTextSize.totalLinesNum - linesNumOnScreen;
         scrolls->vertical.range = fmin(rangeMax, MAX_SCROLL_RANGE);
         SetScrollRange(hwnd, SB_VERT, 0, scrolls->vertical.range, TRUE);

         scrolls->vertical.coeff = (rangeMax <= MAX_SCROLL_RANGE) ? 1.f : ((float)MAX_SCROLL_RANGE / (view->curTextSize.totalLinesNum - linesNumOnScreen));

         scrolls->vertical.pos = view->curPosition.curLineNum * scrolls->vertical.coeff;
         SetScrollPos(hwnd, SB_VERT, (int)scrolls->vertical.pos, TRUE);

    }

    //�������������� ������
    if (view->mode == WITH_LAYOUT && view->curPosition.curCharNum != 0) {
                        //���� ����������� ������� ������ �� ������ ���� � ������ � ��������
        EnableScrollBar(hwnd, SB_HORZ, ESB_ENABLE_BOTH);
        EnableScrollBar(hwnd, SB_HORZ, ESB_DISABLE_RIGHT);
        rangeMax = view->curTextSize.totalCharsNum;
    }
    else if (view->curTextSize.totalCharsNum <= charInLineNumOnScreen) {
         EnableScrollBar(hwnd, SB_HORZ, ESB_DISABLE_BOTH);
         scrolls->horizontal.isActive = FALSE;
         return;
    }
    else {
         EnableScrollBar(hwnd, SB_HORZ, ESB_ENABLE_BOTH);
         scrolls->horizontal.isActive = TRUE;
         rangeMax = view->curTextSize.totalCharsNum - charInLineNumOnScreen;
    }
     scrolls->horizontal.range = fmin(rangeMax, MAX_SCROLL_RANGE);
     SetScrollRange(hwnd, SB_HORZ, 0, scrolls->horizontal.range, TRUE);

     scrolls->horizontal.coeff = (rangeMax <= MAX_SCROLL_RANGE) ? 1.f : ((float)MAX_SCROLL_RANGE / (view->curTextSize.totalCharsNum - charInLineNumOnScreen));

     scrolls->horizontal.pos = view->curPosition.curCharNum * scrolls->horizontal.coeff;
     SetScrollPos(hwnd, SB_HORZ, (int)scrolls->horizontal.pos, TRUE);
}

/** \brief
 *  �������, ����������� ������ ����� �� �������
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 *
 */
void ScrollLineUp(VIEW* view, SCROLLS* scrolls) {
    if (scrolls->vertical.pos == 0 || !scrolls->vertical.isActive)
        return;
    view->curPosition.curLineNum--;
    scrolls->vertical.pos -= scrolls->vertical.coeff;
}

/** \brief
 *  �������, ����������� ������ ���� �� �������
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 *
 */
void ScrollLineDown(VIEW* view, SCROLLS* scrolls) {
    if (scrolls->vertical.pos == scrolls->vertical.range || !scrolls->vertical.isActive)
        return;
    if (view->curTextSize.totalLinesNum - view->curPosition.curLineNum <= view->winH / view->charH)
        return;
    view->curPosition.curLineNum++;
    scrolls->vertical.pos += scrolls->vertical.coeff;
}

/** \brief
 *  �������, ����������� ������ ����� �� ��������
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 *
 */
void ScrollPageUp(VIEW* view, SCROLLS* scrolls) {
    unsigned int linesNumOnScreen = view->winH / view->charH;
    if (!scrolls->vertical.isActive)
        return;

    if (view->curPosition.curLineNum >= linesNumOnScreen) {
        view->curPosition.curLineNum -= linesNumOnScreen;
        scrolls->vertical.pos -= linesNumOnScreen * scrolls->vertical.coeff;
    }
    else {
        view->curPosition.curLineNum = 0;
        scrolls->vertical.pos = 0;
    }
}

/** \brief
 *  �������, ����������� ������ ���� �� ��������
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 *
 */
void ScrollPageDown(VIEW* view, SCROLLS* scrolls) {
    int linesNumOnScreen = view->winH / view->charH;
    if(!scrolls->vertical.isActive)
        return;

    if ((view->curPosition.curLineNum + 2 * linesNumOnScreen) < view->curTextSize.totalLinesNum) {
        view->curPosition.curLineNum += linesNumOnScreen;
        scrolls->vertical.pos += linesNumOnScreen * scrolls->vertical.coeff;
    }
    else {
        view->curPosition.curLineNum = view->curTextSize.totalLinesNum - linesNumOnScreen;
        scrolls->vertical.pos =  view->curPosition.curLineNum * scrolls->vertical.coeff;
    }
}

/** \brief
 *  �������, ����������� ������ � ����� ������
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 *
 */
void ScrollToTop(VIEW* view, SCROLLS* scrolls) {
    if (!scrolls->vertical.isActive)
        return;

    scrolls->vertical.pos = 0;
    view->curPosition.curLineNum = 0;
}

/** \brief
 *  �������, ����������� ������ � ���� ������
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 *
 */
void ScrollToBottom(VIEW* view, SCROLLS* scrolls) {
    if (!scrolls->vertical.isActive)
        return;

    view->curPosition.curLineNum = fmax(view->curTextSize.totalLinesNum - view->winH / view->charH, 0);
    scrolls->vertical.pos = view->curPosition.curLineNum * scrolls->vertical.coeff;
}

/** \brief
 *  �������, ����������� ����������� ������������� �������
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 * \param int newPos - ����� ������� �������
 *
 */
void ScrollThumbVertical(VIEW* view, SCROLLS* scrolls, int newPos) {
    scrolls->vertical.pos = newPos;
    view->curPosition.curLineNum = scrolls->vertical.pos / scrolls->vertical.coeff;
}


/** \brief
 *  �������, ����������� ������ ����� �� ������
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 *
 */
void ScrollLineLeft(VIEW* view, SCROLLS* scrolls) {
    if (scrolls->horizontal.pos == 0 || !scrolls->horizontal.isActive)
        return;
    view->curPosition.curCharNum--;
    scrolls->horizontal.pos -= scrolls->horizontal.coeff;
}

/** \brief
 *  �������, ����������� ������ ������ �� ������
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 *
 */
void ScrollLineRight(VIEW* view, SCROLLS* scrolls) {
    if (scrolls->horizontal.pos == scrolls->horizontal.range || !scrolls->horizontal.isActive || view->mode == WITH_LAYOUT)
        return;
    view->curPosition.curCharNum++;
    scrolls->horizontal.pos += scrolls->horizontal.coeff;
}

/** \brief
 *  �������, ����������� ������ ����� �� ��������
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 *
 */
void ScrollPageLeft(VIEW* view, SCROLLS* scrolls) {
    unsigned int charInLineNumOnScreen = view->winW / view->charW;
    if (!scrolls->horizontal.isActive)
        return;

    if (view->curPosition.curCharNum >= charInLineNumOnScreen) {
        view->curPosition.curCharNum -= charInLineNumOnScreen;
        scrolls->horizontal.pos -= charInLineNumOnScreen * scrolls->horizontal.coeff;
    }
    else {
        view->curPosition.curCharNum = 0;
        scrolls->horizontal.pos = 0;
    }
}

/** \brief
 *  �������, ����������� ������ ������ �� ��������
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 *
 */
void ScrollPageRight(VIEW* view, SCROLLS* scrolls) {
    int charInLineNumOnScreen = view->winW / view->charW;
    if (!scrolls->horizontal.isActive)
        return;

    if ((view->curPosition.curCharNum + 2 * charInLineNumOnScreen) < view->curTextSize.totalCharsNum) {
        view->curPosition.curCharNum += charInLineNumOnScreen;
        scrolls->horizontal.pos += charInLineNumOnScreen * scrolls->horizontal.coeff;
    }
    else {
        view->curPosition.curCharNum = view->curTextSize.totalCharsNum - charInLineNumOnScreen;
        scrolls->horizontal.pos =  view->curPosition.curCharNum * scrolls->horizontal.coeff;
    }
}

/** \brief
 *  �������, ����������� ������ � ������ ���� ������
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 *
 */
void ScrollToLeftEdge(VIEW* view, SCROLLS* scrolls) {
    if (!scrolls->horizontal.isActive)
        return;

    scrolls->horizontal.pos = 0;
    view->curPosition.curCharNum = 0;
}

/** \brief
 *  �������, ����������� ������ � ������� ���� ������
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 *
 */
void ScrollToRightEdge(VIEW* view, SCROLLS* scrolls) {
    if (!scrolls->horizontal.isActive)
        return;

    view->curPosition.curCharNum = fmax((int)(view->curTextSize.totalCharsNum - view->winW / view->charW), 0);
    scrolls->horizontal.pos = view->curPosition.curCharNum * scrolls->horizontal.coeff;
}

/** \brief
 *  �������, ����������� ����������� ��������������� �������
 * \param VIEW* view - ��������� ����������� ������
 * \param SCROLLS* scrolls - ��������� ��������
 * \param int newPos - ����� ������� �������
 *
 */
void ScrollThumbHorizontal(VIEW* view, SCROLLS* scrolls, int newPos) {
    //��������� �������� ������ � ������ � ��������
    if (view->mode == WITH_LAYOUT && newPos >= scrolls->horizontal.pos)
        return;

    scrolls->horizontal.pos = newPos;
    view->curPosition.curCharNum = scrolls->horizontal.pos / scrolls->horizontal.coeff;
}
