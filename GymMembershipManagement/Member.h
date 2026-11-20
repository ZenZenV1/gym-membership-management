#ifndef MEMBER_H
#define MEMBER_H

#include <string>

class Member {
public:
    Member(int id, std::string name, std::string phone, std::string email,
           int planId, std::string startDate, std::string endDate, bool active = true);

    int getId() const;
    const std::string& getName() const;
    const std::string& getPhone() const;
    const std::string& getEmail() const;
    int getPlanId() const;
    const std::string& getStartDate() const;
    const std::string& getEndDate() const;
    bool isActive() const;

    void updateContact(const std::string& phone, const std::string& email);
    void updateMembership(int planId, const std::string& startDate, const std::string& endDate);
    void setActive(bool active);

private:
    int id;
    std::string name;
    std::string phone;
    std::string email;
    int planId;
    std::string startDate;
    std::string endDate;
    bool active;
};

#endif
