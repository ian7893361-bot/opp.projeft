#include "auth/AuthUI.h"
#include "common/ConsoleUtils.h"
#include "transaction_ui/TransactionUI.h"
#include "budget/BudgetUI.h"
#include "report/ReportUI.h"
#include <iostream>
#include <iomanip>

void AuthUI::showLoggedInMenu(UserManager& um) {
    while (true) {
        Student* s = um.getCurrentUser();
        ConsoleUtils::clearScreen();
        ConsoleUtils::printHeader("個人財務管理");
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "  歡迎，" << s->getName()
                  << "（" << s->getId() << "）"
                  << "  |  餘額：$" << s->getBalance() << "\n\n";
        std::cout << "  1. 新增收入\n";
        std::cout << "  2. 新增支出\n";
        std::cout << "  3. 查詢記錄\n";
        std::cout << "  4. 刪除記錄\n";
        std::cout << "  5. 預算管理\n";
        std::cout << "  6. 匯出報表\n";
        std::cout << "  7. 帳戶資訊\n";
        std::cout << "  0. 登出\n";
        std::cout << "選擇：";

        int ch = ConsoleUtils::inputInt("");

        switch (ch) {
            case 1: TransactionUI::showIncomeMenu(s);        break;
            case 2: TransactionUI::showExpenseMenu(s);       break;
            case 3: TransactionUI::showQueryMenu(s);         break;
            case 4: TransactionUI::showDeleteMenu(s);        break;
            case 5: BudgetUI::showBudgetMenu(s);             break;
            case 6: ReportUI::showExportReport(s);           ConsoleUtils::pause(); break;
            case 7: ConsoleUtils::clearScreen(); s->display(); ConsoleUtils::pause(); break;
            case 0: um.logout(); return;
            default: std::cout << "  ✗ 無效選擇\n"; ConsoleUtils::pause();
        }
    }
}

void AuthUI::showMainMenu(UserManager& um) {
    while (true) {
        ConsoleUtils::clearScreen();
        ConsoleUtils::printHeader("學生個人財務管理系統 v1.0");
        std::cout << "  1. 登入\n";
        std::cout << "  2. 註冊新帳戶\n";
        std::cout << "  0. 結束\n";
        std::cout << "選擇：";

        int ch = ConsoleUtils::inputInt("");

        if (ch == 0) {
            std::cout << "\n  感謝使用，再見！\n";
            break;
        } else if (ch == 1) {
            std::string sid = ConsoleUtils::inputNonEmpty("輸入學號：");
            Student* s = um.login(sid);
            if (!s) {
                std::cout << "  ✗ 帳戶不存在，請先註冊。\n";
                ConsoleUtils::pause();
            } else {
                showLoggedInMenu(um);
            }
        } else if (ch == 2) {
            std::string sid  = ConsoleUtils::inputNonEmpty("輸入學號：");
            std::string name = ConsoleUtils::inputNonEmpty("輸入姓名：");
            if (um.registerStudent(sid, name)) {
                std::cout << "  ✓ 註冊成功！請重新登入。\n";
            } else {
                std::cout << "  ✗ 此學號已被使用，請換一個。\n";
            }
            ConsoleUtils::pause();
        } else {
            std::cout << "  ✗ 無效選擇\n";
            ConsoleUtils::pause();
        }
    }
}
