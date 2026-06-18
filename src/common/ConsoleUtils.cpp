#include "common/ConsoleUtils.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <ctime>
#include <sstream>
#include <cstdlib>

namespace ConsoleUtils {

static int txnCounter = 1;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause() {
    std::cout << "\n按 Enter 繼續...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

std::string today() {
    auto now  = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_ptr = std::localtime(&t);
    char buf[16];
    if (tm_ptr) {
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm_ptr);
    } else {
        buf[0] = '\0';
    }
    return buf;
}

std::string newTxnId() {
    std::ostringstream ss;
    ss << "T" << std::setw(5) << std::setfill('0') << txnCounter++;
    return ss.str();
}

int getUtf8Width(const std::string& str) {
    int width = 0;
    size_t i = 0;
    while (i < str.length()) {
        unsigned char c = str[i];
        if (c < 0x80) {
            width += 1;
            i += 1;
        } else if ((c & 0xE0) == 0xC0) {
            width += 1;
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            width += 2;
            i += 3;
        } else if ((c & 0xF8) == 0xF0) {
            width += 2;
            i += 4;
        } else {
            i += 1;
        }
    }
    return width;
}

void printHeader(const std::string& title) {
    const int totalWidth = 44;
    const int innerWidth = totalWidth - 2;
    int titleWidth = getUtf8Width(title);
    
    int currentInnerWidth = innerWidth;
    if (titleWidth > innerWidth) {
        currentInnerWidth = titleWidth + 4;
    }
    
    int leftPadding = (currentInnerWidth - titleWidth) / 2;
    int rightPadding = currentInnerWidth - titleWidth - leftPadding;

    std::cout << "\n\033[1;36m╔";
    for (int i = 0; i < currentInnerWidth; ++i) std::cout << "═";
    std::cout << "╗\n";
    std::cout << "║\033[0m" << std::string(leftPadding, ' ') 
              << "\033[1;37m" << title << "\033[0m" 
              << std::string(rightPadding, ' ') << "\033[1;36m║\n";
    std::cout << "╚";
    for (int i = 0; i < currentInnerWidth; ++i) std::cout << "═";
    std::cout << "╝\033[0m\n";
}

double inputAmount(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string s;
        std::cin >> s;
        try {
            double v = std::stod(s);
            if (v <= 0) { std::cout << "  ✗ 金額必須大於零，請重新輸入。\n"; continue; }
            return v;
        } catch (...) {
            std::cout << "  ✗ 輸入無效，請輸入數字。\n";
        }
    }
}

std::string inputNonEmpty(const std::string& prompt) {
    std::string s;
    while (true) {
        std::cout << prompt;
        std::cin >> s;
        if (!s.empty()) return s;
        std::cout << "  ✗ 不可為空，請重新輸入。\n";
    }
}

void createDirectory(const std::string& path) {
#ifdef _WIN32
    std::string winPath = path;
    for (char& c : winPath) {
        if (c == '/') c = '\\';
    }
    std::string cmd = "if not exist \"" + winPath + "\" mkdir \"" + winPath + "\" >nul 2>&1";
    system(cmd.c_str());
#else
    std::string cmd = "mkdir -p \"" + path + "\" >/dev/null 2>&1";
    system(cmd.c_str());
#endif
}

void setTxnCounter(int val) {
    txnCounter = val;
}

int inputInt(const std::string& prompt) {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) {
            return val;
        }
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "  ✗ 輸入無效，請輸入一個整數數字。\n";
    }
}

} // namespace ConsoleUtils
