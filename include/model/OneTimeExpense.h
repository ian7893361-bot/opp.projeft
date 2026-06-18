#pragma once
#include "model/Expense.h"

// ============================================================
//  OneTimeExpense — 臨時支出（餐費、娛樂等），繼承 Expense
// ============================================================
class OneTimeExpense : public Expense {
public:
    OneTimeExpense(const std::string& id,
                   const std::string& category,
                   double             amount,
                   const std::string& date,
                   const std::string& note)
        : Expense(id, category, amount, date, note) {}

    std::string getTypeString() const override { return "EXPENSE_ONETIME"; }
    std::string getKindLabel()  const override { return "臨時支出"; }
};
