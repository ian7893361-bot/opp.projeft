#pragma once
#include "model/Student.h"
#include <string>

class BudgetUI {
public:
    static void showBudgetMenu(Student* student);
    static void checkBudget(const Student* student,
                            const std::string& category,
                            const std::string& yearMonth);
};
