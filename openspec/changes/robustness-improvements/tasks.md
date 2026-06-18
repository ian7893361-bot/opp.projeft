## 1. 解決重複登入資料重複載入 (Bug 1)

- [x] 1.1 修改 `src/auth/UserManager.cpp` 中的 `login` 方法
- [x] 1.2 在 `loadTransactions` 之前，遍歷 `currentUser->getTransactions()`，逐一 `delete` 指標並執行 `clear()`
- [x] 1.3 清空 `currentUser->getBudgets()`，並將餘額重設為 `0`

## 2. 解決交易 ID 衝突 (Bug 2)

- [x] 2.1 在 `include/common/ConsoleUtils.h` 中宣告 `void setTxnCounter(int val);`
- [x] 2.2 在 `src/common/ConsoleUtils.cpp` 中實作 `setTxnCounter(int val)` 以更新靜態的 `txnCounter`
- [x] 2.3 修改 `src/auth/UserManager.cpp` 的 `login` 方法，在載入交易後，遍歷交易列表，解析 `TXXXXX` 字串，找出最大數值
- [x] 2.4 呼叫 `ConsoleUtils::setTxnCounter(maxVal + 1)` 初始化計數器

## 3. 實作安全整數輸入與選單防呆 (Bug 3)

- [x] 3.1 在 `include/common/ConsoleUtils.h` 中宣告 `int inputInt(const std::string& prompt);`
- [x] 3.2 在 `src/common/ConsoleUtils.cpp` 中實作 `inputInt(prompt)`，清除 cin 錯誤標記並無視多餘緩衝區
- [x] 3.3 修改 `src/auth/AuthUI.cpp`，將所有 `std::cin >> ch` 替換為 `ConsoleUtils::inputInt(...)`
- [x] 3.4 修改 `src/transaction_ui/TransactionUI.cpp`，將所有 `std::cin >> ch` 與 `std::cin >> typeChoice` 替換為 `ConsoleUtils::inputInt(...)`
- [x] 3.5 修改 `src/budget/BudgetUI.cpp`，將選單輸入替換為 `ConsoleUtils::inputInt(...)`

## 4. 專案編譯與驗證

- [x] 4.1 執行編譯，驗證編譯無誤
- [x] 4.2 驗證重複登入：登入 S001 後登出，再次登入，驗證餘額與交易筆數保持一致 (不會翻倍)
- [x] 4.3 驗證 ID 衝突：登入後新增交易，驗證產生的 ID 是否為已存在 ID 遞增 (例如 T00002)
- [x] 4.4 驗證輸入防呆：在主選單與個人選單輸入 `a` 或 `hello`，驗證系統是否正確印出錯誤提示並等待重新輸入，而不會進入死循環
