#pragma once
#include "Student.h"
#include <map>
#include <string>
#include <vector>

// ============================================================
//  FileManager — 負責所有 fstream 讀寫操作
// ============================================================
class FileManager {
public:
    // 使用者清單
    static void loadUsers(std::map<std::string, Student*>& users);
    static void saveUsers(const std::map<std::string, Student*>& users);

    // 交易記錄
    static void loadTransactions(const std::string& studentId,
                                 std::vector<Transaction*>& transactions);
    static void saveTransactions(const std::string& studentId,
                                 const std::vector<Transaction*>& transactions);

    // 預算設定
    static void loadBudgets(const std::string& studentId,
                            std::map<std::string, double>& budgets);
    static void saveBudgets(const std::string& studentId,
                            const std::map<std::string, double>& budgets);

private:
    static std::string usersFile()                      { return "data/users.txt"; }
    static std::string txnFile(const std::string& id)   { return "data/txn_" + id + ".txt"; }
    static std::string budgetFile(const std::string& id){ return "data/budget_" + id + ".txt"; }

    static Transaction* parseTxnBlock(const std::string& block);
};
