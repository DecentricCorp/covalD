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

unsigned int static KimotoGravityWell(const CBlockIndex* pindexLast, const CBlockHeader *pblock, uint64_t TargetBlocksSpacingSeconds, uint64_t PastBlocksMin, uint64_t PastBlocksMax) {
        /* current difficulty formula, megacoin - kimoto gravity well */
        const CBlockIndex  *BlockLastSolved = pindexLast;
        const CBlockIndex  *BlockReading    = pindexLast;
        const CBlockHeader *BlockCreating   = pblock;
                                             BlockCreating         = BlockCreating;
        uint64_t                               PastBlocksMass          = 0;
        int64_t                                PastRateActualSeconds    = 0;
        int64_t                                PastRateTargetSeconds    = 0;
        double                               PastRateAdjustmentRatio = double(1);
        uint256                              PastDifficultyAverage;
        uint256                              PastDifficultyAveragePrev;
        double                               EventHorizonDeviation;
        double                               EventHorizonDeviationFast;
        double                               EventHorizonDeviationSlow;
        
    if (BlockLastSolved == NULL || BlockLastSolved->nHeight == 0 || (uint64_t)BlockLastSolved->nHeight < PastBlocksMin) { return Params().ProofOfWorkLimit().GetCompact(); }
	
        int64_t LatestBlockTime = BlockLastSolved->GetBlockTime();
		
        for (unsigned int i = 1; BlockReading && BlockReading->nHeight > 0; i++) {
                if (PastBlocksMax > 0 && i > PastBlocksMax) { break; }
                PastBlocksMass++;
                
                if (i == 1)        { PastDifficultyAverage.SetCompact(BlockReading->nBits); }
                else                { PastDifficultyAverage = ((uint256().SetCompact(BlockReading->nBits) - PastDifficultyAveragePrev) / i) + PastDifficultyAveragePrev; }
                PastDifficultyAveragePrev = PastDifficultyAverage;
				
				if (LatestBlockTime < BlockReading->GetBlockTime()) {
					//eliminates the ability to go back in time
					LatestBlockTime = BlockReading->GetBlockTime();
				}
                
                PastRateActualSeconds                        = BlockLastSolved->GetBlockTime() - BlockReading->GetBlockTime();
                PastRateTargetSeconds                        = TargetBlocksSpacingSeconds * PastBlocksMass;
                PastRateAdjustmentRatio                        = double(1);
				
				if (PastRateActualSeconds < 5) { PastRateActualSeconds = 5; }


				
                if (PastRateActualSeconds != 0 && PastRateTargetSeconds != 0) {
                PastRateAdjustmentRatio                        = double(PastRateTargetSeconds) / double(PastRateActualSeconds);
                }
                EventHorizonDeviation                        = 1 + (0.7084 * pow((double(PastBlocksMass)/double(144)), -1.228));
                EventHorizonDeviationFast                = EventHorizonDeviation;
                EventHorizonDeviationSlow                = 1 / EventHorizonDeviation;
                
                if (PastBlocksMass >= PastBlocksMin) {
                        if ((PastRateAdjustmentRatio <= EventHorizonDeviationSlow) || (PastRateAdjustmentRatio >= EventHorizonDeviationFast)) { assert(BlockReading); break; }
                }
                if (BlockReading->pprev == NULL) { assert(BlockReading); break; }
                BlockReading = BlockReading->pprev;
        }
        
        uint256 bnNew(PastDifficultyAverage);
        if (PastRateActualSeconds != 0 && PastRateTargetSeconds != 0) {
                bnNew *= PastRateActualSeconds;
                bnNew /= PastRateTargetSeconds;
        }
    if (bnNew > Params().ProofOfWorkLimit()) { bnNew = Params().ProofOfWorkLimit(); }
        
    /// debug print
    printf("Difficulty Retarget - Kimoto Gravity Well\n");
    printf("PastRateAdjustmentRatio = %g\n", PastRateAdjustmentRatio);
    printf("Before: %08x  %s\n", BlockLastSolved->nBits, uint256().SetCompact(BlockLastSolved->nBits).ToString().c_str());
    printf("After:  %08x  %s\n", bnNew.GetCompact(), bnNew.getuint256().ToString().c_str());
        
        return bnNew.GetCompact();
}

unsigned int static GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock)
{
        static const int64_t        BlocksTargetSpacing                        = 60; // 60 seconds
        unsigned int                TimeDaySeconds                                = 60 * 60 * 24;
        int64_t                       PastSecondsMin                                = TimeDaySeconds * 0.25;
        int64_t                       PastSecondsMax                                = TimeDaySeconds * 1;
        uint64_t                      PastBlocksMin                                = PastSecondsMin / BlocksTargetSpacing;
        uint64_t                      PastBlocksMax                                = PastSecondsMax / BlocksTargetSpacing;        
        
        return KimotoGravityWell(pindexLast, pblock, BlocksTargetSpacing, PastBlocksMin, PastBlocksMax);
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
