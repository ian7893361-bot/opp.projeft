#pragma once
#include <string>
#include <iostream>
#include <iomanip>

// ============================================================
//  Transaction — 抽象基底類別，代表一筆財務交易
// ============================================================
class Transaction {
protected:
    std::string id;
    std::string category;
    double      amount;
    std::string date;    // 格式 YYYY-MM-DD
    std::string note;

public:
    Transaction(const std::string& id,
                const std::string& category,
                double             amount,
                const std::string& date,
                const std::string& note)
        : id(id), category(category), amount(amount), date(date), note(note) {}

    virtual ~Transaction() = default;

    // Pure virtual functions
    virtual int         getSign()       const = 0;  // +1 for income, -1 for expense
    virtual std::string getTypeString() const = 0;  // "INCOME", "EXPENSE_FIXED", "EXPENSE_ONETIME"
    virtual std::string getKindLabel()  const = 0;  // 顯示用：收入 / 固定支出 / 臨時支出

    // Concrete accessors
    std::string getId()       const { return id; }
    std::string getCategory() const { return category; }
    double      getAmount()   const { return amount; }
    std::string getDate()     const { return date; }
    std::string getNote()     const { return note; }

    // 取得月份字串 YYYY-MM
    std::string getYearMonth() const {
        return date.size() >= 7 ? date.substr(0, 7) : "";
    }

    // 格式化顯示一筆交易
    void print() const {
        std::cout << std::left
                  << std::setw(12) << date
                  << std::setw(12) << getKindLabel()
                  << std::setw(14) << category
                  << std::right << std::setw(10) << std::fixed << std::setprecision(2)
                  << (getSign() > 0 ? "+" : "-") << amount
                  << "  " << note << "\n";
    }
};
