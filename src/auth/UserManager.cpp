#include "auth/UserManager.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include "common/ConsoleUtils.h"

UserManager::UserManager() {
    FileManager::loadUsers(users);
}

UserManager::~UserManager() {
    for (auto& pair : users) delete pair.second;
    users.clear();
}

bool UserManager::registerStudent(const std::string& studentId,
                                  const std::string& name) {
    if (users.count(studentId)) return false;  // 學號重複
    users[studentId] = new Student(studentId, name);
    FileManager::saveUsers(users);
    return true;
}

Student* UserManager::login(const std::string& studentId) {
    auto it = users.find(studentId);
    if (it == users.end()) return nullptr;
    currentUser = it->second;

    // 清空現有交易物件與預算以防重複載入
    auto& txns = currentUser->getTransactions();
    for (Transaction* t : txns) {
        delete t;
    }
    txns.clear();
    currentUser->getBudgets().clear();
    currentUser->addToBalance(-currentUser->getBalance());

    // 檢查並遷移舊版資料
    std::string oldTxn = "data/txn_" + studentId + ".txt";
    std::string newTxn = "data/" + studentId + "/txn.txt";
    std::string oldBudget = "data/budget_" + studentId + ".txt";
    std::string newBudget = "data/" + studentId + "/budget.txt";

    std::ifstream fTxn(oldTxn);
    std::ifstream fBudget(oldBudget);
    bool hasOldTxn = fTxn.good();
    bool hasOldBudget = fBudget.good();
    fTxn.close();
    fBudget.close();

    if (hasOldTxn || hasOldBudget) {
        ConsoleUtils::createDirectory("data/" + studentId);
        if (hasOldTxn) {
            std::rename(oldTxn.c_str(), newTxn.c_str());
        }
        if (hasOldBudget) {
            std::rename(oldBudget.c_str(), newBudget.c_str());
        }
    }

    // 載入交易與預算
    FileManager::loadTransactions(studentId, currentUser->getTransactions());

    // 找出目前使用者最大交易 ID 數字，並更新 txnCounter
    int maxIdVal = 0;
    for (const auto* t : currentUser->getTransactions()) {
        std::string tid = t->getId();
        if (tid.size() > 1 && tid[0] == 'T') {
            try {
                int val = std::stoi(tid.substr(1));
                if (val > maxIdVal) maxIdVal = val;
            } catch (...) {}
        }
    }
    ConsoleUtils::setTxnCounter(maxIdVal + 1);
    // 重算餘額
    double bal = 0.0;
    for (auto* t : currentUser->getTransactions())
        bal += t->getSign() * t->getAmount();
    // 直接重設 balance（透過多次 addToBalance 會重複，所以這裡手動對齊）
    // 清空餘額再用 addTransaction 是最安全的做法，但我們已有 getTransactions()
    // 改以 getter/setter 方式
    currentUser->addToBalance(bal - currentUser->getBalance());
    FileManager::loadBudgets(studentId, currentUser->getBudgets());
    return currentUser;
}

void UserManager::logout() {
    if (!currentUser) return;
    FileManager::saveTransactions(currentUser->getId(),
                                  currentUser->getTransactions());
    FileManager::saveBudgets(currentUser->getId(),
                              currentUser->getBudgets());
    currentUser = nullptr;
}
