#include "ReportGenerator.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <map>
#include <vector>

std::string ReportGenerator::reportPath(const std::string& studentId,
                                        const std::string& yearMonth) {
    // yearMonth = "2026-06" → strip dash → "202606"
    std::string ym = yearMonth;
    ym.erase(std::remove(ym.begin(), ym.end(), '-'), ym.end());
    return "reports/report_" + studentId + "_" + ym + ".txt";
}

bool ReportGenerator::generate(const Student* student,
                                const std::string& yearMonth) {
    const auto& allTxn = student->getTransactions();

    // 篩選指定月份的交易
    std::vector<Transaction*> monthly;
    std::copy_if(allTxn.begin(), allTxn.end(), std::back_inserter(monthly),
        [&yearMonth](Transaction* t){ return t->getYearMonth() == yearMonth; });

    if (monthly.empty()) return false;

    // 依日期排序
    std::sort(monthly.begin(), monthly.end(),
        [](Transaction* a, Transaction* b){ return a->getDate() < b->getDate(); });

    // 統計
    double totalIncome  = 0.0, totalExpense = 0.0;
    std::map<std::string, double> catExpense;  // 各類別支出
    std::vector<Transaction*> incomes, expenses;

    for (Transaction* t : monthly) {
        if (t->getSign() > 0) {
            totalIncome += t->getAmount();
            incomes.push_back(t);
        } else {
            totalExpense += t->getAmount();
            catExpense[t->getCategory()] += t->getAmount();
            expenses.push_back(t);
        }
    }

    // 寫報表
    std::ofstream fout(reportPath(student->getId(), yearMonth));
    const auto line80 = std::string(56, '=');
    const auto line80d = std::string(56, '-');

    fout << line80 << "\n";
    fout << "  財務管理系統 — 月份報表\n";
    fout << "  學生：" << student->getName()
         << "（" << student->getId() << "）\n";
    fout << "  月份：" << yearMonth << "\n";
    fout << line80 << "\n\n";

    // 摘要
    fout << "【摘要】\n" << line80d << "\n";
    fout << std::fixed << std::setprecision(2);
    fout << "  總收入：  $" << std::setw(10) << totalIncome  << "\n";
    fout << "  總支出：  $" << std::setw(10) << totalExpense << "\n";
    fout << "  結  餘：  $" << std::setw(10) << (totalIncome - totalExpense) << "\n\n";

    // 收入明細
    fout << "【收入明細】\n" << line80d << "\n";
    fout << std::left
         << std::setw(12) << "日期"
         << std::setw(14) << "類別"
         << std::right << std::setw(10) << "金額"
         << "  備註\n" << line80d << "\n";
    for (Transaction* t : incomes) {
        fout << std::left
             << std::setw(12) << t->getDate()
             << std::setw(14) << t->getCategory()
             << std::right << std::setw(9) << "+" << std::setw(9) << t->getAmount()
             << "  " << t->getNote() << "\n";
    }
    fout << "\n";

    // 支出明細
    fout << "【支出明細】\n" << line80d << "\n";
    fout << std::left
         << std::setw(12) << "日期"
         << std::setw(10) << "類型"
         << std::setw(14) << "類別"
         << std::right << std::setw(10) << "金額"
         << "  備註\n" << line80d << "\n";
    for (Transaction* t : expenses) {
        fout << std::left
             << std::setw(12) << t->getDate()
             << std::setw(10) << t->getKindLabel()
             << std::setw(14) << t->getCategory()
             << std::right << std::setw(9) << "-" << std::setw(9) << t->getAmount()
             << "  " << t->getNote() << "\n";
    }
    fout << "\n";

    // 分類統計
    fout << "【分類統計】\n" << line80d << "\n";
    for (const auto& [cat, amt] : catExpense) {
        fout << std::left << std::setw(16) << cat
             << std::right << std::setw(10) << amt << "\n";
    }
    fout << "\n";

    // 預算對比
    fout << "【預算對比】\n" << line80d << "\n";
    fout << std::left
         << std::setw(14) << "類別"
         << std::right << std::setw(10) << "預算"
         << std::setw(10) << "實際"
         << std::setw(10) << "剩餘"
         << "  狀態\n" << line80d << "\n";

    const auto& budgets = student->getBudgets();
    bool anyBudget = false;
    for (const auto& [cat, budget] : budgets) {
        double actual = catExpense.count(cat) ? catExpense[cat] : 0.0;
        double remain = budget - actual;
        bool   over   = remain < 0;
        fout << std::left  << std::setw(14) << cat
             << std::right << std::setw(10) << budget
             << std::setw(10) << actual
             << std::setw(10) << remain
             << "  " << (over ? "⚠ 超支 $" + std::to_string(-remain).substr(0,6) : "正常")
             << "\n";
        anyBudget = true;
    }
    if (!anyBudget) fout << "  （尚未設定預算）\n";

    fout << "\n" << line80 << "\n";
    fout.close();
    return true;
}
