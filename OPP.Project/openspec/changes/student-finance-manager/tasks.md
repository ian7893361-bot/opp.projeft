## 1. 專案基礎架構

- [x] 1.1 建立專案目錄結構：`src/`、`include/`、`data/`、`reports/`
- [x] 1.2 建立 `Makefile`（或 CMakeLists.txt），設定 C++17 編譯選項
- [x] 1.3 建立 `main.cpp` 入口點，實作主選單框架（未登入狀態：登入 / 註冊 / 結束）

## 2. 類別體系 — Person 繼承鏈

- [x] 2.1 定義 `Person` 抽象基底類別（`include/Person.h`）：含純虛函數 `display()`、`getType()`、`getId()`
- [x] 2.2 實作 `Student` 類別（`include/Student.h`、`src/Student.cpp`）：繼承 `Person`，新增 `balance`、`transactions`、`budgets`
- [x] 2.3 實作 `Admin` 類別（`include/Admin.h`、`src/Admin.cpp`）：繼承 `Person`，實作 `getType()` 回傳 `"Admin"`

## 3. 類別體系 — Transaction 繼承鏈

- [x] 3.1 定義 `Transaction` 抽象基底類別（`include/Transaction.h`）：含純虛函數 `getSign()`、`getTypeString()`，及共用欄位 `id`、`category`、`amount`、`date`、`note`
- [x] 3.2 實作 `Income` 類別（`include/Income.h`、`src/Income.cpp`）：繼承 `Transaction`，`getSign()` 回傳 `+1`
- [x] 3.3 實作 `Expense` 抽象類別（`include/Expense.h`）：繼承 `Transaction`，`getSign()` 回傳 `-1`
- [x] 3.4 實作 `FixedExpense` 類別（`include/FixedExpense.h`、`src/FixedExpense.cpp`）：繼承 `Expense`，`getTypeString()` 回傳 `"EXPENSE_FIXED"`
- [x] 3.5 實作 `OneTimeExpense` 類別（`include/OneTimeExpense.h`、`src/OneTimeExpense.cpp`）：繼承 `Expense`，`getTypeString()` 回傳 `"EXPENSE_ONETIME"`

## 4. 資料持久化（讀寫檔）

- [x] 4.1 實作 `FileManager` 類別（`include/FileManager.h`、`src/FileManager.cpp`）
- [x] 4.2 實作 `FileManager::loadUsers()`：讀取 `data/users.txt`，回傳 `map<string, Student*>`
- [x] 4.3 實作 `FileManager::saveUsers()`：將使用者清單寫入 `data/users.txt`
- [x] 4.4 實作 `FileManager::loadTransactions(studentId)`：讀取 `data/txn_<id>.txt`，依 TYPE 欄位建立對應子類別物件
- [x] 4.5 實作 `FileManager::saveTransactions(studentId, transactions)`：將交易清單寫回 `data/txn_<id>.txt`
- [x] 4.6 實作 `FileManager::loadBudgets(studentId)`：讀取 `data/budget_<id>.txt`，回傳 `map<string, double>`
- [x] 4.7 實作 `FileManager::saveBudgets(studentId, budgets)`：將預算設定寫回 `data/budget_<id>.txt`

## 5. 使用者帳戶功能

- [x] 5.1 實作 `UserManager` 類別（`include/UserManager.h`、`src/UserManager.cpp`），使用 `map<string, Student*>` 管理所有使用者
- [x] 5.2 實作 `UserManager::registerStudent()`：驗證學號唯一性，建立新 `Student`，呼叫 `FileManager::saveUsers()`
- [x] 5.3 實作 `UserManager::login(studentId)`：查找使用者，載入交易與預算資料，回傳指標
- [x] 5.4 實作登出邏輯：儲存資料，清除當前使用者狀態

## 6. 交易管理功能

- [x] 6.1 實作新增收入 UI 流程：提示輸入類別、金額、日期、備註，建立 `Income` 物件
- [x] 6.2 實作新增支出 UI 流程：提示選擇固定/臨時類型，建立對應物件，新增後觸發預算檢查
- [x] 6.3 實作查詢全部記錄：以 `sort` 依日期排序 `vector<Transaction*>`，格式化輸出
- [x] 6.4 實作依月份篩選：以 `find_if` 篩選符合年月的交易記錄
- [x] 6.5 實作刪除交易：依 ID 找到交易，從 `vector` 移除並 `delete`，呼叫儲存

## 7. 預算管理與警示功能

- [x] 7.1 實作設定預算 UI：提示輸入類別與金額，更新 `map<string, double>` 並儲存
- [x] 7.2 實作查看預算狀況 UI：以 `accumulate` 計算各類別當月支出，對比預算，格式化輸出表格
- [x] 7.3 實作超支警示函式 `checkBudget(category, month)`：新增支出後呼叫，若超支顯示 `⚠` 警示訊息

## 8. 報表匯出功能

- [x] 8.1 實作 `ReportGenerator` 類別（`include/ReportGenerator.h`、`src/ReportGenerator.cpp`）
- [x] 8.2 實作 `ReportGenerator::generate(student, yearMonth)`：篩選該月交易，計算統計
- [x] 8.3 實作報表寫檔：格式化輸出六個區塊（標題、摘要、收入明細、支出明細、分類統計、預算對比）至 `reports/report_<id>_<YYYYMM>.txt`
- [x] 8.4 實作匯出 UI：提示輸入年月，呼叫 `ReportGenerator`，顯示成功訊息或無資料提示

## 9. 整合測試與除錯

- [x] 9.1 測試完整使用流程：註冊 → 登入 → 新增收支 → 設定預算 → 觸發警示 → 匯出報表 → 登出 → 重啟 → 驗證資料持久化
- [x] 9.2 測試邊界條件：無效金額輸入、刪除不存在的交易、匯出無資料月份
- [x] 9.3 測試多使用者資料隔離：兩個使用者的交易資料不互相影響

## 10. 文件撰寫

- [x] 10.1 撰寫 `README.md`：專案介紹、編譯方式、執行方式、功能清單
- [x] 10.2 繪製 UML 類別圖（可用 ASCII 或工具）並加入文件
- [x] 10.3 撰寫使用說明（操作流程截圖或文字說明）
- [x] 10.4 撰寫開發迭代日誌（兩週的開發歷程記錄）
