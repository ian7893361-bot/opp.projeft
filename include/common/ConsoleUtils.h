#pragma once
#include <string>

namespace ConsoleUtils {
    void clearScreen();
    void pause();
    std::string today();
    std::string newTxnId();
    int getUtf8Width(const std::string& str);
    void printHeader(const std::string& title);
    double inputAmount(const std::string& prompt);
    std::string inputNonEmpty(const std::string& prompt);
    void createDirectory(const std::string& path);
    void setTxnCounter(int val);
    int inputInt(const std::string& prompt);
}
