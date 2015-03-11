// Copyright (c) 2009-2010 Satoshi Nakamoto
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_AUXPOW_H
#define BITCOIN_AUXPOW_H

#include "main.h"
#include "wallet.h"

#include "serializedauxpow.h"


class CAuxPow : public CMerkleTx
{
public:
    CAuxPow(const CTransaction& txIn) : CMerkleTx(txIn)
    {
    }

    CAuxPow() :CMerkleTx()
    {
    }

    // Josh: Conversions between CAuxPow and CSerializedAuxPow
    CAuxPow(const CSerializedAuxPow& serAuxPow);
    CSerializedAuxPow GetSerialized() const;
    
    // Merkle branch with root vchAux
    // root must be present inside the coinbase
    std::vector<uint256> vChainMerkleBranch;
    // Index of chain in chains merkle tree
    unsigned int nChainIndex;
    CBlockHeader parentBlockHeader;

    // Josh: This class is no longer serializable!
    // Use CSerializedAuxPow instead, for that.

    bool Check(uint256 hashAuxBlock, int nChainID);

    uint256 GetParentBlockHash(int algo)
    {
        return parentBlockHeader.GetHash(algo);
    }
};


extern void RemoveMergedMiningHeader(std::vector<unsigned char>& vchAux);
extern CScript MakeCoinbaseWithAux(unsigned int nBits, unsigned int nExtraNonce, std::vector<unsigned char>& vchAux);

#endif
