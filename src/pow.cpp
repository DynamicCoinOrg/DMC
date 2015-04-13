// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "pow.h"

#include <vector>
#include <algorithm>

#include "chain.h"
#include "chainparams.h"
#include "primitives/block.h"
#include "uint256.h"
#include "util.h"

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock)
{
    const uint256 nProofOfWorkLimit = Params().ProofOfWorkLimit();
    const unsigned int nProofOfWorkLimitNBits = nProofOfWorkLimit.GetCompact();

    // Genesis block
    if (pindexLast == NULL) {
        return nProofOfWorkLimitNBits;
    }

    if (Params().AllowMinDifficultyBlocks()) {
        // If the new block's timestamp is more than 2* TargetSpacing
        // then allow mining of a min-difficulty block.
        if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + Params().TargetSpacing()*2) {
            return nProofOfWorkLimitNBits;
        }
    }

    const int64_t diff_window = Params().Interval();
    const int64_t diff_timestamp_outlier_cutoff = 60;

    // Go back by `Interval` blocks (Interval = nTargetTimespan / nTargetSpacing)
    std::vector<int64_t> timestamps;
    std::vector<uint256> chainWorks;
    const CBlockIndex* pindexFirst = pindexLast;
    for (int i = 0; pindexFirst && i < diff_window - 1; i++) {
        timestamps.push_back(pindexFirst->GetBlockTime());
        chainWorks.push_back(pindexFirst->nChainWork);

        pindexFirst = pindexFirst->pprev;
    }

    const size_t length = timestamps.size();
    assert(length == chainWorks.size());
    if (length <= 1) {
        return nProofOfWorkLimitNBits;
    }

//    static_assert(diff_window >= 2, "Window is too small");
    assert(length <= diff_window);
    std::sort(timestamps.begin(), timestamps.end(), std::greater<int64_t>());

    // cutoff timestamp outliers
    size_t cutoff_begin = length;
    size_t cutoff_end   = 0;

//    static_assert(2 * diff_timestamp_outlier_cutoff <= diff_window - 2, "Cut length is too large");
    const int64_t timestamp_core = diff_window - 2 * diff_timestamp_outlier_cutoff;
    if (length > timestamp_core) {
        cutoff_begin = (length - timestamp_core + 1) / 2;
        cutoff_end   = cutoff_begin + timestamp_core;
    }
    assert(/*cutoff_begin >= 0 &&*/ cutoff_end + 2 <= cutoff_begin && cutoff_begin <= length);

    int64_t nTargetTimespan = Params().TargetTimespan();
    int64_t nActualTimespan = timestamps[cutoff_end] - timestamps[cutoff_begin - 1];//pindexLast->GetBlockTime() - pindexFirst->GetBlockTime();
    uint256 nTotalWork      = chainWorks[cutoff_end] - chainWorks[cutoff_begin - 1];//pindexLast->nChainWork - pindexFirst->nChainWork;
    LogPrintf("nTargetTimespan = %d\n", nTargetTimespan);
    LogPrintf("nActualTimespan = %d\n", nActualTimespan);
    LogPrintf("nTotalWork = %d, nFirstChainWork = %d, nLastChainWork = %d\n",
               nTotalWork.getdouble(), chainWorks[cutoff_begin - 1].getdouble(),
                                       chainWorks[cutoff_end].getdouble());

    if (nActualTimespan == 0) {
        nActualTimespan = 1;
    }
    assert(nTotalWork > 0);

    // Retarget
    uint256 bnNewHashes(nTotalWork);
    bnNewHashes *= Params().TargetSpacing();
    bnNewHashes += nActualTimespan;
    bnNewHashes -= 1;
    bnNewHashes /= nActualTimespan;

    uint256 bnNew;
    if (bnNewHashes <= GetNBitsHashes(nProofOfWorkLimitNBits)) {
        bnNew = nProofOfWorkLimit;
    } else {
        uint256 powPerHash(nProofOfWorkLimit / GetNBitsHashes(nProofOfWorkLimitNBits));
        bnNew = bnNewHashes * powPerHash - 1;
    }

    /// debug print
    uint256 bnOld;
    bnOld.SetCompact(pindexLast->nBits);
    LogPrintf("GetNextWorkRequired RETARGET\n");
    LogPrintf("Params().TargetTimespan() = %d    nActualTimespan = %d\n", Params().TargetTimespan(), nActualTimespan);
    LogPrintf("Before: %08x  %s\n", pindexLast->nBits, bnOld.ToString());
    LogPrintf("After:  %08x  %s\n", bnNew.GetCompact(), bnNew.ToString());

    return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits)
{
    bool fNegative;
    bool fOverflow;
    uint256 bnTarget;

    if (Params().SkipProofOfWorkCheck())
       return true;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > Params().ProofOfWorkLimit())
        return error("CheckProofOfWork() : nBits below minimum work");

    // Check proof of work matches claimed amount
    if (hash > bnTarget)
        return error("CheckProofOfWork() : hash doesn't match nBits");

    return true;
}

uint256 GetBlockProof(const CBlockIndex& block)
{
    return GetNBitsHashes(block.nBits);
}

uint256 GetNBitsHashes(unsigned int nBits)
{
    uint256 bnTarget;
    bool fNegative;
    bool fOverflow;
    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);
    if (fNegative || fOverflow || bnTarget == 0)
        return 0;
    // We need to compute 2**256 / (bnTarget+1), but we can't represent 2**256
    // as it's too large for a uint256. However, as 2**256 is at least as large
    // as bnTarget+1, it is equal to ((2**256 - bnTarget - 1) / (bnTarget+1)) + 1,
    // or ~bnTarget / (nTarget+1) + 1.
    return (~bnTarget / (bnTarget + 1)) + 1;
}
