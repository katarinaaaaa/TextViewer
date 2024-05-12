#include "view_processing.h"

/** \brief
 *  Функция, выводящая текст в окно без верстки
 * \param HDC hdc - дескриптор контекста устройства
 * \param TEXT* text - структура текста
 * \param VIEW* view - структура отображения текста
 *
 */
void PrintTextUsual(HDC hdc, TEXT* text, VIEW* view) {
    int i, j, count;
    char* curStr = NULL;
    int maxCharsNum = view->winW / view->charW;
    int linesNum = fmin((int)text->linesNum, (int)view->winH / view->charH); //количество строчек для вывода
    for (i = 0; i < linesNum; i++) {
        j = i + view->curPosition.curLineNum;

        count = fmin((int)text->linesPositions[j + 1] - (int)text->linesPositions[j] - (int)view->curPosition.curCharNum - 2, (int)maxCharsNum);

        curStr = text->textStr + text->linesPositions[j]; //начало текущей строчки
        TextOut(hdc, view->charW * HORZ_OFFSET_MULT, view->charH * i, curStr + view->curPosition.curCharNum, count);
    }
}

/** \brief
 *  Функция, выводящая текст в окно с версткой
 * \param HDC hdc - дескриптор контекста устройства
 * \param TEXT* text - структура текста
 * \param VIEW* view - структура отображения текста
 *
 */
void PrintTextWithLayout(HDC hdc, TEXT* text, VIEW* view) {
    int i, j, count;
    char* curStr = NULL;
    int linesNum = fmin((int)view->curTextSize.totalLinesNum, (int)view->winH / view->charH); //количество строчек для вывода
    for (i = 0; i < linesNum; i++) {
        j = i + view->curPosition.curLineNum;
        curStr = text->textStr + view->strPositionsWithLayout[j * 2];  //начало текущей строчки

        count = view->strPositionsWithLayout[j * 2 + 1] - view->strPositionsWithLayout[j * 2] - view->curPosition.curCharNum;

        TextOut(hdc, view->charW * HORZ_OFFSET_MULT, view->charH * i, curStr + view->curPosition.curCharNum, count);
    }
}

/** \brief
 *  Функция, выводящая текст в окно согласно текущему режиму
 * \param HDC hdc - дескриптор контекста устройства
 * \param TEXT* text - структура текста
 * \param VIEW* view - структура отображения текста
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
 *  Функция, получающая размер рабочей области(с учетом отступа)
 * \param HWND hwnd - дескриптор окна
 * \param VIEW* view - структура отображения текста
 *
 */
void UpdateVeiwWindowSize(HWND hwnd, VIEW* view) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    view->winH = rect.bottom;
    view->winW = rect.right - view->charW * HORZ_OFFSET_MULT; //учитываем отступ справа
}

/** \brief
 *  Функция, инициализирующая структуру отображения
 * \param HWND hwnd - дескриптор окна
 * \param VIEW* view - структура отображения текста
 */
void InitializeView(HWND hwnd, VIEW* view) {
    view->mode = USUAL;
    view->strPositionsWithLayout = NULL;
    view->curPosition.curCharNum = 0;
    view->curPosition.curLineNum = 0;
    UpdateVeiwWindowSize(hwnd, view);
}

/** \brief
 *  Функция, переводящаяя начало вывода в начало текста
 * \param VIEW* view - структура отображения текста
 *
 */
void SetZeroTextPosition(VIEW* view) {
    view->curPosition.curCharNum = 0;
    view->curPosition.curLineNum = 0;
}

/** \brief
 *  Функция, заполняющая поля структуры представления текста, использующиеся для режима с версткой
 * \param TEXT* text - структура текста
 * \param VIEW* view - структура отображения текста
  * \param ERROR_STATE* err - тип ошибки
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
        if (text->textStr[text->linesPositions[i]] == 0) { //если строчка пустая
            view->strPositionsWithLayout[(numOfLinesWithLayout + curSubstrNum) * 2] = text->linesPositions[i];
            view->strPositionsWithLayout[(numOfLinesWithLayout + curSubstrNum) * 2 + 1] = text->linesPositions[i];
            numOfLinesWithLayout++;
            continue;
        }

        for (j = text->linesPositions[i]; j < text->linesPositions[i + 1] - 2; j++) {
            view->strPositionsWithLayout[(numOfLinesWithLayout + curSubstrNum) * 2] = j;

            j += maxSymbolsInLine - 1;
            if (j >= text->linesPositions[i + 1] - 2) //если достигли конца строки
               j = text->linesPositions[i + 1] - 1;
            else if (text->textStr[j + 1] == ' ') //пропускаем пробел в конце строки
               j++;
            else if (text->textStr[j] != ' ') {
               j--;
               while (text->textStr[j] != ' ' && j > view->strPositionsWithLayout[(numOfLinesWithLayout + curSubstrNum) * 2])
                  j--; //доходим до конца предыдущего слова
               if (j <= view->strPositionsWithLayout[(numOfLinesWithLayout + curSubstrNum) * 2]) {
                    //если ни одно слово не вмещается в строку, переносим по буквам
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
 *  Функция, обновляющая поля структуры представления текста, использующиеся для режима с версткой
 * \param TEXT* text - структура текста
 * \param VIEW* view - структура отображения текста
 * \param ERROR_STATE* err - тип ошибки
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
 *  Функция, подготавливающаю структуру представления текста для режима без верстки
 * \param TEXT* text - структура текста
 * \param VIEW* view - структура отображения текста
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
 *  Функция, определяющая текущую позицию текста при переключении в режим с версткой
 * \param TEXT* text - структура текста
 * \param VIEW* view - структура отображения текста
 *
 */
