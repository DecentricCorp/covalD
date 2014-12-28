#include <interest.h>
#include "util.h"
#include "main.h" 

CAmount ComputeInterest(int periods, const CTxOut& txOut)
{
   // do blockchain lookup here
   double APY = 0.05;
   double rate = exp(log(1+APY)/525600) - 1;
   CAmount interest = txOut.nValue * pow(1.0+rate, periods) - txOut.nValue;
   return interest;
}


        CAmount ComputeInterest(const uint256& txHash, const CTxOut& txOut) 
        {
            CTransaction transaction;
            uint256 hashBlock;
            if (!GetTransaction(txHash, transaction, hashBlock, true)) {
               // throw error
            }

            CBlockIndex* spendBlockIndex = mapBlockIndex[hashBlock];
            if(spendBlockIndex == NULL) {
                // throw error
            }
            int periods = chainActive.Height() - spendBlockIndex->nHeight + 1;
            return ComputeInterest(periods, txOut);
        }
