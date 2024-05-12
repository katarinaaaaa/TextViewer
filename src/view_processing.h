#pragma once

#include <tchar.h>
#include <math.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "models.h"
#include "text_processing.h"
#include "caret.h"

#define HORZ_OFFSET_MULT 0.5 //отношение ширины символа и размера горизонтального отступа

void PrintText(HDC hdc, TEXT* text, VIEW* view);

void UpdateVeiwWindowSize(HWND hwnd, VIEW* view);
void InitializeView(HWND hwnd, VIEW* view);
void SetZeroTextPosition(VIEW* view);

void FillInfoForLayout(TEXT* text, VIEW* view, ERROR_STATE* error);
void UpdateInfoForLayout(TEXT* text, VIEW* view, ERROR_STATE* error);
void UpdateViewForUsualMode(VIEW* view, TEXT* text);

void FindTextPositionForLayout(TEXT* text, VIEW* view);
void FindTextPositionForUsual(TEXT* text, VIEW* view);
void MoveWhileResize(VIEW* view);
void UpdateInfoForLayoutAfterResize(TEXT* text, VIEW* view, CARET* caret, ERROR_STATE* error);

void FreeAllMemory(VIEW* view, TEXT* text);
