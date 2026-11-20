#include "GymManagementSystem.h"

#include <iostream>
#include <limits>
#include <string>

namespace {
int readInt(const std::string& prompt, int minimum, int maximum) {
    while (true) {
        std::cout << prompt;
        int value;
        if (std::cin >> value && value >= minimum && value <= maximum) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cout << "Please enter a number from " << minimum << " to " << maximum << ".\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

double readMoney(const std::string& prompt, double minimum = 0.01) {
    while (true) {
        std::cout << prompt;
        double value;
        if (std::cin >> value && value >= minimum) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cout << "Please enter an amount of at least " << minimum << ".\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

void showPlans(const GymManagementSystem& system, bool activeOnly = false) {
    const auto& plans = system.getPlans();
    bool found = false;
    std::cout << "\nID   Plan                         Days       Price       State\n";
    std::cout << "---------------------------------------------------------------\n";
    for (const auto& plan : plans) {
        if (activeOnly && !plan.isActive()) continue;
        found = true;
        std::cout << plan.getId() << "    " << plan.getName() << "    "
                  << plan.getDurationDays() << "         " << plan.getPrice() << "      "
                  << (plan.isActive() ? "Available" : "Inactive") << '\n';
    }
    if (!found) std::cout << "No plans found.\n";
}

void showMembers(const GymManagementSystem& system) {
    const auto& members = system.getMembers();
    if (members.empty()) {
        std::cout << "No members have been registered yet.\n";
        return;
    }
    std::cout << "\nID   Name                         Phone          Plan   Valid until   Status\n";
    std::cout << "----------------------------------------------------------------------------\n";
    for (const auto& member : members) {
        const MembershipPlan* plan = system.findPlan(member.getPlanId());
        std::cout << member.getId() << "    " << member.getName() << "    "
                  << member.getPhone() << "    "
                  << (plan ? plan->getName() : "(deleted plan)") << "    "
                  << member.getEndDate() << "    " << system.membershipStatus(member) << '\n';
    }
}

void manageMembers(GymManagementSystem& system) {
    while (true) {
        std::cout << "\n--- Module 1: Members ---\n"
                  << "1. Register a member\n2. List members\n3. Search by member ID\n"
                  << "4. Update member contact details\n5. Deactivate member\n0. Back\n";
        const int choice = readInt("Choose: ", 0, 5);
        if (choice == 0) return;
        if (choice == 1) {
            showPlans(system, true);
            bool hasActivePlan = false;
            for (const auto& plan : system.getPlans())
                if (plan.isActive()) hasActivePlan = true;
            if (!hasActivePlan) {
                std::cout << "Add an active plan before registering a member.\n";
                continue;
            }
            const std::string name = readLine("Full name: ");
            const std::string phone = readLine("Phone: ");
            const std::string email = readLine("Email: ");
            const int planId = readInt("Active plan ID: ", 1, 1000000000);
            std::string message;
            if (system.registerMember(name, phone, email, planId, message))
                std::cout << message << '\n';
            else
                std::cout << "Could not register member: " << message << '\n';
        } else if (choice == 2) {
            showMembers(system);
        } else if (choice == 3) {
            const int id = readInt("Member ID: ", 1, 1000000000);
            const Member* member = system.findMember(id);
            if (!member) {
                std::cout << "Member not found.\n";
            } else {
                const MembershipPlan* plan = system.findPlan(member->getPlanId());
                std::cout << "ID: " << member->getId() << "\nName: " << member->getName()
                          << "\nPhone: " << member->getPhone() << "\nEmail: " << member->getEmail()
                          << "\nPlan: " << (plan ? plan->getName() : "(deleted plan)")
                          << "\nStart: " << member->getStartDate() << "\nEnd: " << member->getEndDate()
                          << "\nStatus: " << system.membershipStatus(*member) << '\n';
            }
        } else if (choice == 4) {
            const int id = readInt("Member ID: ", 1, 1000000000);
            const std::string phone = readLine("New phone: ");
            const std::string email = readLine("New email: ");
            std::string message;
            system.updateMemberContact(id, phone, email, message);
            std::cout << message << '\n';
        } else if (choice == 5) {
            const int id = readInt("Member ID: ", 1, 1000000000);
            std::string message;
            system.deactivateMember(id, message);
            std::cout << message << '\n';
        }
    }
}

void managePlans(GymManagementSystem& system) {
    while (true) {
        std::cout << "\n--- Module 2: Membership plans ---\n"
                  << "1. Add a plan\n2. List plans\n3. Edit a plan\n4. Activate/deactivate a plan\n0. Back\n";
        const int choice = readInt("Choose: ", 0, 4);
        if (choice == 0) return;
        if (choice == 1) {
            const std::string name = readLine("Plan name: ");
            const int days = readInt("Duration in days (1-3650): ", 1, 3650);
            const double price = readMoney("Price: ");
            std::string message;
            system.addPlan(name, days, price, message);
            std::cout << message << '\n';
        } else if (choice == 2) {
            showPlans(system);
        } else if (choice == 3) {
            showPlans(system);
            const int id = readInt("Plan ID to edit: ", 1, 1000000000);
            const std::string name = readLine("New plan name: ");
            const int days = readInt("New duration in days (1-3650): ", 1, 3650);
            const double price = readMoney("New price: ");
            std::string message;
            system.editPlan(id, name, days, price, message);
            std::cout << message << '\n';
        } else if (choice == 4) {
            showPlans(system);
            const int id = readInt("Plan ID: ", 1, 1000000000);
            std::string message;
            system.togglePlan(id, message);
            std::cout << message << '\n';
        }
    }
}

void manageValidity(GymManagementSystem& system) {
    while (true) {
        std::cout << "\n--- Module 3: Validity and renewals ---\n"
                  << "1. Show membership validity\n2. Renew a membership\n0. Back\n";
        const int choice = readInt("Choose: ", 0, 2);
        if (choice == 0) return;
        if (choice == 1) {
            showMembers(system);
        } else {
            showPlans(system, true);
            const int memberId = readInt("Member ID to renew: ", 1, 1000000000);
            const int planId = readInt("Active plan ID for renewal: ", 1, 1000000000);
            std::string message;
            system.renewMembership(memberId, planId, message);
            std::cout << message << '\n';
        }
    }
}

void managePaymentsAndReports(GymManagementSystem& system) {
    while (true) {
        std::cout << "\n--- Module 4: Payments and reports ---\n"
                  << "1. Record an additional payment\n2. Show payment history\n3. Generate report file\n0. Back\n";
        const int choice = readInt("Choose: ", 0, 3);
        if (choice == 0) return;
        if (choice == 1) {
            const int memberId = readInt("Member ID: ", 1, 1000000000);
            const double amount = readMoney("Amount received: ");
            const std::string note = readLine("Payment note: ");
            std::string message;
            system.recordPayment(memberId, amount, note, message);
            std::cout << message << '\n';
        } else if (choice == 2) {
            const auto& payments = system.getPayments();
            if (payments.empty()) std::cout << "No payments recorded.\n";
            for (const auto& payment : payments) {
                const Member* member = system.findMember(payment.getMemberId());
                std::cout << "Payment #" << payment.getId() << " | " << payment.getDate()
                          << " | Member " << payment.getMemberId()
                          << (member ? " - " + member->getName() : "")
                          << " | " << payment.getAmount() << " | " << payment.getNote() << '\n';
            }
        } else if (choice == 3) {
            std::string message;
            if (system.generateReport(message)) std::cout << message << '\n';
            else std::cout << "Could not generate report: " << message << '\n';
        }
    }
}
} // namespace

int main() {
    GymManagementSystem system;
    std::string message;
    if (!system.loadData(message)) {
        std::cout << "Warning: " << message << '\n';
    }

    std::cout << "=====================================\n"
              << " Gym Membership Management System\n"
              << "=====================================\n";
    while (true) {
        std::cout << "\nMain menu\n1. Members\n2. Membership plans\n"
                  << "3. Validity and renewals\n4. Payments and reports\n0. Save and exit\n";
        const int choice = readInt("Choose: ", 0, 4);
        if (choice == 0) {
            if (system.saveData(message)) std::cout << "Data saved. Goodbye.\n";
            else std::cout << "Could not save all data: " << message << '\n';
            return 0;
        }
        if (choice == 1) manageMembers(system);
        else if (choice == 2) managePlans(system);
        else if (choice == 3) manageValidity(system);
        else if (choice == 4) managePaymentsAndReports(system);
    }
}
