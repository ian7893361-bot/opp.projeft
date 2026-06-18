#include "transaction_ui/TransactionUI.h"
#include "common/ConsoleUtils.h"
#include "budget/BudgetUI.h"
#include "persistence/FileManager.h"
#include "model/Income.h"
#include "model/FixedExpense.h"
#include "model/OneTimeExpense.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

std::string TransactionUI::selectIncomeCategory() {
    const std::vector<std::string> cats = {
        "打工", "獎學金", "紅包", "家人", "其他收入"
    };
    ConsoleUtils::printHeader("選擇收入類別");
    std::cout << "  0. 退出\n";
    for (int i = 0; i < (int)cats.size(); i++)
        std::cout << "  " << (i+1) << ". " << cats[i] << "\n";
    std::cout << "  " << cats.size()+1 << ". 自訂\n";
    int ch = ConsoleUtils::inputInt("");
    if (ch == 0) return "";  // 退出信號
    if (ch >= 1 && ch <= (int)cats.size()) return cats[ch-1];
    return ConsoleUtils::inputNonEmpty("輸入自訂類別：");
}

std::string TransactionUI::selectExpenseCategory() {
    const std::vector<std::string> cats = {
        "餐費", "交通", "學費", "住宿", "娛樂", "購物", "醫療", "其他支出"
    };
    ConsoleUtils::printHeader("選擇支出類別");
    std::cout << "  0. 退出\n";
    for (int i = 0; i < (int)cats.size(); i++)
        std::cout << "  " << (i+1) << ". " << cats[i] << "\n";
    std::cout << "  " << cats.size()+1 << ". 自訂\n";
    int ch = ConsoleUtils::inputInt("");
    if (ch == 0) return "";  // 退出信號
    if (ch >= 1 && ch <= (int)cats.size()) return cats[ch-1];
    return ConsoleUtils::inputNonEmpty("輸入自訂類別：");
}

void TransactionUI::showIncomeMenu(Student* student) {
    while (true) {
        ConsoleUtils::clearScreen();
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "\n  當前餘額：$" << student->getBalance() << "\n";
        std::string category = selectIncomeCategory();
        if (category.empty()) break;  // 選 0 退出

        double amount = ConsoleUtils::inputAmount("金額：$");
        std::cout << "日期（預設今天 " << ConsoleUtils::today() << "，Enter 接受）：";
        std::cin.ignore();
        std::string date;
        std::getline(std::cin, date);
        if (date.empty()) date = ConsoleUtils::today();
        std::cout << "備註（可留空）：";
        std::string note;
        std::getline(std::cin, note);

        Income* inc = new Income(ConsoleUtils::newTxnId(), category, amount, date, note);
        student->addTransaction(inc);
        FileManager::saveTransactions(student->getId(), student->getTransactions());
        std::cout << "\n  ✓ 已新增收入：+" << std::fixed << std::setprecision(2)
                  << amount << "（" << category << "）\n";
        std::cout << "  當前餘額：$" << student->getBalance() << "\n";
        ConsoleUtils::pause();
    }
}

void TransactionUI::showExpenseMenu(Student* student) {
    while (true) {
        ConsoleUtils::clearScreen();
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "\n  當前餘額：$" << student->getBalance() << "\n";
        std::string category = selectExpenseCategory();
        if (category.empty()) break;  // 選 0 退出

        std::cout << "\n支出類型：\n";
        std::cout << "  1. 固定支出（學費、房租等）\n";
        std::cout << "  2. 臨時支出（餐費、娛樂等）\n";
        int typeChoice = ConsoleUtils::inputInt("");

        double amount = ConsoleUtils::inputAmount("金額：$");
        std::cout << "日期（預設今天 " << ConsoleUtils::today() << "，Enter 接受）：";
        std::cin.ignore();
        std::string date;
        std::getline(std::cin, date);
        if (date.empty()) date = ConsoleUtils::today();
        std::cout << "備註（可留空）：";
        std::string note;
        std::getline(std::cin, note);

        std::string yearMonth = date.size() >= 7 ? date.substr(0, 7) : "";
        Transaction* exp = (typeChoice == 1)
            ? (Transaction*) new FixedExpense(ConsoleUtils::newTxnId(), category, amount, date, note)
            : (Transaction*) new OneTimeExpense(ConsoleUtils::newTxnId(), category, amount, date, note);

        student->addTransaction(exp);
        FileManager::saveTransactions(student->getId(), student->getTransactions());
        std::cout << "\n  ✓ 已新增支出：-" << std::fixed << std::setprecision(2)
                  << amount << "（" << category << "）\n";
        std::cout << "  當前餘額：$" << student->getBalance() << "\n";
        if (!yearMonth.empty()) BudgetUI::checkBudget(student, category, yearMonth);
        ConsoleUtils::pause();
    }
}

