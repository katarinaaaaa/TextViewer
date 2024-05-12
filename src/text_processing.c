#include "text_processing.h"

/** \brief
 *  Функция, обрабатывающая ошибки чтения и выделения памяти
 * \param HWND hwnd - дескриптор окна
 * \param TCHAR szClassName[ ] - имя класса окна приложения
 * \param ERROR_STATE err - тип ошибки
 *
 */
void ProcessErrorState(HWND hwnd, TCHAR szClassName[ ], ERROR_STATE err) {
    switch (err){
        case SUCCESS:
            break;
        case NOT_ENOUGH_MEMORY:
            MessageBox(hwnd, "Sorry, a problem with memory allocation appeared. Please, restart the application.",
                       szClassName, MB_ICONWARNING | MB_OK);
            SendMessage(hwnd, WM_CLOSE, 0, 0L);
            break;
        case FILE_OPEN_FAIL:
            MessageBox(hwnd, "Sorry, a problem with reading the file appeared. Please, check that file is available.",
                       szClassName, MB_ICONWARNING | MB_OK);
            SendMessage(hwnd, WM_CLOSE, 0, 0L);
            break;
        default:
            break;
    }
}

/** \brief
 *  Функция, читающая текст из файла
 * \param TEXT* text - структура текста
 * \param char* fileName - имя файла
 * \param ERROR_STATE* err - возврат ошибки
 *
 */
void ReadText(TEXT* text, char* fileName, ERROR_STATE* err) {
    unsigned int i, strCounter = 0, strLen = 0;
    FILE* file = (FILE*)fopen(fileName, "rb");
    if (file == NULL) {
        *err = FILE_OPEN_FAIL;
        return;
    }

    fseek(file, 0, SEEK_END);
    text->textLength = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (text->textLength == 0) {
        fclose(file);
        *err = SUCCESS;
        return;
    }

    text->textStr = (char*)malloc((text->textLength + 1) * sizeof(char));
    text->linesPositions = (unsigned int*)malloc(text->textLength * sizeof(int));
    if (!text->textStr || !text->linesPositions) {
        fclose(file);
        *err = NOT_ENOUGH_MEMORY;
        return;
    }

    fread(text->textStr, sizeof(char), text->textLength, file);
    text->textStr[text->textLength] = '\0';
    fclose(file);

    //сохраняем позиции переводов строки и длину самой длинной строки
    strCounter = 0;
    text->maxLineLen = 0;
    text->linesPositions[strCounter] = 0;
    for (i = 0; i <= text->textLength; i++) {
        strLen++;
        if (text->textStr[i] == '\n' || text->textStr[i] == '\0') {

            strCounter++;

            text->linesPositions[strCounter] = i + 1;

            if (strLen > text->maxLineLen)
                text->maxLineLen = strLen - 1;
            strLen = 0;

            if (text->textStr[i] == '\0') {
                  text->linesPositions[strCounter]++;
                  if (text->maxLineLen == text->linesPositions[strCounter] - text->linesPositions[strCounter - 1] - 2)
                      text->maxLineLen++;
                  break;
            }
            text->textStr[i] = 0;
        }
        if (text->textStr[i] == '\r')
           text->textStr[i] = 0;
    }

    text->textLength = i;
    text->linesNum = strCounter;
    *err = SUCCESS;
}

/** \brief
 *  Функция, инициализирующая структуру текста
 * \param TEXT* text - структура текста
 *
 */
void InitializeText(TEXT* text) {
     text->textStr = NULL;
     text->linesPositions = NULL;
}
