#include "Payment.h"

#include <utility>

Payment::Payment(int id, int memberId, double amount, std::string date, std::string note)
    : id(id), memberId(memberId), amount(amount), date(std::move(date)), note(std::move(note)) {}

int Payment::getId() const { return id; }
int Payment::getMemberId() const { return memberId; }
double Payment::getAmount() const { return amount; }
const std::string& Payment::getDate() const { return date; }
const std::string& Payment::getNote() const { return note; }
