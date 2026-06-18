## Why

在專案完成首階段重構後，經分析發現系統在極端輸入或多次操作下存在以下 3 個核心穩定度問題：
1. **重複登入資料倍增與記憶體洩漏**：同一工作階段（Session）下若登出後再次登入相同帳戶，系統會重複載入相同的交易記錄，導致交易列表重複且餘額累計翻倍，並伴隨記憶體洩漏。
2. **交易 ID 衝突**：每次程式重啟後交易計數器 `txnCounter` 都重設為 1，當使用者新增交易時會產生已存在之 ID（例如 `T00001`），進而導致檔案寫入衝突與資料覆蓋。
3. **輸入防呆不足**：在各大選單輸入非數字（例如字母 `a`）時，`std::cin` 讀取整數會失敗並進入錯誤狀態，導致畫面陷入無限死循環。

本變更提案旨在實作上述 3 個核心防護機制，使整個系統達到生產環境級別的健全度與容錯率。

## What Changes

- **修正登入資料重複載入 (Bug 1)**：在 `UserManager::login()` 載入前，主動清空該使用者現有的交易與預算物件，並妥善釋放記憶體。
- **動態更新交易計數器 (Bug 2)**：在 `ConsoleUtils` 中新增 `setTxnCounter()` 介面，並在登入讀檔後掃描現有交易列表中最大 ID 值來動態初始化計數器，以避開衝突 ID。
- **新增安全整數輸入防呆 (Bug 3)**：在 `ConsoleUtils` 中實作 `inputInt()` 方法，處理輸入串流錯誤狀態，並替換所有選單的 `std::cin >>` 輸入點。

## Capabilities

### New Capabilities
- `safe-integer-input`: 支援安全的終端機整數輸入，自動過濾無效字元與重設緩衝區。
- `auto-id-conflict-resolution`: 登入時自動計算新交易 ID 的起始值以避免衝突。

### Modified Capabilities
- `user-account`: 登入邏輯新增資料重置與防重複載入防護。

## Impact

- **修改原始碼檔案**：
  - `include/common/ConsoleUtils.h` / `src/common/ConsoleUtils.cpp` (新增 `inputInt` 與 `setTxnCounter`)
  - `src/auth/UserManager.cpp` (新增登入清檔重設、動態計數器設定)
  - `src/auth/AuthUI.cpp` / `src/transaction_ui/TransactionUI.cpp` / `src/budget/BudgetUI.cpp` (替換選單輸入為 `inputInt`)
