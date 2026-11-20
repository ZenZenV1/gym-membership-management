#include "GymManagementSystem.h"

#include "DateUtils.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <utility>
#include <vector>

namespace {
const std::filesystem::path dataDirectory("data");

std::string safeField(std::string value) {
    std::replace(value.begin(), value.end(), '|', '/');
    std::replace(value.begin(), value.end(), '\n', ' ');
    std::replace(value.begin(), value.end(), '\r', ' ');
    return value;
}

std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream input(line);
    std::string field;
    while (std::getline(input, field, '|')) fields.push_back(field);
    if (!line.empty() && line.back() == '|') fields.emplace_back();
    return fields;
}

bool toInt(const std::string& value, int& result) {
    try {
        std::size_t used = 0;
        result = std::stoi(value, &used);
        return used == value.size();
    } catch (...) {
        return false;
    }
}

bool toDouble(const std::string& value, double& result) {
    try {
        std::size_t used = 0;
        result = std::stod(value, &used);
        return used == value.size();
    } catch (...) {
        return false;
    }
}

bool toBool(const std::string& value, bool& result) {
    if (value == "1") { result = true; return true; }
    if (value == "0") { result = false; return true; }
    return false;
}

std::string statusFor(const Member& member) {
    if (!member.isActive()) return "Inactive";
    const int today = DateUtils::key(DateUtils::today());
    if (DateUtils::key(member.getStartDate()) > today) return "Upcoming";
    if (DateUtils::key(member.getEndDate()) < today) return "Expired";
    return "Active";
}

void saveAfterChange(const GymManagementSystem& system, const std::string& successMessage,
                     std::string& message) {
    std::string saveMessage;
    if (system.saveData(saveMessage)) {
        message = successMessage;
    } else {
        message = successMessage + " The change is in memory, but saving failed: " + saveMessage;
    }
}
}

bool GymManagementSystem::loadData(std::string& message) {
    try {
        std::filesystem::create_directories(dataDirectory);
        members.clear();
        plans.clear();
        payments.clear();

        std::ifstream planFile(dataDirectory / "plans.txt");
        std::string line;
        while (std::getline(planFile, line)) {
            const auto fields = split(line);
            int id, days;
            double price;
            bool active;
            if (fields.size() == 5 && toInt(fields[0], id) && toInt(fields[2], days) &&
                toDouble(fields[3], price) && toBool(fields[4], active) && id > 0 && days > 0)
                plans.emplace_back(id, fields[1], days, price, active);
        }

        std::ifstream memberFile(dataDirectory / "members.txt");
        while (std::getline(memberFile, line)) {
            const auto fields = split(line);
            int id, planId;
            bool active;
            if (fields.size() == 8 && toInt(fields[0], id) && toInt(fields[4], planId) &&
                toBool(fields[7], active) && id > 0 && DateUtils::isValid(fields[5]) &&
                DateUtils::isValid(fields[6]))
                members.emplace_back(id, fields[1], fields[2], fields[3], planId,
                                     fields[5], fields[6], active);
        }

        std::ifstream paymentFile(dataDirectory / "payments.txt");
        while (std::getline(paymentFile, line)) {
            const auto fields = split(line);
            int id, memberId;
            double amount;
            if (fields.size() == 5 && toInt(fields[0], id) && toInt(fields[1], memberId) &&
                toDouble(fields[2], amount) && id > 0 && memberId > 0 && amount >= 0 &&
                DateUtils::isValid(fields[3]))
                payments.emplace_back(id, memberId, amount, fields[3], fields[4]);
        }
        message = "Data loaded from the data folder.";
        return true;
    } catch (const std::exception& error) {
        message = error.what();
        return false;
    }
}

