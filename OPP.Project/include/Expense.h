#pragma once
#include "Transaction.h"

// ============================================================
//  Expense — 支出抽象類別，繼承 Transaction
// ============================================================
class Expense : public Transaction {
public:
    Expense(const std::string& id,
            const std::string& category,
            double             amount,
            const std::string& date,
            const std::string& note)
        : Transaction(id, category, amount, date, note) {}

    int getSign() const override { return -1; }
    // getTypeString() 與 getKindLabel() 仍為 pure virtual，由子類實作
    virtual std::string getTypeString() const = 0;
    virtual std::string getKindLabel()  const = 0;
};
