#include "report/ReportUI.h"
#include "common/ConsoleUtils.h"
#include "report/ReportGenerator.h"
#include <iostream>

void ReportUI::showExportReport(Student* student) {
    ConsoleUtils::clearScreen();
    ConsoleUtils::printHeader("匯出月份報表");
    std::string ym = ConsoleUtils::inputNonEmpty("輸入年月（格式 YYYY-MM）：");
    if (ReportGenerator::generate(student, ym)) {
        std::cout << "\n  ✓ 報表已儲存至：reports/"
                  << ym << "/report_" << student->getId() << ".txt\n";
    } else {
        std::cout << "\n  ✗ 該月份（" << ym << "）無交易記錄，未產生報表。\n";
    }
}
