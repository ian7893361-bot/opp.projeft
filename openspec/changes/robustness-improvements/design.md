## Context

本變更旨在解決記帳系統中因「重複登入」、「程式重啟 ID 重置」與「非數字輸入選單」引發的軟體穩定性缺陷，在不更改核心資料結構與持久化格式的前提下，增強防禦性程式設計 (Defensive Programming)。

## Goals / Non-Goals

**Goals:**
- 在使用者登入時，徹底清空現有的交易與預算結構（包括指標釋放），防止多次登入導致資料重疊與記憶體洩漏。
- 在每次登入讀檔後，依據最大交易 ID 數字重新設定新交易的 ID 起始數字。
- 封裝安全輸入整數函式，避免鍵盤輸入字母引發選單死循環。

**Non-Goals:**
- 不引入外部數據庫。
- 不修改使用者權限或基礎資料模型（如 `Student`、`Transaction` 的核心定義）。

## Decisions

### 決策 1：登入重置防護 (Bug 1)
每次 `UserManager::login(studentId)` 被呼叫時，我們在從 `users` map 取得該 `Student*` 後，進行以下重置操作：
```cpp
auto& txns = currentUser->getTransactions();
for (Transaction* t : txns) {
    delete t; // 釋放記憶體
}
txns.clear();
currentUser->getBudgets().clear();
currentUser->addToBalance(-currentUser->getBalance()); // 餘額歸零
```
這能確保無論使用者在同一次執行中登出/登入多少次，載入到記憶體中的資料永遠是一份，且不會有記憶體洩漏。

---

### 決策 2：交易 ID 掃描與計數器動態重設 (Bug 2)
1. 在 `ConsoleUtils.h` 與 `ConsoleUtils.cpp` 中新增 `void setTxnCounter(int val)` 以更新 `txnCounter` 狀態。
2. 於 `UserManager::login` 載入所有交易（`loadTransactions`）後，執行最大 ID 的數值部分查找：
   ```cpp
   int maxIdVal = 0;
   for (const auto* t : currentUser->getTransactions()) {
       std::string tid = t->getId();
       if (tid.size() > 1 && tid[0] == 'T') {
           try {
               int val = std::stoi(tid.substr(1));
               if (val > maxIdVal) maxIdVal = val;
           } catch (...) {}
       }
   }
   ConsoleUtils::setTxnCounter(maxIdVal + 1);
   ```

---

### 決策 3：安全的整數輸入與選單替換 (Bug 3)
1. 實作 `ConsoleUtils::inputInt(prompt)`：
   ```cpp
   int inputInt(const std::string& prompt) {
       int val;
       while (true) {
           std::cout << prompt;
           if (std::cin >> val) {
               return val;
           }
           std::cin.clear();
           std::cin.ignore(10000, '\n');
           std::cout << "  ✗ 輸入無效，請輸入一個整數數字。\n";
       }
   }
   ```
2. 將 `AuthUI`、`TransactionUI`、`BudgetUI` 裡所有的 `std::cin >> ch` 與 `std::cin >> typeChoice` 替換為 `ConsoleUtils::inputInt(...)`。

## Risks / Trade-offs

| 風險 | 影響 | 緩解方式 |
|------|------|---------|
| 舊交易 ID 格式不匹配 | 若歷史交易 ID 不是 `T00001` 等格式（如 `TXN_99`），可能無法正確轉換 | 計數器掃描已加入 `try-catch` 保護，如解析失敗則跳過，不會導致崩潰，且會從大於零的預設值繼續。 |
