#pragma once
#include "Student.h"
#include <string>

// ============================================================
//  ReportGenerator — 生成月份財務報表並寫入檔案
// ============================================================
class ReportGenerator {
public:
    // yearMonth 格式：YYYY-MM（例："2026-06"）
    // 回傳 true 表示成功，false 表示該月無資料
    static bool generate(const Student* student, const std::string& yearMonth);

private:
    static std::string reportPath(const std::string& studentId,
                                  const std::string& yearMonth);
};
