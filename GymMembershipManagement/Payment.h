#ifndef PAYMENT_H
#define PAYMENT_H

#include <string>

class Payment {
public:
    Payment(int id, int memberId, double amount, std::string date, std::string note);

    int getId() const;
    int getMemberId() const;
    double getAmount() const;
    const std::string& getDate() const;
    const std::string& getNote() const;

private:
    int id;
    int memberId;
    double amount;
    std::string date;
    std::string note;
};

#endif
