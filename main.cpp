#include "auth/UserManager.h"
#include "auth/AuthUI.h"
#ifdef _WIN32
  #include <windows.h>
#endif
#include <iostream>
#include "common/ConsoleUtils.h"

int main() {
    // ── 設定 UTF-8 輸出與 ANSI 支援（修正 Windows 終端機中文亂碼與色彩）──
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
        SetConsoleMode(hOut, dwMode);
    }
#endif

    // 確保 data / reports 目錄存在
    ConsoleUtils::createDirectory("data");
    ConsoleUtils::createDirectory("reports");

    UserManager um;
    AuthUI::showMainMenu(um);
    return 0;
}
