#pragma once

#include <tchar.h>
#include <math.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "models.h"

void ProcessErrorState(HWND hwnd, TCHAR szClassName[ ], ERROR_STATE err);

void ReadText(TEXT* text, char* fileName, ERROR_STATE* err);
void InitializeText(TEXT* text);
