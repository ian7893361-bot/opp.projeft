#pragma once
#include "model/Student.h"
#include <string>

class TransactionUI {
public:
    static std::string selectIncomeCategory();
    static std::string selectExpenseCategory();
    static void showIncomeMenu(Student* student);
    static void showExpenseMenu(Student* student);
    static void showQueryMenu(Student* student);
    static void showDeleteMenu(Student* student);
};
