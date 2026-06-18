#include "model/Student.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

// 解構子：釋放所有交易物件記憶體
Student::~Student() {
    for (Transaction* t : transactions) {
        delete t;
    }
    transactions.clear();
}

void Student::display() const {
    std::cout << "================================\n";
    std::cout << "  帳戶資訊\n";
    std::cout << "================================\n";
    std::cout << "  學號：" << id   << "\n";
    std::cout << "  姓名：" << name << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  餘額：$" << balance << "\n";
    std::cout << "================================\n";
}

void Student::addTransaction(Transaction* t) {
    transactions.push_back(t);
    balance += t->getSign() * t->getAmount();
}

bool Student::removeTransaction(const std::string& txnId) {
    auto it = std::find_if(transactions.begin(), transactions.end(),
        [&txnId](Transaction* t){ return t->getId() == txnId; });
    if (it == transactions.end()) return false;
    balance -= (*it)->getSign() * (*it)->getAmount();
    delete *it;
    transactions.erase(it);
    return true;
}

void Student::setBudget(const std::string& category, double amount) {
    budgets[category] = amount;
}

double Student::getBudget(const std::string& category) const {
    auto it = budgets.find(category);
    return (it != budgets.end()) ? it->second : 0.0;
}
