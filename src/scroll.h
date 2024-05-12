#pragma once

#include <tchar.h>
#include <math.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "models.h"

#define MAX_SCROLL_RANGE 65000

void InitializeScrolls(HWND hwnd, SCROLLS *scrolls);
void ResetScrollsPosition(HWND hwnd, VIEW* view, SCROLLS* scrolls);

void ScrollLineUp(VIEW* view, SCROLLS* scrolls);
void ScrollLineDown(VIEW* view, SCROLLS* scrolls);
void ScrollPageUp(VIEW* view, SCROLLS* scrolls);
void ScrollPageDown(VIEW* view, SCROLLS* scrolls);
void ScrollToTop(VIEW* view, SCROLLS* scrolls);
void ScrollToBottom(VIEW* view, SCROLLS* scrolls);
void ScrollThumbVertical(VIEW* view, SCROLLS* scrolls, int newPos);

void ScrollLineLeft(VIEW* view, SCROLLS* scrolls);
void ScrollLineRight( VIEW* view, SCROLLS* scrolls);
void ScrollPageLeft(VIEW* view, SCROLLS* scrolls);
void ScrollPageRight(VIEW* view, SCROLLS* scrolls);
void ScrollToLeftEdge(VIEW* view, SCROLLS* scrolls);
void ScrollToRightEdge(VIEW* view, SCROLLS* scrolls);
void ScrollThumbHorizontal(VIEW* view, SCROLLS* scrolls, int newPos);
