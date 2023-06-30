#include "UtilFunction.h"
#include <iostream>
#include <windows.h>

void UtilFunction::ClearConsoleLine()
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD cursorPosition;
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;

    // 현재 커서 위치 가져오기
    GetConsoleScreenBufferInfo(console, &bufferInfo);
    cursorPosition = bufferInfo.dwCursorPosition;

    // 커서를 현재 줄의 맨 앞으로 이동
    cursorPosition.X = 0;
    SetConsoleCursorPosition(console, cursorPosition);

    // 현재 줄을 공백으로 채우기
    DWORD numCharsWritten;
    DWORD numRows = bufferInfo.srWindow.Right - bufferInfo.srWindow.Left + 1;
    FillConsoleOutputCharacter(console, ' ', numRows, cursorPosition, &numCharsWritten);

    // 커서를 다시 현재 위치로 이동
    SetConsoleCursorPosition(console, cursorPosition);
}