void FindTextPositionForLayout(TEXT* text, VIEW* view) {
	unsigned int i = 0;
	unsigned int curAbsolutePosition = text->linesPositions[view->curPosition.curLineNum] + view->curPosition.curCharNum;
               //номер текущего первого символа вывода в исходной строке текста
	if (curAbsolutePosition >= text->linesPositions[view->curPosition.curLineNum + 1])
        curAbsolutePosition = text->linesPositions[view->curPosition.curLineNum + 1] - 1;

    for (i = 0; i < view->curTextSize.totalLinesNum * 2; i += 2) {
        //ищем в какую строчку попадает символ
        if (curAbsolutePosition >= view->strPositionsWithLayout[i] &&
                       curAbsolutePosition < view->strPositionsWithLayout[i + 2]) {
            view->curPosition.curLineNum = i / 2;
            break;
        }
    }
    view->curPosition.curCharNum  = curAbsolutePosition - view->strPositionsWithLayout[i];
}

/** \brief
 *  Функция, определяющая текущую позицию текста при переключении в режим без верстки
 * \param TEXT* text - структура текста
 * \param VIEW* view - структура отображения текста
 *
 */
void FindTextPositionForUsual(TEXT* text, VIEW* view) {
    unsigned int i = 0;
    unsigned int curAbsolutePosition = view->strPositionsWithLayout[view->curPosition.curLineNum * 2] + view->curPosition.curCharNum;
               //номер текущего первого символа вывода в исходной строке текста
    if (curAbsolutePosition >= view->strPositionsWithLayout[view->curPosition.curLineNum * 2 + 1])
         curAbsolutePosition = view->strPositionsWithLayout[view->curPosition.curLineNum * 2  + 1] - 1;

    for (i = 0; i < text->linesNum; i++) {
        //ищем в какую строчку попадает символ
        if (curAbsolutePosition >= text->linesPositions[i] &&
                       curAbsolutePosition < text->linesPositions[i + 1]) {
            view->curPosition.curLineNum = i;
            break;
        }
    }
    view->curPosition.curCharNum = curAbsolutePosition - text->linesPositions[i];
}

/** \brief
 *  Функция, смещающая текст при расширении окна
 * \param VIEW* view - структура отображения текста
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
 *  Функция, определяющая текущую позицию текста и положение каретки при изменении размера окна в режиме с версткой
 * \param TEXT* text - структура текста
 * \param VIEW* view - структура отображения текста
 * \param CARET* caret - структура каретки
 * \param ERROR_STATE err - тип ошибки
 *
 */
void UpdateInfoForLayoutAfterResize(TEXT* text, VIEW* view, CARET* caret, ERROR_STATE* error) {
    unsigned int i = 0;
    unsigned int curAbsolutePosition, curAbsoluteCaretPos;

    //запоминаем положение начала вывода текста
    curAbsolutePosition = view->strPositionsWithLayout[view->curPosition.curLineNum * 2] + view->curPosition.curCharNum;
    if (curAbsolutePosition >= view->strPositionsWithLayout[view->curPosition.curLineNum * 2 + 1])
         curAbsolutePosition = view->strPositionsWithLayout[view->curPosition.curLineNum * 2  + 1] - 1;

    //запоминаем положение каретки
    curAbsoluteCaretPos = view->strPositionsWithLayout[caret->absoluteVerticalPos * 2] + caret->absoluteHorizontalPos;

    UpdateInfoForLayout(text, view, error);

    //ищем в какую строчку попадает символ в новом масштабе
    for (i = 0; i < view->curTextSize.totalLinesNum * 2; i += 2) {
        if (curAbsolutePosition >= view->strPositionsWithLayout[i] &&
                       curAbsolutePosition < view->strPositionsWithLayout[i + 2]) {
            view->curPosition.curLineNum = i / 2;
            break;
        }
    }
    view->curPosition.curCharNum  = curAbsolutePosition - view->strPositionsWithLayout[i];

    //ищем в какую строчку попадает каретка в новом масштабе
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
 *  Функция, освобождающая память перед чтением нового файла
 * \param TEXT* text - структура текста
 * \param VIEW* view - структура отображения текста
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
