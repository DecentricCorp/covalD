// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_POW_H
#define BITCOIN_POW_H

#include <stdint.h>
#include <string>
#include <cmath>


#include "utilstrencodings.h"
#include "bignum.h"

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

#define BLOCK_VERSION_MASK         0x000000ff // fix on main.cpp line 1949
#define BLOCK_VERSION(nVersion)         (nVersion & BLOCK_VERSION_MASK)
#define BLOCK_VERSION_AUXPOW_MASK  0x00000100 // 1 bit   // called, confusingly, BLOCK_VERSION_AUXPOW
#define BLOCK_VERSION_AUXPOW(nVersion)  ((nVersion & BLOCK_VERSION_AUXPOW_MASK) >> 8)
#define BLOCK_VERSION_ALGO_MASK    0x00000e00 // 7 bits  
#define BLOCK_VERSION_ALGO_START_BIT   9
#define BLOCK_VERSION_ALGO(nVersion)    ((nVersion & BLOCK_VERSION_ALGO_MASK) >> 9)
#define BLOCK_VERSION_UNUSED_MASK  0x1000f000 // 5 unused bits -- could make 256 algos
#define BLOCK_VERSION_CHAINID_MASK 0xefff0000 // bits 16-30
#define BLOCK_VERSION_CHAINID(nVersion) ((nVersion & BLOCK_VERSION_CHAINID_MASK) >> 16)
// high (sign) bit unused for the sake of bastards that confuse signed and unsigned.

inline int GetAlgo(int nVersion)
{
    return BLOCK_VERSION_ALGO(nVersion);
}

inline void SetAlgo(int& nVersion, int algo)
{
    nVersion |= (algo << BLOCK_VERSION_ALGO_START_BIT);
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
        const CBlockHeader *pblock, int algo);

/** Check whether a block hash satisfies the proof-of-work requirement specified by nBits */
bool CheckProofOfWork(uint256 hash, unsigned int nBits, int algo);
uint256 GetBlockProof(const CBlockIndex& block);

#endif // BITCOIN_POW_H
