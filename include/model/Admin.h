#pragma once
#include "model/Person.h"

// ============================================================
//  Admin — 繼承 Person，具有管理員權限旗標
// ============================================================
class Admin : public Person {
private:
    bool canManageUsers;

public:
    Admin(const std::string& adminId, const std::string& name)
        : Person(adminId, name), canManageUsers(true) {}

    void        display() const override;
    std::string getType() const override { return "Admin"; }

    bool getCanManageUsers() const { return canManageUsers; }
};
