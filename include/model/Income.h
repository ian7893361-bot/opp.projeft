#pragma once
#include "model/Transaction.h"

// ============================================================
//  Income — 收入，繼承 Transaction
// ============================================================
class Income : public Transaction {
public:
    Income(const std::string& id,
           const std::string& category,
           double             amount,
           const std::string& date,
           const std::string& note)
        : Transaction(id, category, amount, date, note) {}

    int         getSign()       const override { return +1; }
    std::string getTypeString() const override { return "INCOME"; }
    std::string getKindLabel()  const override { return "收入"; }
};