bool GymManagementSystem::saveData(std::string& message) const {
    try {
        std::filesystem::create_directories(dataDirectory);
        std::ofstream planFile(dataDirectory / "plans.txt", std::ios::trunc);
        std::ofstream memberFile(dataDirectory / "members.txt", std::ios::trunc);
        std::ofstream paymentFile(dataDirectory / "payments.txt", std::ios::trunc);
        if (!planFile || !memberFile || !paymentFile) {
            message = "Unable to open one or more data files.";
            return false;
        }
        planFile << std::fixed << std::setprecision(2);
        paymentFile << std::fixed << std::setprecision(2);
        for (const auto& plan : plans)
            planFile << plan.getId() << '|' << safeField(plan.getName()) << '|'
                     << plan.getDurationDays() << '|' << plan.getPrice() << '|'
                     << (plan.isActive() ? 1 : 0) << '\n';
        for (const auto& member : members)
            memberFile << member.getId() << '|' << safeField(member.getName()) << '|'
                       << safeField(member.getPhone()) << '|' << safeField(member.getEmail()) << '|'
                       << member.getPlanId() << '|' << member.getStartDate() << '|'
                       << member.getEndDate() << '|' << (member.isActive() ? 1 : 0) << '\n';
        for (const auto& payment : payments)
            paymentFile << payment.getId() << '|' << payment.getMemberId() << '|'
                        << payment.getAmount() << '|' << payment.getDate() << '|'
                        << safeField(payment.getNote()) << '\n';
        if (!planFile || !memberFile || !paymentFile) {
            message = "A write error occurred while saving data.";
            return false;
        }
        message = "Data saved successfully.";
        return true;
    } catch (const std::exception& error) {
        message = error.what();
        return false;
    }
}

bool GymManagementSystem::registerMember(const std::string& name, const std::string& phone,
                                         const std::string& email, int planId,
                                         std::string& message) {
    MembershipPlan* plan = findPlan(planId);
    if (name.empty() || phone.empty()) { message = "Name and phone are required."; return false; }
    if (!plan || !plan->isActive()) { message = "Choose an existing active plan."; return false; }
    const std::string start = DateUtils::today();
    const std::string end = DateUtils::addDays(start, plan->getDurationDays() - 1);
    members.emplace_back(nextMemberId(), safeField(name), safeField(phone), safeField(email),
                         planId, start, end);
    addPaymentRecord(members.back().getId(), plan->getPrice(), "Membership registration: " + plan->getName());
    saveAfterChange(*this, "Member registered with ID " + std::to_string(members.back().getId()) +
                    ". Payment of " + std::to_string(plan->getPrice()) + " recorded; valid through " + end + ".", message);
    return true;
}

bool GymManagementSystem::updateMemberContact(int memberId, const std::string& phone,
                                               const std::string& email, std::string& message) {
    Member* member = findMember(memberId);
    if (!member) { message = "Member not found."; return false; }
    if (phone.empty()) { message = "Phone cannot be empty."; return false; }
    member->updateContact(safeField(phone), safeField(email));
    saveAfterChange(*this, "Member contact details updated.", message);
    return true;
}

bool GymManagementSystem::deactivateMember(int memberId, std::string& message) {
    Member* member = findMember(memberId);
    if (!member) { message = "Member not found."; return false; }
    member->setActive(false);
    saveAfterChange(*this, "Member deactivated.", message);
    return true;
}

bool GymManagementSystem::renewMembership(int memberId, int planId, std::string& message) {
    Member* member = findMember(memberId);
    MembershipPlan* plan = findPlan(planId);
    if (!member) { message = "Member not found."; return false; }
    if (!plan || !plan->isActive()) { message = "Choose an existing active plan."; return false; }
    const std::string today = DateUtils::today();
    std::string start = today;
    if (DateUtils::key(member->getEndDate()) >= DateUtils::key(today))
        start = DateUtils::addDays(member->getEndDate(), 1);
    const std::string end = DateUtils::addDays(start, plan->getDurationDays() - 1);
    member->updateMembership(planId, start, end);
    addPaymentRecord(memberId, plan->getPrice(), "Membership renewal: " + plan->getName());
    saveAfterChange(*this, "Membership renewed. Payment of " + std::to_string(plan->getPrice()) +
                    " recorded; new validity is " + start + " through " + end + ".", message);
    return true;
}

bool GymManagementSystem::addPlan(const std::string& name, int durationDays, double price,
                                  std::string& message) {
    if (name.empty()) { message = "Plan name is required."; return false; }
    if (durationDays < 1 || durationDays > 3650 || price <= 0) {
        message = "Duration must be 1-3650 days and price must be positive."; return false;
    }
    plans.emplace_back(nextPlanId(), safeField(name), durationDays, price);
    saveAfterChange(*this, "Plan added with ID " + std::to_string(plans.back().getId()) + ".", message);
    return true;
}

bool GymManagementSystem::editPlan(int planId, const std::string& name, int durationDays,
                                   double price, std::string& message) {
    MembershipPlan* plan = findPlan(planId);
    if (!plan) { message = "Plan not found."; return false; }
    if (name.empty() || durationDays < 1 || durationDays > 3650 || price <= 0) {
        message = "Enter a name, a duration from 1-3650 days, and a positive price."; return false;
    }
    plan->update(safeField(name), durationDays, price);
    saveAfterChange(*this, "Plan updated.", message);
    return true;
}

