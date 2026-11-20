#include "MembershipPlan.h"

#include <utility>

MembershipPlan::MembershipPlan(int id, std::string name, int durationDays, double price, bool active)
    : id(id), name(std::move(name)), durationDays(durationDays), price(price), active(active) {}

int MembershipPlan::getId() const { return id; }
const std::string& MembershipPlan::getName() const { return name; }
int MembershipPlan::getDurationDays() const { return durationDays; }
double MembershipPlan::getPrice() const { return price; }
bool MembershipPlan::isActive() const { return active; }

void MembershipPlan::update(const std::string& newName, int newDurationDays, double newPrice) {
    name = newName;
    durationDays = newDurationDays;
    price = newPrice;
}

void MembershipPlan::setActive(bool value) { active = value; }
