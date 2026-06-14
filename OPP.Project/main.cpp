#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <chrono>
#include <ctime>
#include <locale>
#ifdef _WIN32
  #include <windows.h>
#endif

#include "UserManager.h"
#include "Income.h"
#include "FixedExpense.h"
#include "OneTimeExpense.h"
#include "ReportGenerator.h"
#include "FileManager.h"

// ──────────────────────────────────────────────
//  工具函式
// ──────────────────────────────────────────────
static void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void pause() {
    std::cout << "\n按 Enter 繼續...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

static std::string today() {
    auto now  = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
    return buf;
}

// 產生唯一 ID（簡易版）
static int txnCounter = 1;
static std::string newTxnId() {
    std::ostringstream ss;
    ss << "T" << std::setw(5) << std::setfill('0') << txnCounter++;
    return ss.str();
}

static int getUtf8Width(const std::string& str) {
    int width = 0;
    size_t i = 0;
    while (i < str.length()) {
        unsigned char c = str[i];
        if (c < 0x80) {
            width += 1;
            i += 1;
        } else if ((c & 0xE0) == 0xC0) {
            width += 1;
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            width += 2;
            i += 3;
        } else if ((c & 0xF8) == 0xF0) {
            width += 2;
            i += 4;
        } else {
            i += 1;
        }
    }
    return width;
}

static void printHeader(const std::string& title) {
    const int totalWidth = 44;
    const int innerWidth = totalWidth - 2;
    int titleWidth = getUtf8Width(title);
    
    int currentInnerWidth = innerWidth;
    if (titleWidth > innerWidth) {
        currentInnerWidth = titleWidth + 4;
    }
    
    int leftPadding = (currentInnerWidth - titleWidth) / 2;
    int rightPadding = currentInnerWidth - titleWidth - leftPadding;

    // \033[1;36m = 粗體青色 (Cyan), \033[1;37m = 粗體亮白 (White), \033[0m = 重設顏色
    std::cout << "\n\033[1;36m╔";
    for (int i = 0; i < currentInnerWidth; ++i) std::cout << "═";
    std::cout << "╗\n";
    std::cout << "║\033[0m" << std::string(leftPadding, ' ') 
              << "\033[1;37m" << title << "\033[0m" 
              << std::string(rightPadding, ' ') << "\033[1;36m║\n";
    std::cout << "╚";
    for (int i = 0; i < currentInnerWidth; ++i) std::cout << "═";
    std::cout << "╝\033[0m\n";
}

// ──────────────────────────────────────────────
//  輸入輔助
// ──────────────────────────────────────────────
static double inputAmount(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string s;
        std::cin >> s;
        try {
            double v = std::stod(s);
            if (v <= 0) { std::cout << "  ✗ 金額必須大於零，請重新輸入。\n"; continue; }
            return v;
        } catch (...) {
            std::cout << "  ✗ 輸入無效，請輸入數字。\n";
        }
    }
}

static std::string inputNonEmpty(const std::string& prompt) {
    std::string s;
    while (true) {
        std::cout << prompt;
        std::cin >> s;
        if (!s.empty()) return s;
        std::cout << "  ✗ 不可為空，請重新輸入。\n";
    }
}



// ──────────────────────────────────────────────
//  預算警示
// ──────────────────────────────────────────────
static void checkBudget(const Student* student,
                        const std::string& category,
                        const std::string& yearMonth) {
    double budget = student->getBudget(category);
    if (budget <= 0) return;  // 未設定預算

    // 計算當月此類別累計支出
    double total = 0.0;
    for (const Transaction* t : student->getTransactions()) {
        if (t->getSign() < 0 &&
            t->getCategory() == category &&
            t->getYearMonth() == yearMonth) {
            total += t->getAmount();
        }
    }
    if (total > budget) {
        std::cout << "\n  ⚠  [預算警示] " << category
                  << " 已超出預算！\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "     預算：$" << budget
                  << " | 實際：$" << total
                  << " | 超出：$" << (total - budget) << "\n";
    }
}

// ──────────────────────────────────────────────
//  收入類別清單
// ──────────────────────────────────────────────
static std::string selectIncomeCategory() {
    const std::vector<std::string> cats = {
        "打工", "獎學金", "紅包", "家人", "其他收入"
    };
    printHeader("選擇收入類別");
    std::cout << "  0. 退出\n";
    for (int i = 0; i < (int)cats.size(); i++)
        std::cout << "  " << (i+1) << ". " << cats[i] << "\n";
    std::cout << "  " << cats.size()+1 << ". 自訂\n";
    int ch = 0;
    std::cout << "選擇：";
    std::cin >> ch;
    if (ch == 0) return "";  // 退出信號
    if (ch >= 1 && ch <= (int)cats.size()) return cats[ch-1];
    return inputNonEmpty("輸入自訂類別：");
}

