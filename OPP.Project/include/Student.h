#pragma once
#include "Person.h"
#include "Transaction.h"
#include <vector>
#include <map>
#include <string>

// ============================================================
//  Student — 繼承 Person，擁有交易清單與預算設定
// ============================================================
class Student : public Person {
private:
    double balance;                          // 當前餘額（收入 - 支出）
    std::vector<Transaction*> transactions;  // 所有交易（STL vector）
    std::map<std::string, double> budgets;   // 類別預算（STL map）

public:
    Student(const std::string& studentId, const std::string& name)
        : Person(studentId, name), balance(0.0) {}

    ~Student() override;

    // Person interface
    void        display() const override;
    std::string getType() const override { return "Student"; }

    // Balance management
    double getBalance() const { return balance; }
    void   addToBalance(double amount) { balance += amount; }

    // Transaction management
    std::vector<Transaction*>& getTransactions() { return transactions; }
    const std::vector<Transaction*>& getTransactions() const { return transactions; }
    void addTransaction(Transaction* t);
    bool removeTransaction(const std::string& txnId);

    // Budget management
    std::map<std::string, double>& getBudgets() { return budgets; }
    const std::map<std::string, double>& getBudgets() const { return budgets; }
    void   setBudget(const std::string& category, double amount);
    double getBudget(const std::string& category) const;
};
