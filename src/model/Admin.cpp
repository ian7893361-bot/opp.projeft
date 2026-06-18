#include "model/Admin.h"
#include <iostream>

void Admin::display() const {
    std::cout << "================================\n";
    std::cout << "  管理員帳戶\n";
    std::cout << "================================\n";
    std::cout << "  ID  ：" << id   << "\n";
    std::cout << "  姓名：" << name << "\n";
    std::cout << "  權限：" << (canManageUsers ? "可管理使用者" : "一般") << "\n";
    std::cout << "================================\n";
}