// ──────────────────────────────────────────────
//  支出類別清單
// ──────────────────────────────────────────────
static std::string selectExpenseCategory() {
    const std::vector<std::string> cats = {
        "餐費", "交通", "學費", "住宿", "娛樂", "購物", "醫療", "其他支出"
    };
    printHeader("選擇支出類別");
    std::cout << "  0. 退出\n";
    for (int i = 0; i < (int)cats.size(); i++)
        std::cout << "  " << (i+1) << ". " << cats[i] << "\n";
    std::cout << "  " << cats.size()+1 << ". 自訂\n";
    int ch = 0;
    std::cout << "選擇：";
    std::cin >> ch;
    if (ch == 0) return "";  // 退出信號
    if (ch >= 1 && ch <= (int)cats.size()) return cats[ch-1];
    return inputNonEmpty("輸入自訂類別：");
}

// ──────────────────────────────────────────────
//  子選單：新增收入
// ──────────────────────────────────────────────
static void incomeMenu(Student* student) {
    while (true) {
        clearScreen();
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "\n  當前餘額：$" << student->getBalance() << "\n";
        std::string category = selectIncomeCategory();
        if (category.empty()) break;  // 選 0 退出

        double amount = inputAmount("金額：$");
        std::cout << "日期（預設今天 " << today() << "，Enter 接受）：";
        std::cin.ignore();
        std::string date;
        std::getline(std::cin, date);
        if (date.empty()) date = today();
        std::cout << "備註（可留空）：";
        std::string note;
        std::getline(std::cin, note);

        Income* inc = new Income(newTxnId(), category, amount, date, note);
        student->addTransaction(inc);
        FileManager::saveTransactions(student->getId(), student->getTransactions());
        std::cout << "\n  ✓ 已新增收入：+" << std::fixed << std::setprecision(2)
                  << amount << "（" << category << "）\n";
        std::cout << "  當前餘額：$" << student->getBalance() << "\n";
        pause();
    }
}

// ──────────────────────────────────────────────
//  子選單：新增支出
// ──────────────────────────────────────────────
static void expenseMenu(Student* student) {
    while (true) {
        clearScreen();
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "\n  當前餘額：$" << student->getBalance() << "\n";
        std::string category = selectExpenseCategory();
        if (category.empty()) break;  // 選 0 退出

        std::cout << "\n支出類型：\n";
        std::cout << "  1. 固定支出（學費、房租等）\n";
        std::cout << "  2. 臨時支出（餐費、娛樂等）\n";
        std::cout << "選擇：";
        int typeChoice = 0;
        std::cin >> typeChoice;

        double amount = inputAmount("金額：$");
        std::cout << "日期（預設今天 " << today() << "，Enter 接受）：";
        std::cin.ignore();
        std::string date;
        std::getline(std::cin, date);
        if (date.empty()) date = today();
        std::cout << "備註（可留空）：";
        std::string note;
        std::getline(std::cin, note);

        std::string yearMonth = date.size() >= 7 ? date.substr(0, 7) : "";
        Transaction* exp = (typeChoice == 1)
            ? (Transaction*) new FixedExpense(newTxnId(), category, amount, date, note)
            : (Transaction*) new OneTimeExpense(newTxnId(), category, amount, date, note);

        student->addTransaction(exp);
        FileManager::saveTransactions(student->getId(), student->getTransactions());
        std::cout << "\n  ✓ 已新增支出：-" << std::fixed << std::setprecision(2)
                  << amount << "（" << category << "）\n";
        std::cout << "  當前餘額：$" << student->getBalance() << "\n";
        if (!yearMonth.empty()) checkBudget(student, category, yearMonth);
        pause();
    }
}

