# Robustness Improvements Specifications

本規格書定義系統的健壯性與安全性規則。

## Capabilities

### `safe-integer-input`

- 系統所有整數型別選單的鍵盤輸入必須經過防呆處理。
- 當使用者輸入包含非數字字元時，系統不得崩潰或進入死循環，必須清除輸入錯誤標記並丟棄無效緩衝區，提示錯誤後等待重新輸入。

### `auto-id-conflict-resolution`

- 當使用者登入時，系統必須動態掃描其歷史交易記錄，找出最大的交易 ID 數字。
- 接下來新增交易時產生的 ID 必須為最大 ID 的數值部分加 1，以防止交易 ID 衝突與檔案資料覆蓋。
