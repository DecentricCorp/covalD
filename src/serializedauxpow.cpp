#include "serializedauxpow.h"

#include <assert.h>


CSerializedBlockHeader::CSerializedBlockHeader()
{
	SetNull();
}


void CSerializedBlockHeader::SetNull()
{
	nVersion	= 0;
	hashPrevBlock	= 0;
	hashMerkleRoot	= 0;
	nTime		= 0;
	nBits		= 0;
	nNonce		= 0;
}


CSerializedAuxPow::CSerializedAuxPow()
{
	SetNull();
}


void CSerializedAuxPow::SetNull()
{
	// Surprisingly, there is no CTransaction::SetNull()
	parentCoinbaseTxn = CTransaction();
	assert(parentCoinbaseTxn.IsNull());
	
	parentBlockHeaderHash = 0;
	
	vCoinbaseBranch.clear();
	nCoinbaseSideMask = 0;
	
	vBlockchainBranch.clear();
	nBlockchainSideMask = 0;
	
	parentBlockHeader.SetNull();
}