// ──────────────────────────────────────────────
//  子選單：查詢記錄
// ──────────────────────────────────────────────
static void queryMenu(Student* student) {
    while (true) {
        clearScreen();
        printHeader("查詢記錄");
        std::cout << "  1. 全部記錄\n";
        std::cout << "  2. 依月份篩選\n";
        std::cout << "  0. 返回\n";
        std::cout << "選擇：";
        int ch = 0;
        std::cin >> ch;
        if (ch == 0) break;
        if (ch != 1 && ch != 2) { std::cout << "  ✗ 無效選擇\n"; pause(); continue; }

        std::string filter = "";
        if (ch == 2) filter = inputNonEmpty("輸入年月（格式 YYYY-MM）：");

        auto& txns = student->getTransactions();
        std::sort(txns.begin(), txns.end(),
            [](Transaction* a, Transaction* b){ return a->getDate() < b->getDate(); });

        std::vector<Transaction*> show;
        std::copy_if(txns.begin(), txns.end(), std::back_inserter(show),
            [&filter](Transaction* t){
                return filter.empty() || t->getYearMonth() == filter;
            });

        if (show.empty()) {
            std::cout << "\n  （無符合的記錄）\n";
        } else {
            std::cout << "\n";
            std::cout << std::left
                      << std::setw(8)  << "ID"
                      << std::setw(12) << "日期"
                      << std::setw(10) << "類型"
                      << std::setw(14) << "類別"
                      << std::right << std::setw(10) << "金額"
                      << "  備註\n";
            std::cout << std::string(70, '-') << "\n";
            for (Transaction* t : show) {
                std::cout << std::left
                          << std::setw(8)  << t->getId()
                          << std::setw(12) << t->getDate()
                          << std::setw(10) << t->getKindLabel()
                          << std::setw(14) << t->getCategory()
                          << std::right << std::setw(8)
                          << (t->getSign() > 0 ? "+" : "-")
                          << std::fixed << std::setprecision(2)
                          << std::setw(9) << t->getAmount()
                          << "  " << t->getNote() << "\n";
            }
            std::cout << std::string(70, '-') << "\n";
            std::cout << "共 " << show.size() << " 筆\n";
        }
        pause();
    }
}

// ──────────────────────────────────────────────
//  子選單：刪除記錄
// ──────────────────────────────────────────────
static void deleteMenu(Student* student) {
    while (true) {
        clearScreen();
        printHeader("刪除記錄");
        std::cout << "  1. 刪除一筆（輸入交易 ID）\n";
        std::cout << "  0. 返回\n";
        std::cout << "選擇：";
        int ch = 0;
        std::cin >> ch;
        if (ch == 0) break;
        if (ch != 1) { std::cout << "  ✗ 無效選擇\n"; pause(); continue; }

        std::string txnId = inputNonEmpty("輸入要刪除的交易 ID：");
        std::cout << "確認刪除 " << txnId << "？（y/n）：";
        char confirm;
        std::cin >> confirm;
        if (confirm == 'y' || confirm == 'Y') {
            if (student->removeTransaction(txnId)) {
                FileManager::saveTransactions(student->getId(), student->getTransactions());
                std::cout << "  ✓ 已刪除。\n";
            } else {
                std::cout << "  ✗ 找不到該記錄（ID: " << txnId << "）\n";
            }
        } else {
            std::cout << "  已取消。\n";
        }
        pause();
    }
}

// ──────────────────────────────────────────────
//  功能：預算管理
// ──────────────────────────────────────────────
static void budgetMenu(Student* student) {
    while (true) {
        clearScreen();
        printHeader("預算管理");
        std::cout << "  1. 設定 / 修改預算\n";
        std::cout << "  2. 查看預算狀況\n";
        std::cout << "  0. 返回\n";
        std::cout << "選擇：";
        int ch = 0;
        std::cin >> ch;

        if (ch == 0) break;

        if (ch == 1) {
            // 設定預算
            std::string category = selectExpenseCategory();
            double amount = inputAmount("預算金額：$");
            student->setBudget(category, amount);
            FileManager::saveBudgets(student->getId(), student->getBudgets());
            std::cout << "  ✓ 已設定 " << category << " 的月預算為 $"
                      << std::fixed << std::setprecision(2) << amount << "\n";
        } else if (ch == 2) {
            // 查看預算狀況
            const auto& budgets = student->getBudgets();
            if (budgets.empty()) {
                std::cout << "\n  （尚未設定任何預算）\n";
            } else {
                // 計算當月各類別支出
                std::string ym = today().substr(0, 7);
                std::map<std::string, double> catSpend;
                for (const Transaction* t : student->getTransactions()) {
                    if (t->getSign() < 0 && t->getYearMonth() == ym)
                        catSpend[t->getCategory()] += t->getAmount();
                }

                std::cout << "\n";
                std::cout << std::left
                          << std::setw(14) << "類別"
                          << std::right << std::setw(10) << "預算"
                          << std::setw(10) << "已使用"
                          << std::setw(10) << "剩餘"
                          << "  狀態\n";
                std::cout << std::string(56, '-') << "\n";
                for (const auto& [cat, budget] : budgets) {
                    double used   = catSpend.count(cat) ? catSpend[cat] : 0.0;
                    double remain = budget - used;
                    std::cout << std::left  << std::setw(14) << cat
                              << std::right << std::fixed << std::setprecision(2)
                              << std::setw(10) << budget
                              << std::setw(10) << used
                              << std::setw(10) << remain
                              << "  " << (remain < 0 ? "⚠ 超支" : "正常") << "\n";
                }
            }
        }
        pause();
    }
}

