學生個人財務管理系統
Student Personal Finance Manager — C++ OOP 期末專題

專案簡介
本系統為 C++ 物件導向程式設計期末專題，以終端機介面實作多使用者個人財務管理工具。
系統同時兼具教學示範價值（C++ 繼承、STL、fstream IO）與實際可用性。

核心技術展示
技術需求	實作方式
C++ 類別繼承	Person → Student/Admin、Transaction → Income/Expense → FixedExpense/OneTimeExpense（3 層繼承鏈）
讀寫檔	FileManager 以 fstream 讀寫使用者、交易、預算資料
STL 類別庫	map<string, Student*> 使用者索引、vector<Transaction*> 交易清單、sort、accumulate、find_if、copy_if
終端機 UI	cout 格式化選單、cin 互動輸入、多層選單結構
系統功能
🔐 多使用者帳戶：註冊 / 登入 / 登出，各使用者資料完全隔離
💰 收支記錄：新增收入（打工、獎學金等）、固定支出（學費）、臨時支出（餐費）
📋 查詢 & 篩選：依月份篩選記錄，依日期排序
🗑️ 刪除記錄：依交易 ID 刪除，自動更新餘額
⚠️ 預算警示：設定各類別月預算，超支即時提示
📊 報表匯出：匯出月份文字報表至 reports/ 資料夾
💾 資料持久化：所有資料跨次執行保存
目錄結構
OPP.Project/
├── main.cpp               # 程式入口、所有 UI 邏輯
├── Makefile               # 編譯設定（C++17）
├── include/               # 標頭檔（.h）
│   ├── Person.h           # 抽象基底類別
│   ├── Student.h          # 繼承 Person
│   ├── Admin.h            # 繼承 Person
│   ├── Transaction.h      # 抽象基底類別
│   ├── Income.h           # 繼承 Transaction
│   ├── Expense.h          # 抽象，繼承 Transaction
│   ├── FixedExpense.h     # 繼承 Expense
│   ├── OneTimeExpense.h   # 繼承 Expense
│   ├── FileManager.h      # 讀寫檔管理
│   ├── UserManager.h      # 使用者帳戶管理
│   └── ReportGenerator.h  # 報表產生器
├── src/                   # 實作檔（.cpp）
│   ├── Student.cpp
│   ├── Admin.cpp
│   ├── FileManager.cpp
│   ├── UserManager.cpp
│   └── ReportGenerator.cpp
├── data/                  # 資料儲存目錄（執行後自動建立）
│   ├── users.txt          # 使用者清單
│   ├── txn_<學號>.txt     # 各使用者交易紀錄
│   └── budget_<學號>.txt  # 各使用者預算設定
└── reports/               # 匯出報表目錄（執行後自動建立）
    └── report_<學號>_<YYYYMM>.txt
類別繼承圖（UML）
┌────────────────────────────────────────────────────────────┐
│                    Person（抽象）                           │
│  + display() : void = 0                                    │
│  + getType() : string = 0                                  │
│  + getId()   : string                                      │
│  + getName() : string                                      │
└───────────────────┬────────────────────┘
                    │ 繼承
        ┌───────────┴───────────┐
        ▼                       ▼
┌──────────────┐       ┌──────────────────┐
│   Student    │       │      Admin        │
│ - balance    │       │ - canManageUsers  │
│ - trans[]    │       │ + display()       │
│ - budgets{}  │       │ + getType()→Admin │
│ + display()  │       └──────────────────┘
│ + getType()  │
│ + addTxn()   │
│ + removeTxn()│
│ + setBudget()│
└──────────────┘

┌────────────────────────────────────────────────────────────┐
│                  Transaction（抽象）                        │
│  + getSign()       : int    = 0                            │
│  + getTypeString() : string = 0                            │
│  + getKindLabel()  : string = 0                            │
│  + getId(), getCategory(), getAmount(), getDate()          │
└──────────────────────┬─────────────────────┘
                       │ 繼承
            ┌──────────┴──────────┐
            ▼                     ▼
┌────────────────┐      ┌─────────────────────┐
│    Income      │      │   Expense（抽象）    │
│ + getSign()+1  │      │ + getSign() → -1     │
│ + getTypeString│      └──────────┬───────────┘
│  → "INCOME"    │                 │ 繼承
└────────────────┘      ┌──────────┴──────────┐
                        ▼                      ▼
               ┌──────────────┐    ┌─────────────────┐
               │ FixedExpense │    │ OneTimeExpense   │
               │ → "EXPENSE   │    │ → "EXPENSE_      │
               │   _FIXED"    │    │    ONETIME"      │
               └──────────────┘    └─────────────────┘
編譯方式
需求：安裝 MinGW-w64（g++ 支援 C++17）

# 方式一：使用 Makefile
make

# 方式二：手動編譯
g++ -std=c++17 -Wall -I include main.cpp src/*.cpp -o finance_manager
執行方式
./finance_manager        # Linux / macOS
finance_manager.exe      # Windows
操作流程
主選單
├─ 1. 登入
│   └─ 個人選單
│       ├─ 1. 新增收入（選類別 → 輸金額日期備註）
│       ├─ 2. 新增支出（選固定/臨時 → 輸入資料 → 預算檢查）
│       ├─ 3. 查詢記錄（全部 or 依月份）
│       ├─ 4. 刪除記錄（輸入交易 ID）
│       ├─ 5. 預算管理
│       │   ├─ 設定預算（類別 + 金額）
│       │   └─ 查看預算狀況（表格顯示）
│       ├─ 6. 匯出報表（輸入 YYYY-MM）
│       ├─ 7. 帳戶資訊
│       └─ 0. 登出
├─ 2. 註冊新帳戶（學號 + 姓名）
└─ 0. 結束
資料格式範例
data/txn_S001.txt

ID=T00001
TYPE=INCOME
CATEGORY=打工
AMOUNT=3000.00
DATE=2026-06-01
NOTE=六月份工讀
---
ID=T00002
TYPE=EXPENSE_ONETIME
CATEGORY=餐費
AMOUNT=250.00
DATE=2026-06-05
NOTE=午餐
---
reports/report_S001_202606.txt

========================================================
  財務管理系統 — 月份報表
  學生：王小明（S001）
  月份：2026-06
========================================================

【摘要】
  總收入：  $  3000.00
  總支出：  $   250.00
  結  餘：  $  2750.00
...
開發環境
語言：C++17
編譯器：g++ (MinGW-w64)
平台：Windows / Linux / macOS
依賴：標準 STL（無第三方函式庫）
作者
期末專題 — 物件導向程式設計
開發時程：2 週（2026/06）
