#pragma once

#include <tchar.h>
#include <math.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "models.h"
#include "scroll.h"

void SetZeroCaretPosition(CARET* caret);
void FixCaretOutOfLineBounds(CARET* caret, TEXT* text, VIEW* view, SCROLLS* scrolls, HWND hwnd);

void CaretUp(CARET* caret, TEXT* text, VIEW* view, SCROLLS* scrolls, HWND hwnd);
void CaretDown(CARET* caret, TEXT* text, VIEW* view, SCROLLS* scrolls, HWND hwnd);
void CaretRight(CARET* caret, TEXT* text, VIEW* view, SCROLLS* scrolls, HWND hwnd);
void CaretLeft(CARET* caret, TEXT* text, VIEW* view, SCROLLS* scrolls, HWND hwnd);

void FindCaretPositionForLayout(CARET* caret, TEXT* text, VIEW* view);
void FindCaretPositionForUsual(CARET* caret, TEXT* text, VIEW* view);
