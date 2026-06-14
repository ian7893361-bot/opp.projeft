#pragma once
#include "Student.h"
#include "FileManager.h"
#include <map>
#include <string>

// ============================================================
//  UserManager — 管理多使用者登入 / 登出 / 註冊
//  使用 STL map<string, Student*> 以學號為索引
// ============================================================
class UserManager {
private:
    std::map<std::string, Student*> users;  // 全域使用者清單
    Student* currentUser = nullptr;          // 目前登入的使用者

public:
    UserManager();
    ~UserManager();

    // 帳戶操作
    bool    registerStudent(const std::string& studentId, const std::string& name);
    Student* login(const std::string& studentId);
    void    logout();

    // 存取
    Student*        getCurrentUser()  const { return currentUser; }
    bool            isLoggedIn()      const { return currentUser != nullptr; }
    const std::map<std::string, Student*>& getUsers() const { return users; }
};
