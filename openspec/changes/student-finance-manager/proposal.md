## Why

學生在日常生活中缺乏一個結構化的方式來追蹤個人收支，導致月底常有「錢去哪了」的困惑。本系統以 C++ 物件導向設計為核心，提供多使用者的終端機財務管理工具，兼具期末專題的教學示範價值與實際可用性。

## What Changes

- 新增完整的多使用者帳戶系統（註冊、登入、登出）
- 新增收支記錄功能（新增、查詢、刪除交易）
- 新增分類預算設定與超支警示功能
- 新增月份報表匯出為可讀文字檔案
- 使用自訂文字格式進行資料持久化（讀檔 / 寫檔）
- 以 C++ 類別繼承（Person → Student/Admin、Transaction → Income/Expense）展示 OOP 設計
- 以 STL 容器（`vector`、`map`）與演算法（`sort`、`accumulate`、`find_if`）處理資料

## Capabilities

### New Capabilities

- `user-account`: 使用者帳戶管理，包含註冊、登入、登出，以及以學號為索引的多使用者資料隔離
- `transaction-management`: 收支記錄的新增、查詢與刪除，支援收入（Income）與支出（Expense）兩大類及細分類別
- `budget-alert`: 各類別預算上限設定，及消費超出預算時的即時警示提示
- `report-export`: 依月份匯出完整收支報表至文字檔，包含收支明細、分類統計與預算達成率
- `data-persistence`: 以自訂文字格式讀寫使用者資料、交易紀錄與預算設定，確保資料跨次執行持久保存

### Modified Capabilities

（無現有規格，本次為全新開發）

## Impact

- **新建專案**：`c:/Users/a0968/Desktop/OPP.Project/` 下的全部 C++ 原始碼
- **類別體系**：`Person`（抽象）、`Student`、`Admin`、`Transaction`（抽象）、`Income`、`Expense`、`FixedExpense`、`OneTimeExpense`
- **資料目錄**：`data/` 資料夾，儲存 `users.txt`、`txn_<id>.txt`、`budget_<id>.txt`
- **報表目錄**：`reports/` 資料夾，儲存匯出的月份報表文字檔
- **依賴環境**：標準 C++17、STL，無第三方函式庫；終端機 UI 使用標準 `iostream`
