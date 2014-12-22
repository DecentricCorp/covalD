#include "amount.h"
#include "main.h"

//extern BlockMap mapBlockIndex;
//extern CChain chainActive;

class CInterest
{
    public:
        static CAmount ComputeInterest(int periods, const CTxOut& txOut)
        {
            // do blockchain lookup here

            double APY = 0.05;
            double rate = exp(log(1+APY)/525600) - 1;
            CAmount interest = txOut.nValue * pow(1.0+rate, periods) - txOut.nValue;
            return interest;
        }

///};

};
