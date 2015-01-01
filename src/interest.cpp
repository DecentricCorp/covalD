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
//    CAmount interest = r*n + ((n * (n-1)) /2) * pow(r,2) + ((n*(n-1)*(n-2))/6)*pow(r, 3)+ ((n*(n-1)*(n-2)*(n-3))/24)*pow(r,4);
         CAmount interest = txOut.nValue * (pow(1.0+r, n) - 1);
    return interest;
}


/*CAmount ComputeInterest(const uint256& txHash, const CTxOut& txOut) 
{
//    error("Called with txHash - %s", txHash.getHex());
    CTransaction transaction;
    uint256 hashBlock;

    if (!GetTransaction(txHash, transaction, hashBlock, true)) {
       CAmount none = 0;
       return none;
       // throw error - how does CheckInputs and some of the Wallet functions throw errors?
    }

    CBlockIndex* spendBlockIndex = mapBlockIndex[hashBlock];
    if (spendBlockIndex == NULL) {
       CAmount none = 0;
       return none;
       // throw error
    }

    int periods = chainActive.Height() - spendBlockIndex->nHeight + 1;
    return ComputeInterest(periods, txOut);
}
*/
