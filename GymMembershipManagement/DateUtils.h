#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include <string>

namespace DateUtils {
std::string today();
bool isValid(const std::string& date);
std::string addDays(const std::string& date, int days);
int key(const std::string& date);
}

#endif
