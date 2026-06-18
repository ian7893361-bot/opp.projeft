# 開發迭代日誌

## 專案資訊
- **專題名稱**：學生個人財務管理系統
- **課程**：物件導向程式設計
- **開發時程**：2 週
- **開發語言**：C++17

---

## 迭代 1 — 系統設計與規格（第 1-2 天）

### 目標
建立完整的設計文件，確定技術架構。

### 完成項目
- ✅ 分析需求，確定多使用者 + 收支管理 + 預算警示 + 報表匯出四大核心功能
- ✅ 設計 Person 繼承鏈：`Person → Student/Admin`
- ✅ 設計 Transaction 繼承鏈：`Transaction → Income/Expense → FixedExpense/OneTimeExpense`（3 層繼承）
- ✅ 確定 STL 容器選擇：`map<string, Student*>` 多使用者索引、`vector<Transaction*>` 交易清單
- ✅ 確定資料格式：key=value 文字格式，以 `---` 分隔區塊
- ✅ 建立 OpenSpec 規格文件（proposal.md、design.md、5 個 spec 檔案）

### 技術決策
- 使用原始指標（非 `shared_ptr`）以符合課程 C++ 基礎語法要求
- 資料格式選用自訂文字格式（非 CSV/JSON），`fstream + getline` 即可解析
- 選擇 `printf/cout` 風格 UI（非 ncurses），降低複雜度

---

## 迭代 2 — 基礎架構與類別體系（第 3-4 天）

### 目標
建立所有類別的 header 與 .cpp 實作。

### 完成項目
- ✅ 建立目錄結構：`src/`、`include/`、`data/`、`reports/`
- ✅ 建立 `Makefile`（C++17 編譯設定）
- ✅ 實作 `Person.h`（抽象基底，含純虛函數 `display()`、`getType()`）
- ✅ 實作 `Student.h / .cpp`（繼承 Person，管理交易 vector 與預算 map）
- ✅ 實作 `Admin.h / .cpp`（繼承 Person）
- ✅ 實作 `Transaction.h`（抽象基底，含純虛函數 `getSign()`、`getTypeString()`）
- ✅ 實作 `Income.h`（繼承 Transaction，getSign() = +1）
- ✅ 實作 `Expense.h`（抽象，繼承 Transaction，getSign() = -1）
- ✅ 實作 `FixedExpense.h`（繼承 Expense）
- ✅ 實作 `OneTimeExpense.h`（繼承 Expense）

### 學到的事
- C++ 純虛函數（`= 0`）強制子類實作，是展示多態的關鍵語法
- 使用 `override` 關鍵字可讓編譯器驗證確實覆寫父類函數

---

## 迭代 3 — 資料持久化（第 5-6 天）

### 目標
實作 fstream 讀寫功能，確保資料跨次執行保存。

### 完成項目
- ✅ 設計並實作 `FileManager` 類別（靜態方法）
- ✅ `loadUsers()` / `saveUsers()`：讀寫 `data/users.txt`
- ✅ `loadTransactions()` / `saveTransactions()`：讀寫 `data/txn_<id>.txt`
  - 依 TYPE 欄位多態建立 `Income`、`FixedExpense`、`OneTimeExpense` 物件
- ✅ `loadBudgets()` / `saveBudgets()`：讀寫 `data/budget_<id>.txt`

### 技術挑戰
- **解析 key=value 區塊**：以 `---` 分隔，用 `istringstream + find('=')` 解析每個欄位
- **多態還原**：讀檔時依 `TYPE` 欄位決定建立哪個子類別物件（工廠模式）
- **輸入驗證**：`stod()` 以 `try-catch` 包裹，防止非數字欄位導致崩潰

---

## 迭代 4 — 使用者管理與 UI（第 7-9 天）

### 目標
實作登入 / 登出 / 註冊 + 主選單 UI。

### 完成項目
- ✅ 實作 `UserManager`：以 `map<string, Student*>` 管理多使用者
  - `registerStudent()`：驗證學號唯一性，建立新 Student
  - `login()`：查找使用者，載入資料（呼叫 FileManager）
  - `logout()`：儲存資料，清除工作階段
- ✅ `main.cpp` 主選單（未登入）：登入 / 註冊 / 結束
- ✅ 個人選單（已登入）：7 個功能選項
- ✅ 新增收入 UI、新增支出 UI（固定/臨時分類）
- ✅ 查詢記錄（全部 + 依月份篩選，使用 `sort` + `find_if`）
- ✅ 刪除記錄（以 `find_if` 找到並刪除，`delete` 釋放記憶體）

### STL 使用亮點
```cpp
// sort：依日期排序
std::sort(txns.begin(), txns.end(),
    [](Transaction* a, Transaction* b){ return a->getDate() < b->getDate(); });

// find_if：篩選月份
std::copy_if(txns.begin(), txns.end(), std::back_inserter(show),
    [&filter](Transaction* t){ return t->getYearMonth() == filter; });
```

---

## 迭代 5 — 預算警示與報表匯出（第 10-11 天）

### 目標
完成進階功能：預算管理 + 月份報表。

### 完成項目
- ✅ 預算設定 UI：輸入類別 + 金額，儲存至 `map<string, double>`
- ✅ 預算總覽：計算各類別當月累計支出，顯示剩餘 / 超支狀態
- ✅ `checkBudget()`：新增支出後自動呼叫，超支顯示 `⚠` 警示
- ✅ `ReportGenerator::generate()`：
  - 以 `copy_if` 篩選月份交易
  - 以 `sort` 依日期排序
  - 計算總收入 / 總支出 / 分類統計
  - 輸出 6 個區塊的格式化文字報表

### 報表格式設計
```
========================================================
  財務管理系統 — 月份報表
【摘要】【收入明細】【支出明細】【分類統計】【預算對比】
========================================================
```

---

## 迭代 6 — 整合測試與文件（第 12-14 天）

### 目標
完整測試 + 撰寫說明文件。

### 測試項目
- ✅ 完整流程：註冊 → 登入 → 新增收支 → 設定預算 → 觸發警示 → 匯出報表 → 登出 → 重啟 → 驗證持久化
- ✅ 邊界條件：無效金額輸入（`try-catch` 攔截）、刪除不存在交易、匯出空月份
- ✅ 多使用者隔離：兩個帳戶的資料不互相影響

### 文件完成
- ✅ `README.md`：專案介紹、編譯方式、UML 圖、操作流程
- ✅ `DEVLOG.md`（本檔案）：開發迭代歷程

---

## 技術需求對照表（最終驗證）

| 需求 | 實作 | 狀態 |
|------|------|------|
| C++ 類別繼承 | Person→Student/Admin（2層）、Transaction→Income/Expense→Fixed/OneTime（3層） | ✅ |
| 讀寫檔 | FileManager fstream 讀寫 3 種資料檔案 | ✅ |
| STL 類別庫 | `map`、`vector`、`sort`、`accumulate`、`find_if`、`copy_if`、`back_inserter` | ✅ |
| 終端機 UI | 多層 cout/cin 選單，格式化 `setw/setfill` 輸出 | ✅ |
| 規格說明 | OpenSpec proposal + design + 5 specs | ✅ |
| 說明文件 | README.md + DEVLOG.md | ✅ |
