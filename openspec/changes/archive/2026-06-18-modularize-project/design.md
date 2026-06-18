## Context

本變更旨在將學生的財務管理系統從「單一檔案主介面 + 扁平資料存儲」重構成為「模組化介面 + 分層資料存儲」。此設計重在實踐 C++17 跨平台 `<filesystem>`、物件導向的模組化拆分，以及提升開發架構的清晰度。

## Goals / Non-Goals

**Goals:**
- 將 `main.cpp` 的 UI 選單依迭代功能完全解耦，分別移至獨立的模組中。
- 將公用終端工具、輸入驗證提煉為 `common/ConsoleUtils`。
- 修改 `FileManager`，使每個使用者的 `txn.txt` 與 `budget.txt` 儲存於各自的專屬資料夾 `data/<student_id>/`。
- 修改 `ReportGenerator`，使匯出報表儲存於 `reports/<YYYY-MM>/`。
- 採用 C++17 `<filesystem>` 取代 `system("mkdir ...")` 的系統級指令呼叫，提升跨平台安全性與可移植性。
- 更新 `Makefile` 支援遞迴尋找或指定子路徑編譯。

**Non-Goals:**
- 不改變系統的核心邏輯（如記帳方式、預算運算等業務邏輯）。
- 不更換儲存格式（依然保持原有的 key=value 文字塊格式）。
- 不引入外部 GUI 或數據庫。

## Decisions

### 決策 1：程式碼目錄分層 (方案 A)
將 `include/` 與 `src/` 下區分成對應的 6 個子資料夾：
- `common/`: 通用工具（包含 ANSI 彩色代碼定義、寬度計算、防呆輸入）
- `model/`: core 繼承體系的模型（`Person`, `Student`, `Admin`, `Transaction`, `Income`, `Expense` 等）
- `persistence/`: 資料庫讀寫 (`FileManager`)
- `auth/`: 使用者帳號與系統入口選單 (`UserManager`, `AuthUI`)
- `transaction_ui/`: 交易記錄增刪查改 UI 選單 (`TransactionUI`)
- `budget/`: 預算設定與狀態警示 UI 選單 (`BudgetUI`)
- `report/`: 報表生成與匯出 UI 選單 (`ReportGenerator`, `ReportUI`)

在包含標頭檔時，編譯參數維持 `-I include`，程式中統一使用子資料夾包含，例如：
```cpp
#include "model/Student.h"
#include "common/ConsoleUtils.h"
```

---

### 決策 2：運行資料與報表的層級化儲存 (方案 B)

#### 使用者專屬資料夾
當前學號為 `S001` 的使用者登入時：
* `FileManager` 會嘗試檢查並建立目錄：`data/S001/`
* 其交易紀錄載入/儲存路徑變更為：`data/S001/txn.txt`
* 其預算設定載入/儲存路徑變更為：`data/S001/budget.txt`

#### 報表月份分層歸檔
當使用者選擇匯出 `2026-06` 的月份報表時：
* 系統會自動檢查並建立目錄：`reports/2026-06/`
* 其報表寫入路徑變更為：`reports/2026-06/report_S001.txt`

#### 目錄建立代碼實作（跨平台）
我們將使用 C++17 `<filesystem>` 的靜態函式來處理目錄建立：
```cpp
#include <filesystem>

// 確保父目錄與個人目錄存在
std::filesystem::create_directories("data/" + studentId);
std::filesystem::create_directories("reports/" + yearMonth);
```
這比執行 Windows/Linux 的 `system("mkdir ...")` 指令更為乾淨安全。

---

### 決策 3：Makefile 編譯規則更新

重構後原始碼分散於多個子目錄中，我們需要更新 `Makefile`。
我們可以明確列出所有的 `.cpp` 檔案，或是使用 `wildcard` 函式搜尋：

```makefile
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -I include
TARGET   = finance_manager

# 遍歷所有子目錄中的 .cpp 原始碼
SRCS     = main.cpp \
           $(wildcard src/common/*.cpp) \
           $(wildcard src/model/*.cpp) \
           $(wildcard src/persistence/*.cpp) \
           $(wildcard src/auth/*.cpp) \
           $(wildcard src/transaction_ui/*.cpp) \
           $(wildcard src/budget/*.cpp) \
           $(wildcard src/report/*.cpp)

all:
	$(CXX) $(CXXFLAGS) -static $(SRCS) -o $(TARGET).exe

linux:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	del /Q $(TARGET).exe 2>nul & rm -f $(TARGET)

.PHONY: all linux clean
```

---

## Risks / Trade-offs

| 風險 | 影響 | 緩解方式 |
|------|------|---------|
| 歷史資料相容性 | 已存在的扁平資料（如 `data/txn_S001.txt`）不會自動移動到 `data/S001/txn.txt` | 1. 撰寫一個一次性的遷移代碼（若舊檔存在則在登入時自動移動至新目錄）。<br>2. 或在重構後手動執行資料清理與升級（期末專題全新執行時則無此問題）。 |
| 標頭檔依賴循環 (Circular Dependency) | 拆分 UI 模組後，模組間可能互相呼叫 | 確保主流程單向傳遞：`AuthUI` 呼叫 `TransactionUI`/`BudgetUI`/`ReportUI`，而各子 UI 僅依賴 `ConsoleUtils` 與 `model/` 資料物件，不互相包含。 |
