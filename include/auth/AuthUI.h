#pragma once
#include "auth/UserManager.h"

class AuthUI {
public:
    static void showMainMenu(UserManager& um);
    static void showLoggedInMenu(UserManager& um);
};
