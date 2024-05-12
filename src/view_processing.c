#include "view_processing.h"

/** \brief
 *  �������, ��������� ����� � ���� ��� �������
 * \param HDC hdc - ���������� ��������� ����������
 * \param TEXT* text - ��������� ������
 * \param VIEW* view - ��������� ����������� ������
 *
 */
void PrintTextUsual(HDC hdc, TEXT* text, VIEW* view) {
    int i, j, count;
    char* curStr = NULL;
    int maxCharsNum = view->winW / view->charW;
    int linesNum = fmin((int)text->linesNum, (int)view->winH / view->charH); //���������� ������� ��� ������
    for (i = 0; i < linesNum; i++) {
        j = i + view->curPosition.curLineNum;

        count = fmin((int)text->linesPositions[j + 1] - (int)text->linesPositions[j] - (int)view->curPosition.curCharNum - 2, (int)maxCharsNum);

        curStr = text->textStr + text->linesPositions[j]; //������ ������� �������
        TextOut(hdc, view->charW * HORZ_OFFSET_MULT, view->charH * i, curStr + view->curPosition.curCharNum, count);
    }
}

/** \brief
 *  �������, ��������� ����� � ���� � ��������
 * \param HDC hdc - ���������� ��������� ����������
 * \param TEXT* text - ��������� ������
 * \param VIEW* view - ��������� ����������� ������
 *
 */
void PrintTextWithLayout(HDC hdc, TEXT* text, VIEW* view) {
    int i, j, count;
    char* curStr = NULL;
    int linesNum = fmin((int)view->curTextSize.totalLinesNum, (int)view->winH / view->charH); //���������� ������� ��� ������
    for (i = 0; i < linesNum; i++) {
        j = i + view->curPosition.curLineNum;
        curStr = text->textStr + view->strPositionsWithLayout[j * 2];  //������ ������� �������

        count = view->strPositionsWithLayout[j * 2 + 1] - view->strPositionsWithLayout[j * 2] - view->curPosition.curCharNum;

        TextOut(hdc, view->charW * HORZ_OFFSET_MULT, view->charH * i, curStr + view->curPosition.curCharNum, count);
    }
}

/** \brief
 *  �������, ��������� ����� � ���� �������� �������� ������
 * \param HDC hdc - ���������� ��������� ����������
 * \param TEXT* text - ��������� ������
 * \param VIEW* view - ��������� ����������� ������
 *
 */
void PrintText(HDC hdc, TEXT* text, VIEW* view) {
     switch (view->mode) {
        case USUAL:
            PrintTextUsual(hdc, text, view);
            break;
        case WITH_LAYOUT:
            PrintTextWithLayout(hdc, text, view);
            break;
        default:
            break;
    }
}

/** \brief
 *  �������, ���������� ������ ������� �������(� ������ �������)
 * \param HWND hwnd - ���������� ����
 * \param VIEW* view - ��������� ����������� ������
 *
 */
void UpdateVeiwWindowSize(HWND hwnd, VIEW* view) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    view->winH = rect.bottom;
    view->winW = rect.right - view->charW * HORZ_OFFSET_MULT; //��������� ������ ������
}

/** \brief
 *  �������, ���������������� ��������� �����������
 * \param HWND hwnd - ���������� ����
 * \param VIEW* view - ��������� ����������� ������
 */
void InitializeView(HWND hwnd, VIEW* view) {
    view->mode = USUAL;
    view->strPositionsWithLayout = NULL;
    view->curPosition.curCharNum = 0;
    view->curPosition.curLineNum = 0;
    UpdateVeiwWindowSize(hwnd, view);
}

/** \brief
 *  �������, ������������ ������ ������ � ������ ������
 * \param VIEW* view - ��������� ����������� ������
 *
 */
void SetZeroTextPosition(VIEW* view) {
    view->curPosition.curCharNum = 0;
    view->curPosition.curLineNum = 0;
}

/** \brief
 *  �������, ����������� ���� ��������� ������������� ������, �������������� ��� ������ � ��������
 * \param TEXT* text - ��������� ������
 * \param VIEW* view - ��������� ����������� ������
  * \param ERROR_STATE* err - ��� ������
 *
 */
