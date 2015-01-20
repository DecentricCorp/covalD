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

unsigned int KimotoGravityWell(const CBlockIndex* pindexLast, int algo) {
	
	unsigned int nProofOfWorkLimit = Params().ProofOfWorkLimit(algo).GetCompact();
    LogPrintf("Proof Of Work Limit For Algo %i, is % i\n", algo, nProofOfWorkLimit);

    // Genesis block
    if (pindexLast == NULL){
		LogPrintf("Genesis Block Difficulty");
        return nProofOfWorkLimit;
	}
	const CBlockIndex* pindexPrevAlgo = GetLastBlockIndexForAlgo(pindexLast, algo);
    if (pindexPrevAlgo == NULL){
		LogPrintf("pindexPrevAlgo == NULL for Algo %i, is % i\n", algo, nProofOfWorkLimit);
        return nProofOfWorkLimit;
	}
    /* Franko Multi Algo Gravity Well */
    const CBlockIndex   *BlockLastSolved                 = pindexPrevAlgo;
    const CBlockIndex   *BlockReading                    = pindexPrevAlgo;

	int					 AlgoCounter					 = 0;
    uint64_t             PastBlocksMass                  = 0;
    int64_t              PastRateActualSeconds           = 0;
    int64_t              PastRateTargetSeconds           = 0;
    double               PastRateAdjustmentRatio         = double(1);
    uint256              PastDifficultyAverage;
    uint256              PastDifficultyAveragePrev;
    uint256              BlockReadingDifficulty;
    double               EventHorizonDeviation;
    double               EventHorizonDeviationFast;
    double               EventHorizonDeviationSlow;

    static const int64_t TargetBlockSpacing              = 60; // == 1 minute
    unsigned int         TimeDaySeconds                  = 60 * 60 * 24;
    int64_t              PastSecondsMin                  = TimeDaySeconds * 0.25; // == 6300 Seconds
    int64_t              PastSecondsMax                  = TimeDaySeconds * 7; // == 604800 Seconds
    uint64_t             PastBlocksMin                   = PastSecondsMin / TargetBlockSpacing; // == 360 blocks
    uint64_t             PastBlocksMax                   = PastSecondsMax / TargetBlockSpacing; // == 10080 blocks

	//loop through and count the blocks found by the algo
	for (unsigned int i = 1; BlockReading && BlockReading->nHeight > 0; i++) {
        if (PastBlocksMax > 0 && i > PastBlocksMax) { break; }
		// Makes sure we are only calculating blocks from the specified algo
		if (BlockReading->GetAlgo() != algo){ continue; }
		AlgoCounter++;
		BlockReading = BlockReading->pprev;
	}

    if (BlockLastSolved == NULL || BlockLastSolved->nHeight == 0 ||
        (uint64_t)BlockLastSolved->nHeight < PastBlocksMin ||
			AlgoCounter < PastBlockMin) {
        return Params().ProofOfWorkLimit(algo).GetCompact();
    }
    
    int64_t LatestBlockTime = BlockLastSolved->GetBlockTime();


    BlockReading = pindexPrevAlgo;
    for (unsigned int i = 1; BlockReading && BlockReading->nHeight > 0; i++) {
        if (PastBlocksMax > 0 && i > AlgoCounter) { break; }
		// Makes sure we are only calculating blocks from the specified algo
		if (BlockReading->GetAlgo() != algo){ continue; }

        PastBlocksMass++;

        if (i == 1) {
            PastDifficultyAverage.SetCompact(BlockReading->nBits);
        } else {
            BlockReadingDifficulty.SetCompact(BlockReading->nBits);
            if (BlockReadingDifficulty > PastDifficultyAveragePrev) {
                PastDifficultyAverage = PastDifficultyAveragePrev + ((BlockReadingDifficulty - PastDifficultyAveragePrev) / i);
            } else {
                PastDifficultyAverage = PastDifficultyAveragePrev - ((PastDifficultyAveragePrev - BlockReadingDifficulty) / i);
            }
        }
        PastDifficultyAveragePrev = PastDifficultyAverage;

        if (LatestBlockTime < BlockReading->GetBlockTime()) {
                LatestBlockTime = BlockReading->GetBlockTime();
        }
        PastRateActualSeconds = LatestBlockTime - BlockReading->GetBlockTime();
        PastRateTargetSeconds = TargetBlockSpacing * PastBlocksMass;
        PastRateAdjustmentRatio = double(1);
        
        if (PastRateActualSeconds < 1) { PastRateActualSeconds = 1; }
        
        if (PastRateActualSeconds != 0 && PastRateTargetSeconds != 0) {
            PastRateAdjustmentRatio = double(PastRateTargetSeconds) / double(PastRateActualSeconds);
        }
        EventHorizonDeviation                   = 1 + (0.7084 * pow((double(PastBlocksMass)/double(144)), -1.228));
        EventHorizonDeviationFast               = EventHorizonDeviation;
        EventHorizonDeviationSlow               = 1 / EventHorizonDeviation;

        if (PastBlocksMass >= PastBlocksMin) {
            if ((PastRateAdjustmentRatio <= EventHorizonDeviationSlow) ||
                (PastRateAdjustmentRatio >= EventHorizonDeviationFast)) {
                assert(BlockReading);
                break;
            }
        }
        if (BlockReading->pprev == NULL) { assert(BlockReading); break; }
        BlockReading = BlockReading->pprev;
    }

    uint256 bnNew(PastDifficultyAverage);
    if (PastRateActualSeconds != 0 && PastRateTargetSeconds != 0) {
        bnNew *= PastRateActualSeconds;
        bnNew /= PastRateTargetSeconds;
    }
    if (bnNew > Params().ProofOfWorkLimit(algo)) { bnNew = Params().ProofOfWorkLimit(algo); }

    // debug print

        LogPrintf("Franko Multi Algo Gravity Well\n");
    
    LogPrintf("PastRateAdjustmentRatio =  %g    PastRateTargetSeconds = %d    PastRateActualSeconds = %d\n",
               PastRateAdjustmentRatio, PastRateTargetSeconds, PastRateActualSeconds);
    LogPrintf("Before: %08x  %s\n", BlockLastSolved->nBits, uint256().SetCompact(BlockLastSolved->nBits).ToString());
    LogPrintf("After:  %08x  %s\n", bnNew.GetCompact(), bnNew.ToString());

    return bnNew.GetCompact();
}

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, int algo)
{
      return KimotoGravityWell(pindexLast, algo);
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
