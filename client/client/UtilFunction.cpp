#include "UtilFunction.h"
#include <iostream>
#include <windows.h>

void UtilFunction::ClearConsoleLine()
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD cursorPosition;
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;

    // ���� Ŀ�� ��ġ ��������
    GetConsoleScreenBufferInfo(console, &bufferInfo);
    cursorPosition = bufferInfo.dwCursorPosition;

    // Ŀ���� ���� ���� �� ������ �̵�
    cursorPosition.X = 0;
    SetConsoleCursorPosition(console, cursorPosition);

    // ���� ���� �������� ä���
    DWORD numCharsWritten;
    DWORD numRows = bufferInfo.srWindow.Right - bufferInfo.srWindow.Left + 1;
    FillConsoleOutputCharacter(console, ' ', numRows, cursorPosition, &numCharsWritten);

    // Ŀ���� �ٽ� ���� ��ġ�� �̵�
    SetConsoleCursorPosition(console, cursorPosition);
}