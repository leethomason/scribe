#pragma once

#include <string>
#include <vector>

class ErrorReporter {
public:
    struct Report {
        std::string file;
        int line = 0;
        std::string message;
    };

    static void reportSyntax(const std::string& file, int line, const std::string& message) {
        report(file, line, message);
	}

    static void report(const std::string& file, int line, const std::string& message) {
        m_reports.push_back({file, line, message});
    }

    static void reportRuntime(const std::string& message) {
		report("", 0, message);
	}

    static void printReports();

    static const std::vector<Report>& reports() {
        return m_reports;
    }
    static void clear() {
        m_reports.clear();
    }
    static bool hasError() {
        return m_reports.size() > 0;
    }

private:
    static std::vector<Report> m_reports;
};
