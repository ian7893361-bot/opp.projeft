#include "budget/BudgetUI.h"
#include "common/ConsoleUtils.h"
#include "transaction_ui/TransactionUI.h"
#include "persistence/FileManager.h"
#include <iostream>
#include <iomanip>
#include <map>

void BudgetUI::checkBudget(const Student* student,
                           const std::string& category,
                           const std::string& yearMonth) {
    double budget = student->getBudget(category);
    if (budget <= 0) return;  // 未設定預算

    // 計算當月此類別累計支出
    double total = 0.0;
    for (const Transaction* t : student->getTransactions()) {
        if (t->getSign() < 0 &&
            t->getCategory() == category &&
            t->getYearMonth() == yearMonth) {
            total += t->getAmount();
        }
    }
    if (total > budget) {
        std::cout << "\n  ⚠  [預算警示] " << category
                  << " 已超出預算！\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "     預算：$" << budget
                  << " | 實際：$" << total
                  << " | 超出：$" << (total - budget) << "\n";
    }
}

void BudgetUI::showBudgetMenu(Student* student) {
    while (true) {
        ConsoleUtils::clearScreen();
        ConsoleUtils::printHeader("預算管理");
        std::cout << "  1. 設定 / 修改預算\n";
        std::cout << "  2. 查看預算狀況\n";
        std::cout << "  0. 返回\n";
        int ch = ConsoleUtils::inputInt("");

        if (ch == 0) break;

        if (ch == 1) {
            // 設定預算
            std::string category = TransactionUI::selectExpenseCategory();
            if (category.empty()) continue;
            double amount = ConsoleUtils::inputAmount("預算金額：$");
            student->setBudget(category, amount);
            FileManager::saveBudgets(student->getId(), student->getBudgets());
            std::cout << "  ✓ 已設定 " << category << " 的月預算為 $"
                      << std::fixed << std::setprecision(2) << amount << "\n";
        } else if (ch == 2) {
            // 查看預算狀況
            const auto& budgets = student->getBudgets();
            if (budgets.empty()) {
                std::cout << "\n  （尚未設定任何預算）\n";
            } else {
                // 計算當月各類別支出
                std::string ym = ConsoleUtils::today().substr(0, 7);
                std::map<std::string, double> catSpend;
                for (const Transaction* t : student->getTransactions()) {
                    if (t->getSign() < 0 && t->getYearMonth() == ym)
                        catSpend[t->getCategory()] += t->getAmount();
                }

                std::cout << "\n";
                std::cout << std::left
                          << std::setw(14) << "類別"
                          << std::right << std::setw(10) << "預算"
                          << std::setw(10) << "已使用"
                          << std::setw(10) << "剩餘"
                          << "  狀態\n";
                std::cout << std::string(56, '-') << "\n";
                for (const auto& pair : budgets) {
                    const std::string& cat = pair.first;
                    double budget = pair.second;
                    double used   = catSpend.count(cat) ? catSpend[cat] : 0.0;
                    double remain = budget - used;
                    std::cout << std::left  << std::setw(14) << cat
                              << std::right << std::fixed << std::setprecision(2)
                              << std::setw(10) << budget
                              << std::setw(10) << used
                              << std::setw(10) << remain
                              << "  " << (remain < 0 ? "⚠ 超支" : "正常") << "\n";
                }
            }
        }
        ConsoleUtils::pause();
    }
}