void FillInfoForLayout(TEXT* text, VIEW* view, ERROR_STATE* error) {
     unsigned int numOfLinesWithLayout = 0, curSubstrNum = 0, i, j;
     int maxSymbolsInLine = (float)view->winW / view->charW;

     view->strPositionsWithLayout = (unsigned int*)malloc(sizeof(int) * ((ceil((float)text->maxLineLen / (float)maxSymbolsInLine) + 1) * text->linesNum) * 4);
     if (!view->strPositionsWithLayout) {
        *error = NOT_ENOUGH_MEMORY;
        return;
     }

     for (i = 0; i < text->linesNum; i++) {
        curSubstrNum = 0;
        if (text->textStr[text->linesPositions[i]] == 0) { //���� ������� ������
            view->strPositionsWithLayout[(numOfLinesWithLayout + curSubstrNum) * 2] = text->linesPositions[i];
            view->strPositionsWithLayout[(numOfLinesWithLayout + curSubstrNum) * 2 + 1] = text->linesPositions[i];
            numOfLinesWithLayout++;
            continue;
        }

        for (j = text->linesPositions[i]; j < text->linesPositions[i + 1] - 2; j++) {
            view->strPositionsWithLayout[(numOfLinesWithLayout + curSubstrNum) * 2] = j;

            j += maxSymbolsInLine - 1;
            if (j >= text->linesPositions[i + 1] - 2) //���� �������� ����� ������
               j = text->linesPositions[i + 1] - 1;
            else if (text->textStr[j + 1] == ' ') //���������� ������ � ����� ������
               j++;
            else if (text->textStr[j] != ' ') {
               j--;
               while (text->textStr[j] != ' ' && j > view->strPositionsWithLayout[(numOfLinesWithLayout + curSubstrNum) * 2])
                  j--; //������� �� ����� ����������� �����
               if (j <= view->strPositionsWithLayout[(numOfLinesWithLayout + curSubstrNum) * 2]) {
                    //���� �� ���� ����� �� ��������� � ������, ��������� �� ������
                    j = view->strPositionsWithLayout[(numOfLinesWithLayout + curSubstrNum) * 2] + maxSymbolsInLine;
                    view->strPositionsWithLayout[(numOfLinesWithLayout + curSubstrNum) * 2 + 1] = j;
                    curSubstrNum++;
                    j--;
                    continue;
               }
            }

            view->strPositionsWithLayout[(numOfLinesWithLayout + curSubstrNum) * 2 + 1] = j;
            if (text->textStr[j] == 0)
                view->strPositionsWithLayout[(numOfLinesWithLayout + curSubstrNum) * 2 + 1]--;
            curSubstrNum++;
        }
        numOfLinesWithLayout += curSubstrNum;
    }

     view->curTextSize.totalLinesNum = numOfLinesWithLayout;
     view->curTextSize.totalCharsNum = maxSymbolsInLine - 1;

     *error = SUCCESS;
}

/** \brief
 *  �������, ����������� ���� ��������� ������������� ������, �������������� ��� ������ � ��������
 * \param TEXT* text - ��������� ������
 * \param VIEW* view - ��������� ����������� ������
 * \param ERROR_STATE* err - ��� ������
 *
 */
void UpdateInfoForLayout(TEXT* text, VIEW* view, ERROR_STATE* error) {
    if (view->strPositionsWithLayout) {
        free(view->strPositionsWithLayout);
        view->strPositionsWithLayout = NULL;
    }
    FillInfoForLayout(text, view, error);
}

/** \brief
 *  �������, ���������������� ��������� ������������� ������ ��� ������ ��� �������
 * \param TEXT* text - ��������� ������
 * \param VIEW* view - ��������� ����������� ������
 *
 */
void UpdateViewForUsualMode(VIEW* view, TEXT* text) {
    if (view->strPositionsWithLayout) {
        free(view->strPositionsWithLayout);
        view->strPositionsWithLayout = NULL;
    }
    view->curTextSize.totalCharsNum = text->maxLineLen - 1;
    view->curTextSize.totalLinesNum = text->linesNum;
}

/** \brief
 *  �������, ������������ ������� ������� ������ ��� ������������ � ����� � ��������
 * \param TEXT* text - ��������� ������
 * \param VIEW* view - ��������� ����������� ������
 *
 */
void FindTextPositionForLayout(TEXT* text, VIEW* view) {
	unsigned int i = 0;
	unsigned int curAbsolutePosition = text->linesPositions[view->curPosition.curLineNum] + view->curPosition.curCharNum;
               //����� �������� ������� ������� ������ � �������� ������ ������
	if (curAbsolutePosition >= text->linesPositions[view->curPosition.curLineNum + 1])
        curAbsolutePosition = text->linesPositions[view->curPosition.curLineNum + 1] - 1;

    for (i = 0; i < view->curTextSize.totalLinesNum * 2; i += 2) {
        //���� � ����� ������� �������� ������
        if (curAbsolutePosition >= view->strPositionsWithLayout[i] &&
                       curAbsolutePosition < view->strPositionsWithLayout[i + 2]) {
            view->curPosition.curLineNum = i / 2;
            break;
        }
    }
    view->curPosition.curCharNum  = curAbsolutePosition - view->strPositionsWithLayout[i];
}

/** \brief
 *  �������, ������������ ������� ������� ������ ��� ������������ � ����� ��� �������
 * \param TEXT* text - ��������� ������
 * \param VIEW* view - ��������� ����������� ������
 *
 */