bool GymManagementSystem::togglePlan(int planId, std::string& message) {
    MembershipPlan* plan = findPlan(planId);
    if (!plan) { message = "Plan not found."; return false; }
    plan->setActive(!plan->isActive());
    saveAfterChange(*this, plan->isActive() ? "Plan activated." : "Plan deactivated.", message);
    return true;
}

bool GymManagementSystem::recordPayment(int memberId, double amount, const std::string& note,
                                        std::string& message) {
    if (!findMember(memberId)) { message = "Member not found."; return false; }
    if (amount <= 0) { message = "Payment amount must be positive."; return false; }
    addPaymentRecord(memberId, amount, note.empty() ? "Additional payment" : safeField(note));
    saveAfterChange(*this, "Payment recorded with receipt number " +
                    std::to_string(payments.back().getId()) + ".", message);
    return true;
}

Member* GymManagementSystem::findMember(int id) {
    for (auto& member : members) if (member.getId() == id) return &member;
    return nullptr;
}
const Member* GymManagementSystem::findMember(int id) const {
    for (const auto& member : members) if (member.getId() == id) return &member;
    return nullptr;
}
MembershipPlan* GymManagementSystem::findPlan(int id) {
    for (auto& plan : plans) if (plan.getId() == id) return &plan;
    return nullptr;
}
const MembershipPlan* GymManagementSystem::findPlan(int id) const {
    for (const auto& plan : plans) if (plan.getId() == id) return &plan;
    return nullptr;
}

std::string GymManagementSystem::membershipStatus(const Member& member) const {
    return statusFor(member);
}

bool GymManagementSystem::generateReport(std::string& message) const {
    try {
        std::filesystem::create_directories("reports");
        std::ofstream report("reports/gym_report.txt", std::ios::trunc);
        if (!report) { message = "Unable to create reports/gym_report.txt."; return false; }
        int active = 0, expired = 0, inactive = 0, upcoming = 0;
        double revenue = 0.0;
        for (const auto& member : members) {
            const std::string status = membershipStatus(member);
            if (status == "Active") ++active;
            else if (status == "Expired") ++expired;
            else if (status == "Upcoming") ++upcoming;
            else ++inactive;
        }
        for (const auto& payment : payments) revenue += payment.getAmount();

        report << "GYM MEMBERSHIP MANAGEMENT SYSTEM REPORT\n"
               << "Generated: " << DateUtils::today() << "\n\n"
               << "MEMBERSHIP SUMMARY\n"
               << "Total registered members: " << members.size() << '\n'
               << "Active memberships: " << active << '\n'
               << "Expired memberships: " << expired << '\n'
               << "Upcoming memberships: " << upcoming << '\n'
               << "Deactivated members: " << inactive << "\n\n"
               << "PAYMENT SUMMARY\n"
               << "Payments recorded: " << payments.size() << '\n'
               << std::fixed << std::setprecision(2)
               << "Total recorded revenue: " << revenue << "\n\n"
               << "PAYMENT DETAILS\n";
        for (const auto& payment : payments) {
            const Member* member = findMember(payment.getMemberId());
            report << "Receipt " << payment.getId() << " | " << payment.getDate()
                   << " | Member " << payment.getMemberId()
                   << (member ? " (" + member->getName() + ")" : "")
                   << " | " << payment.getAmount() << " | " << payment.getNote() << '\n';
        }
        message = "Report generated at reports/gym_report.txt.";
        return true;
    } catch (const std::exception& error) {
        message = error.what();
        return false;
    }
}

const std::vector<Member>& GymManagementSystem::getMembers() const { return members; }
const std::vector<MembershipPlan>& GymManagementSystem::getPlans() const { return plans; }
const std::vector<Payment>& GymManagementSystem::getPayments() const { return payments; }

int GymManagementSystem::nextMemberId() const {
    int highest = 0;
    for (const auto& member : members) highest = std::max(highest, member.getId());
    return highest + 1;
}
int GymManagementSystem::nextPlanId() const {
    int highest = 0;
    for (const auto& plan : plans) highest = std::max(highest, plan.getId());
    return highest + 1;
}
int GymManagementSystem::nextPaymentId() const {
    int highest = 0;
    for (const auto& payment : payments) highest = std::max(highest, payment.getId());
    return highest + 1;
}
void GymManagementSystem::addPaymentRecord(int memberId, double amount, const std::string& note) {
    payments.emplace_back(nextPaymentId(), memberId, amount, DateUtils::today(), safeField(note));
}
