#ifndef BITCOIN_SERIALIZEDAUXPOW_H
#define BITCOIN_SERIALIZEDAUXPOW_H


// https://en.bitcoin.it/wiki/Merged_mining_specification
//
// Layout of an AuxPOW block on the wire:
// (the classic block header precedes)
// txn            coinbase txn (in parent blockchain! must contain linkage in arbitrary data area)
// u256           parent blockchain's block header hash
// merkle_branch  coinbase branch (coinbase_txn to parent block's hashMerkleRoot)
// merkle_branch  blockchain branch (this auxpow to others, for merging multiple coins)
// 80 bytes       parent blockchain's block header (the classic block header)
// (the transactions follow)
//
// Layout of a merkle_branch:
// var_int              branch_length (count of hashes that follow)
// u256[branch_length]  branch_hash
// u32                  branch_side_mask (bitfield)
//
// Class inheritance:
// CAuxPow <-- CMerkleTx <-- CTransaction
//
// How this maps to what we have here:
// CAuxPow:
//    CMerkleTx:
//        CTransaction:
//            see elsewhere, this is unchanged from classic Bitcoin = coinbase txn in parent
//        hashBlock     = parent blockchain's block header hash
//        vMerkleBranch = coinbase branch (includes count)
//        nIndex        = coinbase branch's branch_side_mask (must be zero?)
//    vChainMerkleBranch = blockchain branch (includes count)
//    nChainIndex        = blockchain branch's branch_side_mask
//    parentBlockHeader  = parent blockchain's block header
//
// This class CSerializedAuxPow is equivalent to CAuxPow
// but has no other depdendencies,
// so it solves the circular dependency problem.


#include "primitives/transaction.h"
#include "uint256.h"


// Names and meanings are identical to the real CBlockHeader
// in primitives/block.h which is intentionally not included here
// because of circular dependency problems.
class CSerializedBlockHeader
{
public:
	int32_t		nVersion;
	uint256		hashPrevBlock;
	uint256		hashMerkleRoot;
	uint32_t	nTime;
	uint32_t	nBits;
	uint32_t	nNonce;
	
public:
	CSerializedBlockHeader();
	
	void SetNull();

public:	
	ADD_SERIALIZE_METHODS;

	template <typename Stream, typename Operation>
	inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
	{
		// We want to serialize the nVersion stored within this object,
		// not the nVersion of the caller,
		// since this block header might be from another blockchain (not ours)!
		READWRITE(this->nVersion);
		READWRITE(hashPrevBlock);
		READWRITE(hashMerkleRoot);
		READWRITE(nTime);
		READWRITE(nBits);
		READWRITE(nNonce);
	}
};


class CSerializedAuxPow
{
public:
	CTransaction		parentCoinbaseTxn;
	uint256			parentBlockHeaderHash;
	std::vector<uint256>	vCoinbaseBranch;
	uint32_t		nCoinbaseSideMask;
	std::vector<uint256>	vBlockchainBranch;
	uint32_t		nBlockchainSideMask;
	CSerializedBlockHeader	parentBlockHeader;

public:
	CSerializedAuxPow();
	
	void SetNull();

public:
	ADD_SERIALIZE_METHODS;

	template <typename Stream, typename Operation>
	inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
	{
		READWRITE(parentCoinbaseTxn);
		READWRITE(parentBlockHeaderHash);
		READWRITE(vCoinbaseBranch);
		READWRITE(nCoinbaseSideMask);
		READWRITE(vBlockchainBranch);
		READWRITE(nBlockchainSideMask);
		READWRITE(parentBlockHeader);
	}
};


#endif
