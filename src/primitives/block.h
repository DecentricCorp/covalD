// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_PRIMITIVES_BLOCK_H
#define BITCOIN_PRIMITIVES_BLOCK_H

#include "primitives/transaction.h"
#include "serialize.h"
#include "uint256.h"
#include "pow.h"
#include "crypto/scrypt.h"
#include <boost/shared_ptr.hpp>

class CAuxPow;

template <typename Stream>
void ReadWriteAuxPow(Stream& s, const boost::shared_ptr<CAuxPow>& auxpow, int nType, int nVersion, CSerActionSerialize ser_action);

template <typename Stream>
void ReadWriteAuxPow(Stream& s, boost::shared_ptr<CAuxPow>& auxpow, int nType, int nVersion, CSerActionUnserialize ser_action);

//template <typename Stream>
//int ReadWriteAuxPow(Stream& s, const boost::shared_ptr<CAuxPow>& auxpow, int nType, int nVersion, CSerActionGetSerializeSize ser_action);

// primary version
//static const int BLOCK_VERSION_DEFAULT = (1 << 0);
static const int BLOCK_VERSION_AUXPOW = (1 << 8);
static const int BLOCK_VERSION_CHAIN_START = (1 << 16);
static const int BLOCK_VERSION_CHAIN_END = (1 << 30);

// Unitus aux chain ID = 0x007D (155)
static const int AUXPOW_CHAIN_ID = 0x007D;
static const int AUXPOW_START_MAINNET = 1; //TODO change me
static const int AUXPOW_START_TESTNET = 1;


/** The maximum allowed size for a serialized block, in bytes (network rule) */
static const unsigned int MAX_BLOCK_SIZE = 1000000;

/** Nodes collect new transactions into a block, hash them into a hash tree,
 * and scan through nonce values to make the block's hash satisfy proof-of-work
 * requirements.  When they solve the proof-of-work, they broadcast the block
 * to everyone and the block is added to the block chain.  The first transaction
 * in the block is a special one that creates a new coin owned by the creator
 * of the block.
 */
class CBlockHeader
{
public:
    // header
    static const int32_t CURRENT_VERSION=3;
    int32_t nVersion;
    uint256 hashPrevBlock;
    uint256 hashMerkleRoot;
    uint32_t nTime;
    uint32_t nBits;
    uint32_t nNonce;
	boost::shared_ptr<CAuxPow> auxpow;

    CBlockHeader()
    {
        SetNull();
    }

    int GetAlgo() const { return ::GetAlgo(nVersion); }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(hashPrevBlock);
        READWRITE(hashMerkleRoot);
        READWRITE(nTime);
        READWRITE(nBits);
        READWRITE(nNonce);
        ReadWriteAuxPow(s, auxpow, nType, nVersion, ser_action);
    }

	void SetAuxPow(CAuxPow* pow);

    void SetNull()
    {
		nVersion = CBlockHeader::CURRENT_VERSION | (AUXPOW_CHAIN_ID * BLOCK_VERSION_CHAIN_START);
		hashPrevBlock = 0;
        hashMerkleRoot = 0;
        nTime = 0;
        nBits = 0;
        nNonce = 0;
    }

    int GetChainID() const
    {
        return nVersion / BLOCK_VERSION_CHAIN_START;
    }


    bool IsNull() const
    {
        return (nBits == 0);
    }

    // Note: we use explicitly provided algo instead of the one returned by GetAlgo(), because this can be a block
    // from foreign chain (parent block in merged mining) which does not encode algo in its nVersion field.
    uint256 GetHash(int algo=-1) const;

    int64_t GetBlockTime() const
    {
        return (int64_t)nTime;
    }
};


class CBlock : public CBlockHeader
{
public:
    // network and disk
    std::vector<CTransaction> vtx;

    // memory only
    mutable std::vector<uint256> vMerkleTree;

    CBlock()
    {
        SetNull();
    }

    CBlock(const CBlockHeader &header)
    {
        SetNull();
        *((CBlockHeader*)this) = header;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(*(CBlockHeader*)this);
        READWRITE(vtx);
    }

    void SetNull()
    {
        CBlockHeader::SetNull();
        vtx.clear();
        vMerkleTree.clear();
    }

    CBlockHeader GetBlockHeader() const
    {
        CBlockHeader block;
        block.nVersion       = nVersion;
        block.hashPrevBlock  = hashPrevBlock;
        block.hashMerkleRoot = hashMerkleRoot;
        block.nTime          = nTime;
        block.nBits          = nBits;
        block.nNonce         = nNonce;
        block.auxpow         = auxpow;
        return block;
    }

    // Build the in-memory merkle tree for this block and return the merkle root.
    // If non-NULL, *mutated is set to whether mutation was detected in the merkle
    // tree (a duplication of transactions in the block leading to an identical
    // merkle root).
    uint256 BuildMerkleTree(bool* mutated = NULL) const;

    std::vector<uint256> GetMerkleBranch(int nIndex) const;
    static uint256 CheckMerkleBranch(uint256 hash, const std::vector<uint256>& vMerkleBranch, int nIndex);
    std::string ToString() const;
};


/** Describes a place in the block chain to another node such that if the
 * other node doesn't have the same branch, it can find a recent common trunk.
 * The further back it is, the further before the fork it may be.
 */
struct CBlockLocator
{
    std::vector<uint256> vHave;

    CBlockLocator() {}

    CBlockLocator(const std::vector<uint256>& vHaveIn)
    {
        vHave = vHaveIn;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);
        READWRITE(vHave);
    }

    void SetNull()
    {
        vHave.clear();
    }

    bool IsNull()
    {
        return vHave.empty();
    }
};

#endif // BITCOIN_PRIMITIVES_BLOCK_H