void FindTextPositionForUsual(TEXT* text, VIEW* view) {
    unsigned int i = 0;
    unsigned int curAbsolutePosition = view->strPositionsWithLayout[view->curPosition.curLineNum * 2] + view->curPosition.curCharNum;
               //����� �������� ������� ������� ������ � �������� ������ ������
    if (curAbsolutePosition >= view->strPositionsWithLayout[view->curPosition.curLineNum * 2 + 1])
         curAbsolutePosition = view->strPositionsWithLayout[view->curPosition.curLineNum * 2  + 1] - 1;

    for (i = 0; i < text->linesNum; i++) {
        //���� � ����� ������� �������� ������
        if (curAbsolutePosition >= text->linesPositions[i] &&
                       curAbsolutePosition < text->linesPositions[i + 1]) {
            view->curPosition.curLineNum = i;
            break;
        }
    }
    view->curPosition.curCharNum = curAbsolutePosition - text->linesPositions[i];
}

/** \brief
 *  �������, ��������� ����� ��� ���������� ����
 * \param VIEW* view - ��������� ����������� ������
 *
 */
void MoveWhileResize(VIEW* view) {
    unsigned int linesNumOnScreen = view->winH / view->charH;
    unsigned int charInLineNumOnScreen = view->winW / view->charW;
    if (view->curPosition.curLineNum != 0 &&
                 ((view->curTextSize.totalLinesNum - view->curPosition.curLineNum + 1) <= linesNumOnScreen)) {
        view->curPosition.curLineNum = fmax((int)(view->curTextSize.totalLinesNum - linesNumOnScreen), 0);
    }
    if (view->curPosition.curCharNum != 0 &&
                 ((view->curTextSize.totalCharsNum - view->curPosition.curCharNum + 1) <= charInLineNumOnScreen)) {
        view->curPosition.curCharNum = fmax((int)(view->curTextSize.totalCharsNum - charInLineNumOnScreen), 0);
    }
    if (view->curPosition.curLineNum >= view->curTextSize.totalLinesNum)
        view->curPosition.curLineNum =  view->curTextSize.totalLinesNum - charInLineNumOnScreen + 1;
}

/** \brief
 *  �������, ������������ ������� ������� ������ � ��������� ������� ��� ��������� ������� ���� � ������ � ��������
 * \param TEXT* text - ��������� ������
 * \param VIEW* view - ��������� ����������� ������
 * \param CARET* caret - ��������� �������
 * \param ERROR_STATE err - ��� ������
 *
 */
void UpdateInfoForLayoutAfterResize(TEXT* text, VIEW* view, CARET* caret, ERROR_STATE* error) {
    unsigned int i = 0;
    unsigned int curAbsolutePosition, curAbsoluteCaretPos;

    //���������� ��������� ������ ������ ������
    curAbsolutePosition = view->strPositionsWithLayout[view->curPosition.curLineNum * 2] + view->curPosition.curCharNum;
    if (curAbsolutePosition >= view->strPositionsWithLayout[view->curPosition.curLineNum * 2 + 1])
         curAbsolutePosition = view->strPositionsWithLayout[view->curPosition.curLineNum * 2  + 1] - 1;

    //���������� ��������� �������
    curAbsoluteCaretPos = view->strPositionsWithLayout[caret->absoluteVerticalPos * 2] + caret->absoluteHorizontalPos;

    UpdateInfoForLayout(text, view, error);

    //���� � ����� ������� �������� ������ � ����� ��������
    for (i = 0; i < view->curTextSize.totalLinesNum * 2; i += 2) {
        if (curAbsolutePosition >= view->strPositionsWithLayout[i] &&
                       curAbsolutePosition < view->strPositionsWithLayout[i + 2]) {
            view->curPosition.curLineNum = i / 2;
            break;
        }
    }
    view->curPosition.curCharNum  = curAbsolutePosition - view->strPositionsWithLayout[i];

    //���� � ����� ������� �������� ������� � ����� ��������
    for (i = 0; i < view->curTextSize.totalLinesNum * 2; i += 2) {
        if (curAbsoluteCaretPos >= view->strPositionsWithLayout[(int)fmax(i - 1, 0)] &&
                       curAbsoluteCaretPos <= view->strPositionsWithLayout[i + 1]) {
            caret->absoluteVerticalPos = i / 2;
            break;
        }
    }
    caret->absoluteHorizontalPos = (int)curAbsoluteCaretPos - (int)view->strPositionsWithLayout[i];
    caret->verticalPos = (int)caret->absoluteVerticalPos - (int)view->curPosition.curLineNum;
    caret->horizontalPos = (int)caret->absoluteHorizontalPos - (int)view->curPosition.curCharNum;
}

/** \brief
 *  �������, ������������� ������ ����� ������� ������ �����
 * \param TEXT* text - ��������� ������
 * \param VIEW* view - ��������� ����������� ������
 *
 */
void FreeAllMemory(VIEW* view, TEXT* text) {
    if (text->textStr) {
        free(text->textStr);
        text->textStr = NULL;
    }
    if (text->linesPositions) {
        free(text->linesPositions);
        text->linesPositions = NULL;
    }
    if (view->strPositionsWithLayout) {
        free(view->strPositionsWithLayout);
        view->strPositionsWithLayout = NULL;
    }
}
