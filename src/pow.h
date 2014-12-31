// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_POW_H
#define BITCOIN_POW_H

#include <stdint.h>
#include <string>

class CBlockHeader;
class CBlockIndex;
class uint256;

enum {
    ALGO_SHA256D = 0,
    ALGO_SCRYPT  = 1,
//    ALGO_GROESTL = 2,
//    ALGO_SKEIN   = 3,
//    ALGO_QUBIT   = 4,
    NUM_ALGOS };

enum // FIXME merge this with auxpow fields
{
    // primary version
    BLOCK_VERSION_DEFAULT        = 2,

    // algo
    BLOCK_VERSION_ALGO           = (7 << 9),
    BLOCK_VERSION_SCRYPT         = (1 << 9),
//    BLOCK_VERSION_GROESTL        = (2 << 9),
//    BLOCK_VERSION_SKEIN          = (3 << 9),
//    BLOCK_VERSION_QUBIT          = (4 << 9),
};

inline int GetAlgo(int nVersion)
{
    switch (nVersion & BLOCK_VERSION_ALGO)
    {
        case 0:
            return ALGO_SHA256D;
        case BLOCK_VERSION_SCRYPT:
            return ALGO_SCRYPT;
//        case BLOCK_VERSION_GROESTL:
//            return ALGO_GROESTL;
//        case BLOCK_VERSION_SKEIN:
//            return ALGO_SKEIN;
//        case BLOCK_VERSION_QUBIT:
//            return ALGO_QUBIT;
    }
    return ALGO_SHA256D;
}

inline std::string GetAlgoName(int Algo)
{
    switch (Algo)
    {
        case ALGO_SHA256D:
            return std::string("sha256d");
        case ALGO_SCRYPT:
            return std::string("scrypt");
//        case ALGO_GROESTL:
//            return std::string("groestl");
//        case ALGO_SKEIN:
//            return std::string("skein");
//        case ALGO_QUBIT:
//            return std::string("qubit");
    }
    return std::string("unknown");
}

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, 
        const CBlockHeader *pblock);

/** Check whether a block hash satisfies the proof-of-work requirement specified by nBits */
bool CheckProofOfWork(uint256 hash, unsigned int nBits, int algo);
uint256 GetBlockProof(const CBlockIndex& block);

#endif // BITCOIN_POW_H
