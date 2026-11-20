#include "DateUtils.h"

#include <ctime>
#include <iomanip>
#include <sstream>

namespace {
bool leapYear(int year) {
    return year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
}

int daysInMonth(int year, int month) {
    const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && leapYear(year)) return 29;
    return days[month - 1];
}

bool parse(const std::string& date, int& year, int& month, int& day) {
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') return false;
    try {
        year = std::stoi(date.substr(0, 4));
        month = std::stoi(date.substr(5, 2));
        day = std::stoi(date.substr(8, 2));
    } catch (...) {
        return false;
    }
    return month >= 1 && month <= 12 && day >= 1 && day <= daysInMonth(year, month);
}
}

namespace DateUtils {
std::string today() {
    const std::time_t now = std::time(nullptr);
    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif
    std::ostringstream output;
    output << std::put_time(&localTime, "%Y-%m-%d");
    return output.str();
}

bool isValid(const std::string& date) {
    int year, month, day;
    return parse(date, year, month, day);
}

int key(const std::string& date) {
    int year, month, day;
    if (!parse(date, year, month, day)) return 0;
    return year * 10000 + month * 100 + day;
}

std::string addDays(const std::string& date, int days) {
    int year, month, day;
    if (!parse(date, year, month, day) || days < 0) return "";
    while (days-- > 0) {
        ++day;
        if (day > daysInMonth(year, month)) {
            day = 1;
            ++month;
            if (month > 12) {
                month = 1;
                ++year;
            }
        }
    }
    std::ostringstream output;
    output << std::setfill('0') << std::setw(4) << year << '-'
           << std::setw(2) << month << '-' << std::setw(2) << day;
    return output.str();
}
}
