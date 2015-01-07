#include <cfloat>
#include "main.h"
#include "interest.h"
#include "util.h"

double InterestCompoundingRate()
{
    //    double rate = exp(log( (1 + Params().InterestAPY() / 100 ) / 525600)) - 1;r
    double APY = Params().InterestAPY()/100;
    double n = Params().CompoundingPeriods();

    double rate = APY/n - (n-1)/(2*pow(n,2))*pow(APY, 2) + (2*pow(n, 2)-3*n+1)/(6*pow(n, 3))*pow(APY, 3);

    return rate;
}

CAmount ComputeInterest(int n, const CTxOut& txOut)
{
    double r = InterestCompoundingRate();

    // this is txOut.nValue*((1+r)^n - 1) but with more precision
    // the computation was for satoshi
    // freeg is 10^8
    double nFreeg = txOut.nValue / 100000000;
    // converted to satoshi 10^6
    int nValue = nFreeg * 1000000;

    CAmount interest = nValue*expm1(n*log1p(r));
    //std::cout << float(interest) << std::endl;

    return interest;
}
