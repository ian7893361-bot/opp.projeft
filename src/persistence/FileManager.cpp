#include "persistence/FileManager.h"
#include "model/Income.h"
#include "model/FixedExpense.h"
#include "model/OneTimeExpense.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include "common/ConsoleUtils.h"

// ── 使用者清單 ────────────────────────────────────────────────
void FileManager::loadUsers(std::map<std::string, Student*>& users) {
    std::ifstream fin(usersFile());
    if (!fin.is_open()) return;  // 首次執行，檔案不存在屬正常

    std::string line;
    std::string sid, sname;
    while (std::getline(fin, line)) {
        if (line.rfind("ID=", 0) == 0)   sid   = line.substr(3);
        else if (line.rfind("NAME=", 0) == 0) sname = line.substr(5);
        else if (line == "---" && !sid.empty()) {
            users[sid] = new Student(sid, sname);
            sid.clear(); sname.clear();
        }
    }
}

void FileManager::saveUsers(const std::map<std::string, Student*>& users) {
    std::ofstream fout(usersFile());
    for (const auto& pair : users) {
        fout << "ID="   << pair.second->getId()   << "\n";
        fout << "NAME=" << pair.second->getName() << "\n";
        fout << "---\n";
    }
}

// ── 交易記錄 ─────────────────────────────────────────────────
Transaction* FileManager::parseTxnBlock(const std::string& block) {
    std::map<std::string, std::string> kv;
    std::istringstream ss(block);
    std::string line;
    while (std::getline(ss, line)) {
        auto pos = line.find('=');
        if (pos != std::string::npos)
            kv[line.substr(0, pos)] = line.substr(pos + 1);
    }
    if (kv.count("ID") == 0 || kv.count("TYPE") == 0) return nullptr;

    const std::string& id  = kv["ID"];
    const std::string& cat = kv["CATEGORY"];
    double             amt = 0.0;
    try { amt = std::stod(kv["AMOUNT"]); } catch (...) { return nullptr; }
    const std::string& dt  = kv["DATE"];
    const std::string& nt  = kv.count("NOTE") ? kv["NOTE"] : "";
    const std::string& tp  = kv["TYPE"];

    if (tp == "INCOME")          return new Income(id, cat, amt, dt, nt);
    if (tp == "EXPENSE_FIXED")   return new FixedExpense(id, cat, amt, dt, nt);
    if (tp == "EXPENSE_ONETIME") return new OneTimeExpense(id, cat, amt, dt, nt);
    return nullptr;
}

void FileManager::loadTransactions(const std::string& studentId,
                                   std::vector<Transaction*>& transactions) {
    std::ifstream fin(txnFile(studentId));
    if (!fin.is_open()) return;

    std::string block, line;
    while (std::getline(fin, line)) {
        if (line == "---") {
            if (!block.empty()) {
                Transaction* t = parseTxnBlock(block);
                if (t) transactions.push_back(t);
                block.clear();
            }
        } else {
            block += line + "\n";
        }
    }
}

void FileManager::saveTransactions(const std::string& studentId,
                                   const std::vector<Transaction*>& transactions) {
    ConsoleUtils::createDirectory("data/" + studentId);
    std::ofstream fout(txnFile(studentId));
    for (const Transaction* t : transactions) {
        fout << "ID="       << t->getId()         << "\n";
        fout << "TYPE="     << t->getTypeString()  << "\n";
        fout << "CATEGORY=" << t->getCategory()    << "\n";
        fout << "AMOUNT="   << std::fixed << std::setprecision(2) << t->getAmount() << "\n";
        fout << "DATE="     << t->getDate()        << "\n";
        fout << "NOTE="     << t->getNote()        << "\n";
        fout << "---\n";
    }
}

// ── 預算設定 ─────────────────────────────────────────────────
void FileManager::loadBudgets(const std::string& studentId,
                              std::map<std::string, double>& budgets) {
    std::ifstream fin(budgetFile(studentId));
    if (!fin.is_open()) return;
    std::string line;
    while (std::getline(fin, line)) {
        auto pos = line.find('=');
        if (pos != std::string::npos) {
            std::string cat = line.substr(0, pos);
            double      amt = 0.0;
            try { amt = std::stod(line.substr(pos + 1)); } catch (...) {}
            budgets[cat] = amt;
        }
    }
}

void FileManager::saveBudgets(const std::string& studentId,
                              const std::map<std::string, double>& budgets) {
    ConsoleUtils::createDirectory("data/" + studentId);
    std::ofstream fout(budgetFile(studentId));
    for (const auto& pair : budgets) {
        fout << pair.first << "=" << std::fixed << std::setprecision(2) << pair.second << "\n";
    }
}
