#pragma once
#include <windows.h>

typedef enum ERROR_STATE {
    NOT_ENOUGH_MEMORY,
    FILE_OPEN_FAIL,
    SUCCESS
} ERROR_STATE;


/* ������ ������ */
typedef struct TEXT {
    char* textStr;       //�����
    unsigned int textLength;      //���������� ��������
    unsigned int linesNum;        //���������� �����
    unsigned int maxLineLen;      //����� ���������� ������
    unsigned int* linesPositions; //������ �������� ����� �����
} TEXT;


typedef enum VIEW_MODE {
    USUAL,
    WITH_LAYOUT,
} VIEW_MODE;

typedef struct TEXT_POSITION {
    unsigned int curLineNum; //����� �����, � ������� ���������� ����� ������, ����������� �� ������
    unsigned int curCharNum; //����� �������, � ������� ���������� ������ ����� ������, ������������ �� ������
} TEXT_POSITION;

typedef struct TEXT_SIZE {
    unsigned int totalLinesNum; //���������� ����� � ������� ������� ������ (��� ������� - ����� ����� � �������� ������)
    unsigned int totalCharsNum; //���������� �������� � ������ � ������� ������� ������
                       //(��� ������� - ����� ���������� ������ � �������� ������)
} TEXT_SIZE;

/* ������ ����������� ������ �� ������*/
typedef struct VIEW {
    boolean textIsLoaded;        //����������, ������ �� �����-������ ����
    unsigned int winW, winH;     //������� ����
    unsigned int charW, charH;   //������� �������
    VIEW_MODE mode;              //����� ���������
    TEXT_POSITION curPosition;   //������� ����� ������ ������ ������
    unsigned int* strPositionsWithLayout; //������ �������� ����� ����� ��� ������ � ������ � ��������
    TEXT_SIZE curTextSize;
} VIEW;


/* ������ ������ ��������� */
typedef struct ONE_SCROLL {
    float pos;           //������� ��������� �������
    float coeff;         //����������� ��������� ������� ������������ ������
    unsigned int range;           //������������ ��������� ������� �������
    boolean isActive;    //����������, ������� �� ������
} ONE_SCROLL;

typedef struct SCROLLS {
    ONE_SCROLL vertical;   //������������ ������
    ONE_SCROLL horizontal; //�������������� ������
} SCROLLS;

/* ������ ������� */
typedef struct CARET {
    int verticalPos;        //������������ ������� ������������ ������� ����� ������
    int horizontalPos;      //�������������� ������� ������������ ������� ����� ������
    unsigned int absoluteVerticalPos;    //������������ ������� ������������ ����� ������
    unsigned int absoluteHorizontalPos;  //������������� ������� ������������ ����� ������
} CARET;
