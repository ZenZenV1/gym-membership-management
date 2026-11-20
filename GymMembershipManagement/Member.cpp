#include "Member.h"

#include <utility>

Member::Member(int id, std::string name, std::string phone, std::string email,
               int planId, std::string startDate, std::string endDate, bool active)
    : id(id), name(std::move(name)), phone(std::move(phone)), email(std::move(email)),
      planId(planId), startDate(std::move(startDate)), endDate(std::move(endDate)), active(active) {}

int Member::getId() const { return id; }
const std::string& Member::getName() const { return name; }
const std::string& Member::getPhone() const { return phone; }
const std::string& Member::getEmail() const { return email; }
int Member::getPlanId() const { return planId; }
const std::string& Member::getStartDate() const { return startDate; }
const std::string& Member::getEndDate() const { return endDate; }
bool Member::isActive() const { return active; }

void Member::updateContact(const std::string& newPhone, const std::string& newEmail) {
    phone = newPhone;
    email = newEmail;
}

void Member::updateMembership(int newPlanId, const std::string& newStartDate,
                              const std::string& newEndDate) {
    planId = newPlanId;
    startDate = newStartDate;
    endDate = newEndDate;
    active = true;
}

void Member::setActive(bool value) { active = value; }
