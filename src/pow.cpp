// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <map>

#include "pow.h"
#include "chain.h"

#include "chain.h"
#include "chainparams.h"
#include "checkpoints.h"
#include "primitives/block.h"
#include "uint256.h"
#include "util.h"

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, 
        const CBlockHeader *pblock)
{
    int algo = pblock->GetAlgo();
    unsigned int nProofOfWorkLimit = Params().ProofOfWorkLimit(algo).GetCompact();
    const Checkpoints::MapCheckpoints& cp = *Params().Checkpoints().mapCheckpoints;
    int minheight = 0;
    for(std::map<int, uint256>::const_iterator it=cp.begin(); it != cp.end(); it++) 
        if(it->first > minheight) minheight = it->first; 

    // Genesis block
    if (pindexLast == NULL || pindexLast->nHeight <= minheight) 
        return nProofOfWorkLimit;

    // If we're within one averaging interval, just return the work of the
    // previous block using the same algorithm.
    if ((pindexLast->nHeight+1) % Params().Interval() != 0)
    {
        const CBlockIndex* pindex = GetLastBlockIndex(pindexLast, algo);
        if(pindex == NULL || pindex->nHeight <= minheight)
            return nProofOfWorkLimit;
        if (Params().AllowMinDifficultyBlocks())
        {
            // Special difficulty rule for testnet:
            // If the new block's timestamp is more than 2* 10 minutes
            // then allow mining of a min-difficulty block.
            if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + Params().TargetSpacing()*2*NUM_ALGOS)
                return nProofOfWorkLimit;
            else
            {
                // Return the last non-special-min-difficulty-rules-block
                while (pindex && pindex->pprev && pindex->nHeight % Params().Interval() != 0 
                        && pindex->nBits == nProofOfWorkLimit)
                    pindex = GetLastBlockIndex(pindex->pprev, algo);
                if(pindex == NULL || pindex->nHeight <= minheight)
                    return nProofOfWorkLimit;
                return pindex->nBits;
            }
        }
        return pindex->nBits;
    }

    const CBlockIndex* pindexFirst = NULL;
    pindexFirst = pindexLast;
    for (int i = 0; pindexFirst && i < Params().Interval()- 1; i++)
    {
        pindexFirst = pindexFirst->pprev;
        pindexFirst = GetLastBlockIndex(pindexFirst, algo);
        if (pindexFirst == NULL || pindexFirst->nHeight <= minheight)
            return nProofOfWorkLimit; // not nAveragingInterval blocks of this algo available
    }

    int64_t nActualTimespan = pindexLast->GetBlockTime() - pindexFirst->GetBlockTime();

    // Retarget
    uint256 bnNew;
    uint256 bnOld;
    bnNew.SetCompact(pindexLast->nBits);
    bnOld = bnNew;
    bnNew *= nActualTimespan/NUM_ALGOS;
    bnNew /= Params().TargetTimespan();
    // FIXME should I use the median block time over an interval instead of the average?
    // FIXME evaluate derivative and implement damping function
    // FIXME Params().fDampingConstant should be roughly the half-life of decay
    //       after an impulse
    // FIXME make a vector of block times.
    // FIXME make a vector of the derivatives by finite difference.

    if (bnNew > Params().ProofOfWorkLimit(algo))
        bnNew = Params().ProofOfWorkLimit(algo);

    /// debug print
    LogPrintf("GetNextWorkRequired RETARGET for algo %s\n", GetAlgoName(algo));
    LogPrintf("Params().TargetTimespan() = %d    nActualTimespan = %d\n", Params().TargetTimespan(), nActualTimespan);
    LogPrintf("Before: %08x  %s\n", pindexLast->nBits, bnOld.ToString());
    LogPrintf("After:  %08x  %s\n", bnNew.GetCompact(), bnNew.ToString());

    return bnNew.GetCompact();
}

// FIXME algo is only needed here for a minimum work block.
// if all algos are treated the same with the same minimum limit...
bool CheckProofOfWork(uint256 hash, unsigned int nBits, int algo)
{
    bool fNegative;
    bool fOverflow;
    uint256 bnTarget;

    if (Params().SkipProofOfWorkCheck() || hash == Params().HashAirdropBlock())
       return true;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > Params().ProofOfWorkLimit(algo)) {
        LogPrintf("CheckProofOfWork: \n");
        LogPrintf("                           hash = %s\n", hash.GetHex());
        LogPrintf("                        bnTaget = %s\n", bnTarget.GetHex());
        LogPrintf("  Params().ProofOfWorkLimit(%s) = %s\n", GetAlgoName(algo), Params().ProofOfWorkLimit(algo).GetHex());
        return error("CheckProofOfWork(algo=%d) : nBits below minimum work", algo);
    }

    // Check proof of work matches claimed amount
    if (hash > bnTarget)
        return error("CheckProofOfWork(algo=%s) : hash doesn't match nBits", GetAlgoName(algo));

    return true;
}

uint256 GetBlockProof(const CBlockIndex& block)
{
    uint256 bnTarget;
    bool fNegative;
    bool fOverflow;
    bnTarget.SetCompact(block.nBits, &fNegative, &fOverflow);
    if (fNegative || fOverflow || bnTarget == 0)
        return 0;
    // We need to compute 2**256 / (bnTarget+1), but we can't represent 2**256
    // as it's too large for a uint256. However, as 2**256 is at least as large
    // as bnTarget+1, it is equal to ((2**256 - bnTarget - 1) / (bnTarget+1)) + 1,
    // or ~bnTarget / (nTarget+1) + 1.
    return (~bnTarget / (bnTarget + 1)) + 1;
}
