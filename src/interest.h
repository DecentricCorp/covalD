#include "amount.h"
#include "main.h"

//extern CBlockIndex mapBlockIndex;
//extern CChain chainActive;

CAmount ComputeInterest(int periods, const CTxOut& txOut);
CAmount ComputeInterest(const uint256& periods, const CTxOut txOut);
/*
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

        static CAmount ComputeInterest(uint256 txHash, const CTxOut& txOut)
        {
            CTransaction transaction;
            uint256 hashBlock;
            if (!GetTransaction(txHash, transaction, hashBlock)) {
               // throw error
            }

            CBlockIndex* spendBlockIndex = mapBlockIndex[hashBlock];
            int periods = chainActive.Height() - spendBlockIndex->nHeight + 1;
            return CInterest::ComputeInterest(periods, txOut);
        }

///};

};
*/
