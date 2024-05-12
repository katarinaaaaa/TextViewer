#pragma once
#include <windows.h>

typedef enum ERROR_STATE {
    NOT_ENOUGH_MEMORY,
    FILE_OPEN_FAIL,
    SUCCESS
} ERROR_STATE;


/* модель текста */
typedef struct TEXT {
    char* textStr;       //текст
    unsigned int textLength;      //количество символов
    unsigned int linesNum;        //количество строк
    unsigned int maxLineLen;      //длина наибольшей строки
    unsigned int* linesPositions; //массив индексов начал строк
} TEXT;


typedef enum VIEW_MODE {
    USUAL,
    WITH_LAYOUT,
} VIEW_MODE;

typedef struct TEXT_POSITION {
    unsigned int curLineNum; //номер линии, с которой начинается часть текста, находящаяся на экране
    unsigned int curCharNum; //номер символа, с которой начинаются строки части текста, находящегося на экране
} TEXT_POSITION;

typedef struct TEXT_SIZE {
    unsigned int totalLinesNum; //количество строк в текущей верстке текста (без верстки - число строк в исходном тексте)
    unsigned int totalCharsNum; //количество символов в строке в текущей верстке текста
                       //(без верстки - длина наибольшей строки в исходном тексте)
} TEXT_SIZE;

/* модель отображения текста на экране*/
typedef struct VIEW {
    boolean textIsLoaded;        //показывает, открыт ли какой-нибудь файл
    unsigned int winW, winH;     //размеры окна
    unsigned int charW, charH;   //размеры символа
    VIEW_MODE mode;              //режим просмотра
    TEXT_POSITION curPosition;   //текущая точка начала вывода текста
    unsigned int* strPositionsWithLayout; //массив индексов начал строк при выводе в режиме с версткой
    TEXT_SIZE curTextSize;
} VIEW;


/* модель полосы прокрутки */
typedef struct ONE_SCROLL {
    float pos;           //текущее положение скролла
    float coeff;         //коэффициент прокрутки скролла относительно текста
    unsigned int range;           //максимальная возможная позиция скролла
    boolean isActive;    //показывает, активен ли скролл
} ONE_SCROLL;

typedef struct SCROLLS {
    ONE_SCROLL vertical;   //вертикальный скролл
    ONE_SCROLL horizontal; //горизонтальный скролл
} SCROLLS;

/* модель каретки */
typedef struct CARET {
    int verticalPos;        //вертикальная позиция относительно текущей части вывода
    int horizontalPos;      //горизонтальная позиция относительно текущей части вывода
    unsigned int absoluteVerticalPos;    //вертикальная позиция относительно всего текста
    unsigned int absoluteHorizontalPos;  //горизонтаьная позиция относительно всего текста
} CARET;
