#include "UserManager.h"
#include <iostream>

UserManager::UserManager() {
    FileManager::loadUsers(users);
}

UserManager::~UserManager() {
    for (auto& [id, s] : users) delete s;
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
    // 載入交易與預算
    FileManager::loadTransactions(studentId, currentUser->getTransactions());
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
