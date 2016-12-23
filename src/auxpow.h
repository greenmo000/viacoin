// Copyright (c) 2009-2010 Satoshi Nakamoto
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_AUXPOW_H
#define BITCOIN_AUXPOW_H

#include "consensus/params.h"
#include "wallet/wallet.h"
#include "primitives/block.h"
#include "primitives/transaction.h"
#include "serialize.h"
#include "uint256.h"

#include <vector>

const int AUXPOW_START_TESTNET = 452608;
const int AUXPOW_START_MAINNET = 498725;

class CAuxPow : public CMerkleTx
{
public:
    CAuxPow(const CTransaction& txIn) : CMerkleTx(txIn)
    {
    }

    CAuxPow() :CMerkleTx()
    {
    }

    // Merkle branch with root vchAux
    // root must be present inside the coinbase
    std::vector<uint256> vChainMerkleBranch;
    //Index of chain in chains merkle tree
    int nChainIndex;
    CBlockHeader parentBlock;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(*(CMerkleTx*)this);
        nVersion = this->nVersion;

        READWRITE(vChainMerkleBranch);
        READWRITE(nChainIndex);
        // Always serialize the saved parent block as header so that the size of CAuxPow
        // is consistent.
        READWRITE(parentBlock);
    }

    /**
   * Check the auxpow, given the merge-mined block's hash and our chain ID.
   * Note that this does not verify the actual PoW on the parent block!  It
   * just confirms that all the merkle branches are valid.
   * @param hashAuxBlock Hash of the merge-mined block.
   * @param nChainId The auxpow chain ID of the block to check.
   * @param params Consensus parameters.
   * @return True if the auxpow is valid.
   */
    bool check(const uint256& hashAuxBlock, int nChainId, const Consensus::Params& params) const;

    /**
   * Get the parent block's hash.  This is used to verify that it
   * satisfies the PoW requirement.
   * @return The parent block hash.
   */
    inline uint256
    getParentBlockPoWHash() const
    {
        return parentBlock.GetPoWHash();
    }

    uint256 GetParentBlockHash() const
    {
        return parentBlock.GetPoWHash();
    }
};

template<typename Stream> void SerReadWrite(Stream& s, boost::shared_ptr<CAuxPow>& pobj, int nType, int nVersion, CSerActionSerialize ser_action)
{
    if (nVersion & BLOCK_VERSION_AUXPOW) {
        ::Serialize(s, *pobj, nType, nVersion);
    }
}

template<typename Stream> void SerReadWrite(Stream& s, boost::shared_ptr<CAuxPow>& pobj, int nType, int nVersion, CSerActionUnserialize ser_action)
{
    if (nVersion & BLOCK_VERSION_AUXPOW) {
        pobj.reset(new CAuxPow());
        ::Unserialize(s, *pobj, nType, nVersion);
    } else
        pobj.reset();
}

extern void RemoveMergedMiningHeader(std::vector<unsigned char>& vchAux);
extern int GetAuxPowStartBlock();
extern CKeyID GetAuxpowMiningKey();
#endif