void TransactionUI::showQueryMenu(Student* student) {
    while (true) {
        ConsoleUtils::clearScreen();
        ConsoleUtils::printHeader("查詢記錄");
        std::cout << "  1. 全部記錄\n";
        std::cout << "  2. 依月份篩選\n";
        std::cout << "  0. 返回\n";
        int ch = ConsoleUtils::inputInt("");
        if (ch == 0) break;
        if (ch != 1 && ch != 2) { std::cout << "  ✗ 無效選擇\n"; ConsoleUtils::pause(); continue; }

        std::string filter = "";
        if (ch == 2) filter = ConsoleUtils::inputNonEmpty("輸入年月（格式 YYYY-MM）：");

        auto& txns = student->getTransactions();
        std::sort(txns.begin(), txns.end(),
            [](Transaction* a, Transaction* b){ return a->getDate() < b->getDate(); });

        std::vector<Transaction*> show;
        std::copy_if(txns.begin(), txns.end(), std::back_inserter(show),
            [&filter](Transaction* t){
                return filter.empty() || t->getYearMonth() == filter;
            });

        if (show.empty()) {
            std::cout << "\n  （無符合的記錄）\n";
        } else {
            std::cout << "\n";
            std::cout << std::left
                      << std::setw(8)  << "ID"
                      << std::setw(12) << "日期"
                      << std::setw(10) << "類型"
                      << std::setw(14) << "類別"
                      << std::right << std::setw(10) << "金額"
                      << "  備註\n";
            std::cout << std::string(70, '-') << "\n";
            for (Transaction* t : show) {
                std::cout << std::left
                          << std::setw(8)  << t->getId()
                          << std::setw(12) << t->getDate()
                          << std::setw(10) << t->getKindLabel()
                          << std::setw(14) << t->getCategory()
                          << std::right << std::setw(8)
                          << (t->getSign() > 0 ? "+" : "-")
                          << std::fixed << std::setprecision(2)
                          << std::setw(9) << t->getAmount()
                          << "  " << t->getNote() << "\n";
            }
            std::cout << std::string(70, '-') << "\n";
            std::cout << "共 " << show.size() << " 筆\n";
        }
        ConsoleUtils::pause();
    }
}

void TransactionUI::showDeleteMenu(Student* student) {
    while (true) {
        ConsoleUtils::clearScreen();
        ConsoleUtils::printHeader("刪除記錄");
        std::cout << "  1. 刪除一筆（輸入交易 ID）\n";
        std::cout << "  0. 返回\n";
        int ch = ConsoleUtils::inputInt("");
        if (ch == 0) break;
        if (ch != 1) { std::cout << "  ✗ 無效選擇\n"; ConsoleUtils::pause(); continue; }

        std::string txnId = ConsoleUtils::inputNonEmpty("輸入要刪除的交易 ID：");
        std::cout << "確認刪除 " << txnId << "？（y/n）：";
        char confirm;
        std::cin >> confirm;
        if (confirm == 'y' || confirm == 'Y') {
            if (student->removeTransaction(txnId)) {
                FileManager::saveTransactions(student->getId(), student->getTransactions());
                std::cout << "  ✓ 已刪除。\n";
            } else {
                std::cout << "  ✗ 找不到該記錄（ID: " << txnId << "）\n";
            }
        } else {
            std::cout << "  已取消。\n";
        }
        ConsoleUtils::pause();
    }
}
