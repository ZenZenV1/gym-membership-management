#ifndef GYM_MANAGEMENT_SYSTEM_H
#define GYM_MANAGEMENT_SYSTEM_H

#include "Member.h"
#include "MembershipPlan.h"
#include "Payment.h"

#include <string>
#include <vector>

class GymManagementSystem {
public:
    bool loadData(std::string& message);
    bool saveData(std::string& message) const;

    bool registerMember(const std::string& name, const std::string& phone,
                        const std::string& email, int planId, std::string& message);
    bool updateMemberContact(int memberId, const std::string& phone,
                             const std::string& email, std::string& message);
    bool deactivateMember(int memberId, std::string& message);
    bool renewMembership(int memberId, int planId, std::string& message);

    bool addPlan(const std::string& name, int durationDays, double price, std::string& message);
    bool editPlan(int planId, const std::string& name, int durationDays,
                  double price, std::string& message);
    bool togglePlan(int planId, std::string& message);
    bool recordPayment(int memberId, double amount, const std::string& note, std::string& message);

    Member* findMember(int id);
    const Member* findMember(int id) const;
    MembershipPlan* findPlan(int id);
    const MembershipPlan* findPlan(int id) const;
    std::string membershipStatus(const Member& member) const;
    bool generateReport(std::string& message) const;

    const std::vector<Member>& getMembers() const;
    const std::vector<MembershipPlan>& getPlans() const;
    const std::vector<Payment>& getPayments() const;

private:
    std::vector<Member> members;
    std::vector<MembershipPlan> plans;
    std::vector<Payment> payments;

    int nextMemberId() const;
    int nextPlanId() const;
    int nextPaymentId() const;
    void addPaymentRecord(int memberId, double amount, const std::string& note);
};

#endif
