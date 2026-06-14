#pragma once
#include "Expense.h"

// ============================================================
//  FixedExpense — 固定支出（學費、房租等），繼承 Expense
// ============================================================
class FixedExpense : public Expense {
public:
    FixedExpense(const std::string& id,
                 const std::string& category,
                 double             amount,
                 const std::string& date,
                 const std::string& note)
        : Expense(id, category, amount, date, note) {}

    std::string getTypeString() const override { return "EXPENSE_FIXED"; }
    std::string getKindLabel()  const override { return "固定支出"; }
};
