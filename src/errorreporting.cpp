#include "errorreporting.h"
#include <fmt/core.h>

std::vector<ErrorReporter::Report> ErrorReporter::m_reports;

void ErrorReporter::printReports()
{
    for (auto& report : ErrorReporter::reports()) {
        fmt::print("Error: {}:{} {}\n", report.file, report.line, report.message);
    }
}

