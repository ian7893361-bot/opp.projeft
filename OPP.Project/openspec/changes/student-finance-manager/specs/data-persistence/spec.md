## ADDED Requirements

### Requirement: 系統啟動時自動從檔案讀取所有使用者資料
系統 SHALL 在啟動時讀取 `data/users.txt`，載入所有使用者帳戶至 `map<string, Student*>`。

#### Scenario: 首次啟動（無資料檔）
- **WHEN** `data/users.txt` 不存在
- **THEN** 系統建立空的資料目錄與使用者清單，顯示主選單

#### Scenario: 正常啟動（有既有資料）
- **WHEN** `data/users.txt` 存在並包含有效使用者資料
- **THEN** 系統成功載入所有使用者帳戶，顯示主選單

### Requirement: 使用者登入後系統載入其個人交易與預算資料
系統 SHALL 在使用者登入時，讀取 `data/txn_<id>.txt` 與 `data/budget_<id>.txt`，將交易還原為對應的 `Transaction` 子類別物件。

#### Scenario: 成功讀取交易資料
- **WHEN** 使用者成功登入，且其交易檔案存在
- **THEN** 系統解析檔案中的每筆交易，依 TYPE 欄位建立 `Income`、`FixedExpense` 或 `OneTimeExpense` 物件並加入 `vector`

#### Scenario: 首次登入（無交易檔）
- **WHEN** 使用者首次登入，`data/txn_<id>.txt` 不存在
- **THEN** 系統以空的交易清單初始化，正常進入個人選單

### Requirement: 系統在資料變更後自動寫檔儲存
系統 SHALL 在每次新增或刪除交易、修改預算後，立即將對應使用者的資料寫回對應檔案。

#### Scenario: 新增交易後自動儲存
- **WHEN** 使用者成功新增收入或支出
- **THEN** 系統將完整的交易清單（含新增項目）寫回 `data/txn_<id>.txt`

#### Scenario: 登出時儲存使用者清單
- **WHEN** 使用者登出
- **THEN** 系統確保 `data/users.txt` 已反映最新的使用者資料

### Requirement: 資料格式採用 key=value 文字區塊格式
系統 SHALL 使用以下格式讀寫交易記錄，以 `---` 作為記錄分隔符號：

```
ID=<uuid>
TYPE=<INCOME|EXPENSE_FIXED|EXPENSE_ONETIME>
CATEGORY=<category>
AMOUNT=<decimal>
DATE=<YYYY-MM-DD>
NOTE=<text>
---
```

#### Scenario: 格式解析正確性
- **WHEN** 系統讀取一個格式正確的 `txn_<id>.txt`
- **THEN** 所有交易記錄被正確解析，無資料遺失，物件類型與 TYPE 欄位一致
