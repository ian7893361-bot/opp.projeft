## Context

本專題為 C++ 物件導向程式設計期末作業，在終端機環境下開發一個多使用者的個人財務管理系統。系統使用標準 C++17 與 STL，不依賴第三方函式庫，資料以自訂文字格式持久化儲存。

開發時程為兩週，目標是同時滿足「教學示範（OOP、STL、IO）」與「實際可用」兩個目標。

## Goals / Non-Goals

**Goals:**
- 以多層繼承體系（Person → Student/Admin、Transaction → Income/Expense → FixedExpense/OneTimeExpense）展示 C++ OOP 設計
- 以 `map<string, Student*>` 實現多使用者資料隔離與快速查找
- 以 `vector<Transaction*>` 搭配 `sort`、`accumulate`、`find_if` 操作交易資料
- 支援 `fstream` 讀寫自訂文字格式，確保資料持久化
- 提供清楚的 `printf/cout` 風格終端機選單 UI
- 支援分類預算設定與超支警示
- 支援按月匯出可讀文字報表

**Non-Goals:**
- 不實作圖形介面（GUI）或 ncurses 互動式游標
- 不實作網路連線或資料庫
- 不實作 CSV 匯入（初版；後期可擴充）
- 不實作加密或密碼安全性機制（超出課程範圍）

## Decisions

### 決策 1：繼承體系設計
**選擇**：`Person`（純虛基底）→ `Student` / `Admin`；`Transaction`（純虛基底）→ `Income` / `Expense` → `FixedExpense` / `OneTimeExpense`

**理由**：
- `Person` 定義共用介面（`display()`、`getType()`），以純虛函數強制子類實作，展示多態
- `Transaction` 同樣為抽象類別，`getCategory()` 與 `getSign()` 由子類各自實作
- 層次夠深（三層）足以展示繼承鏈，又不過度設計

**替代方案**：全部用單一 `User` 類別 + 欄位區分類型 → 太簡單，無法展示多態

---

### 決策 2：資料儲存格式
**選擇**：自訂 key=value 文字格式（每筆交易一個 block，以 `---` 分隔）

```
ID=TXN001
TYPE=EXPENSE
CATEGORY=Food
AMOUNT=250.00
DATE=2026-06-10
NOTE=午餐
---
```

**理由**：
- 比 CSV 更好實作（不需處理逗號跳脫）
- 人類可直接閱讀與編輯
- `fstream` + `getline` + `stringstream` 即可完成解析
- 後期擴充為 CSV 只需修改 `save()` / `load()` 方法

**替代方案**：JSON → 需額外 parser；CSV → 欄位逗號需跳脫處理

---

### 決策 3：STL 容器選擇
| 場景 | 容器 | 理由 |
|------|------|------|
| 使用者索引 | `map<string, Student*>` | O(log n) 以學號查找 |
| 交易清單 | `vector<Transaction*>` | 動態大小、支援 sort |
| 預算設定 | `map<string, double>` | 類別 → 金額對映 |
| 類別統計 | `map<string, double>` | 累加各類別支出 |

---

### 決策 4：多態物件管理
使用原始指標 `vector<Transaction*>` 搭配解構子清理，而非 `shared_ptr`，以符合課程 C++ 基礎語法要求。

## Risks / Trade-offs

| 風險 | 緩解方式 |
|------|---------|
| 記憶體洩漏（`new` 後忘記 `delete`）| 析構子統一清理容器內所有指標 |
| 多使用者同時存取資料（檔案競爭）| 單執行緒設計，每次登入獨占操作，無需 lock |
| 輸入驗證不足（非數字輸入金額）| 以 `try-catch` 包裹 `stod()`，無效輸入重新提示 |
| 兩週時程緊張 | 以功能優先順序迭代：核心 CRUD → 預算警示 → 報表匯出 |
