## Why

當前的「學生個人財務管理系統」包含多個不同功能模組（帳戶、交易、預算、報表），但目前有大量的輔助函式、介面選單邏輯都混合在 `main.cpp` 當中（共計 567 行）。隨著專案的演進，這種單一檔案的架構（Monolith）會大幅降低程式的可讀性與可維護性，也不利於期末專題的架構展示與多人分工開發。

同時，系統運行期間產生的交易資料、預算設定、以及匯出的報表目前都是扁平地堆放在 `data/` 和 `reports/` 下，沒有進行層級化的管理。

本提案旨在透過「方案 A：程式碼模組化」與「方案 B：運行資料層級化」雙管齊下，將程式結構與數據存儲均進行拆分，達到更優雅的 C++ 物件導向與多租戶架構設計。

## What Changes

- **方案 A (程式碼層面)**：
  - 將 `main.cpp` 瘦身，僅保留程式進入點 `main()`。
  - 將終端機控制與輸入防呆函式提取至 `common/ConsoleUtils`。
  - 依開發迭代階段，將介面選單拆分為 `auth/AuthUI`、`transaction_ui/TransactionUI`、`budget/BudgetUI` 以及 `report/ReportUI`。
  - 修改 `Makefile`，使其支援子目錄編譯。
- **方案 B (運行數據層面)**：
  - 修改 `FileManager`，將每個學生的個人交易記錄 `txn.txt` 與預算設定 `budget.txt` 儲存於 `data/<student_id>/` 子目錄下，實現更徹底的實體資料隔離。
  - 修改 `ReportGenerator`，將匯出的報表檔儲存於 `reports/<YYYY-MM>/` 子目錄下，實現按月份層級歸檔。
  - 使用 C++17 標準庫 `<filesystem>` 跨平台處理資料夾的動態建立。

## Capabilities

### New Capabilities
- `modular-console-ui`: 以解耦的模組化結構管理各迭代階段的 UI 選單與控制邏輯。
- `hierarchical-storage`: 使用層級化的資料目錄與報表目錄管理使用者運行時產生的資料。

### Modified Capabilities
- `data-persistence`: 檔案讀寫路徑變更為學生專屬資料夾 (`data/<student_id>/`)。
- `report-export`: 報表匯出路徑變更為月份專屬資料夾 (`reports/<YYYY-MM>/`)。

## Impact

- **新增原始碼檔案**：
  - `include/common/ConsoleUtils.h`, `src/common/ConsoleUtils.cpp`
  - `include/auth/AuthUI.h`, `src/auth/AuthUI.cpp`
  - `include/transaction_ui/TransactionUI.h`, `src/transaction_ui/TransactionUI.cpp`
  - `include/budget/BudgetUI.h`, `src/budget/BudgetUI.cpp`
  - `include/report/ReportUI.h`, `src/report/ReportUI.cpp`
- **修改原始碼檔案**：
  - `main.cpp` (大幅精簡)
  - `include/persistence/FileManager.h`, `src/persistence/FileManager.cpp` (變更儲存路徑，新增 `<filesystem>` 邏輯)
  - `include/report/ReportGenerator.h`, `src/report/ReportGenerator.cpp` (變更報表路徑，新增 `<filesystem>` 邏輯)
  - `Makefile` (更新編譯規則，涵蓋子目錄 `src/**/*.cpp`)
- **資料與報表目錄結構**：
  - `data/` 下新增 `<student_id>/` 夾。
  - `reports/` 下新增 `<YYYY-MM>/` 夾。
