#ifndef MEMBERSHIP_PLAN_H
#define MEMBERSHIP_PLAN_H

#include <string>

class MembershipPlan {
public:
    MembershipPlan(int id, std::string name, int durationDays, double price, bool active = true);

    int getId() const;
    const std::string& getName() const;
    int getDurationDays() const;
    double getPrice() const;
    bool isActive() const;

    void update(const std::string& name, int durationDays, double price);
    void setActive(bool active);

private:
    int id;
    std::string name;
    int durationDays;
    double price;
    bool active;
};

#endif
