## 1. 專案目錄結構調整

- [x] 1.1 建立 `include/` 底下的子資料夾：`common/`、`model/`、`persistence/`、`auth/`、`transaction_ui/`、`budget/`、`report/`
- [x] 1.2 建立 `src/` 底下的子資料夾：`common/`、`model/`、`persistence/`、`auth/`、`transaction_ui/`、`budget/`、`report/`
- [x] 1.3 將原有類別的標頭檔與實作檔搬移至對應子資料夾：
  - `Person.h` -> `include/common/` (或 `include/model/`)
  - `Student.h`, `Admin.h` -> `include/model/`
  - `Transaction.h`, `Income.h`, `Expense.h`, `FixedExpense.h`, `OneTimeExpense.h` -> `include/model/`
  - `FileManager.h` -> `include/persistence/`
  - `UserManager.h` -> `include/auth/`
  - `ReportGenerator.h` -> `include/report/`
  - 搬移對應的 `.cpp` 到 `src/` 對應子資料夾下
- [x] 1.4 更新這些現有類別中 `#include` 的相對路徑（使其相容 `-I include` 和新目錄分層）

## 2. 實作 ConsoleUtils 模組 (共通工具與輸入)

- [x] 2.1 建立 `include/common/ConsoleUtils.h` 與 `src/common/ConsoleUtils.cpp`
- [x] 2.2 從 `main.cpp` 中抽取 `clearScreen()`, `pause()`, `today()`, `getUtf8Width()`, `printHeader()` 等工具函式放入 `ConsoleUtils`
- [x] 2.3 從 `main.cpp` 中抽取 `inputAmount()`, `inputNonEmpty()` 等防呆輸入函式放入 `ConsoleUtils`
- [x] 2.4 將 ID 產生器 `newTxnId()` 放入 `ConsoleUtils` (可在此處維護 `txnCounter`)

## 3. 實作各 UI 模組拆分 (方案 A)

- [x] 3.1 實作 `AuthUI` (`include/auth/AuthUI.h`, `src/auth/AuthUI.cpp`)：
  - 包含 `mainMenu()`, `loggedInMenu()`
- [x] 3.2 實作 `TransactionUI` (`include/transaction_ui/TransactionUI.h`, `src/transaction_ui/TransactionUI.cpp`)：
  - 包含 `selectIncomeCategory()`, `selectExpenseCategory()`, `incomeMenu()`, `expenseMenu()`, `queryMenu()`, `deleteMenu()`
- [x] 3.3 實作 `BudgetUI` (`include/budget/BudgetUI.h`, `src/budget/BudgetUI.cpp`)：
  - 包含 `budgetMenu()`, `checkBudget()`
- [x] 3.4 實作 `ReportUI` (`include/report/ReportUI.h`, `src/report/ReportUI.cpp`)：
  - 包含 `exportReport()`

## 4. 實作資料儲存路徑變更與目錄自動建立 (方案 B)

- [x] 4.1 修改 `FileManager`，更新 `txnFile()` 與 `budgetFile()` 的路徑為 `data/<student_id>/txn.txt` 與 `data/<student_id>/budget.txt`
- [x] 4.2 在 `FileManager::saveTransactions()`、`FileManager::saveBudgets()` 寫入前，使用 `std::filesystem::create_directories` 確保學生目錄存在
- [x] 4.3 修改 `ReportGenerator::reportPath`，更新路徑為 `reports/<YYYY-MM>/report_<student_id>.txt`
- [x] 4.4 在 `ReportGenerator::generate()` 寫入前，使用 `std::filesystem::create_directories` 確保月份目錄存在
- [x] 4.5 精簡 `main.cpp`，將原有的 Windows/Linux 平台 initialization (console CP 設定) 與 `AuthUI::showMainMenu(um)` 整合，移除所有 UI 選單程式

## 5. 專案編譯與整合測試

- [x] 5.1 更新 `Makefile`，更新 `SRCS` 清單以 wildcard 包含所有子目錄原始碼
- [x] 5.2 執行編譯，解決可能出現的 `#include` 路徑遺漏或編譯錯誤
- [x] 5.3 執行測試：
  - 驗證登入、註冊功能
  - 驗證記帳與預算管理
  - 驗證報表匯出功能
  - 驗證實體路徑：檢查 `data/` 下是否確實建立 `<student_id>/` 子資料夾，以及 `reports/` 下是否確實建立 `<YYYY-MM>/` 子資料夾。