// ──────────────────────────────────────────────
//  功能：匯出報表
// ──────────────────────────────────────────────
static void exportReport(Student* student) {
    clearScreen();
    printHeader("匯出月份報表");
    std::string ym = inputNonEmpty("輸入年月（格式 YYYY-MM）：");
    if (ReportGenerator::generate(student, ym)) {
        // 組合路徑顯示給使用者
        std::string ymStripped = ym;
        ymStripped.erase(std::remove(ymStripped.begin(), ymStripped.end(), '-'),
                         ymStripped.end());
        std::cout << "\n  ✓ 報表已儲存至：reports/report_"
                  << student->getId() << "_" << ymStripped << ".txt\n";
    } else {
        std::cout << "\n  ✗ 該月份（" << ym << "）無交易記錄，未產生報表。\n";
    }
}

// ──────────────────────────────────────────────
//  已登入主選單
// ──────────────────────────────────────────────
static void loggedInMenu(UserManager& um) {
    while (true) {
        Student* s = um.getCurrentUser();
        clearScreen();
        printHeader("個人財務管理");
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "  歡迎，" << s->getName()
                  << "（" << s->getId() << "）"
                  << "  |  餘額：$" << s->getBalance() << "\n\n";
        std::cout << "  1. 新增收入\n";
        std::cout << "  2. 新增支出\n";
        std::cout << "  3. 查詢記錄\n";
        std::cout << "  4. 刪除記錄\n";
        std::cout << "  5. 預算管理\n";
        std::cout << "  6. 匯出報表\n";
        std::cout << "  7. 帳戶資訊\n";
        std::cout << "  0. 登出\n";
        std::cout << "選擇：";

        int ch = 0;
        std::cin >> ch;

        switch (ch) {
            case 1: incomeMenu(s);        break;
            case 2: expenseMenu(s);       break;
            case 3: queryMenu(s);         break;
            case 4: deleteMenu(s);        break;
            case 5: budgetMenu(s);        break;
            case 6: exportReport(s);      pause(); break;
            case 7: clearScreen(); s->display(); pause(); break;
            case 0: um.logout(); return;
            default: std::cout << "  ✗ 無效選擇\n"; pause();
        }
    }
}

// ──────────────────────────────────────────────
//  主選單（未登入）
// ──────────────────────────────────────────────
static void mainMenu(UserManager& um) {
    while (true) {
        clearScreen();
        printHeader("學生個人財務管理系統 v1.0");
        std::cout << "  1. 登入\n";
        std::cout << "  2. 註冊新帳戶\n";
        std::cout << "  0. 結束\n";
        std::cout << "選擇：";

        int ch = 0;
        std::cin >> ch;

        if (ch == 0) {
            std::cout << "\n  感謝使用，再見！\n";
            break;
        } else if (ch == 1) {
            std::string sid = inputNonEmpty("輸入學號：");
            Student* s = um.login(sid);
            if (!s) {
                std::cout << "  ✗ 帳戶不存在，請先註冊。\n";
                pause();
            } else {
                loggedInMenu(um);
            }
        } else if (ch == 2) {
            std::string sid  = inputNonEmpty("輸入學號：");
            std::string name = inputNonEmpty("輸入姓名：");
            if (um.registerStudent(sid, name)) {
                std::cout << "  ✓ 註冊成功！請重新登入。\n";
            } else {
                std::cout << "  ✗ 此學號已被使用，請換一個。\n";
            }
            pause();
        } else {
            std::cout << "  ✗ 無效選擇\n";
            pause();
        }
    }
}

// ──────────────────────────────────────────────
//  程式入口
// ──────────────────────────────────────────────
int main() {
    // ── 設定 UTF-8 輸出與 ANSI 支援（修正 Windows 終端機中文亂碼與色彩）──
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
        SetConsoleMode(hOut, dwMode);
    }
#endif

    // 確保 data / reports 目錄存在
#ifdef _WIN32
    system("if not exist data mkdir data");
    system("if not exist reports mkdir reports");
#else
    system("mkdir -p data reports");
#endif

    UserManager um;
    mainMenu(um);
    return 0;
}
