#include "amount.h"

double InterestRate();
double InterestCompoundingRate();
CAmount ComputeInterest(int periods, const CTxOut& txOut);
//#CAmount ComputeInterest(const uint256& periods, const CTxOut& txOut);
