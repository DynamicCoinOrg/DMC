#include "GrsApi.h"
#include "amount.h"
#include "chainparams.h"
#include "main.h"
#include "block.h"
#include "util.h"


CGrsApi::CGrsApi(const std::string& baseUrl)
  : baseApiUrl(baseUrl)
{}


CAmount CGrsApi::GetPrice(unsigned int time) const
{
    // projected prices for before-the-trading era

    if (time >= block_0_t && time < block_128002_t) {
        // genesis reward zone
        return 656 * USD1 + 35 * USCENT1;
    } else if (time >= block_128002_t && time < block_193536_t) {
        // decreasing reward zone
        return 10 * USCENT1;
    } else if (time >= block_193536_t && time < Params().LiveFeedSwitchTime()) {
        return 10 * USCENT1;
    }

    // get price from the live feed
    return GetLatestPrice();    //TODO(dmc): temporary simplification
}

CAmount CGrsApi::GetLatestPrice() const
{
    return 10 * USCENT1;   // STUB: 0.1USD, TODO(dmc): get actual coin price
}


CDmcSystem::CDmcSystem(const std::string& apiUrl)
    : grsApi(apiUrl)
{
    genesisReward = 65535 * COIN;
    minReward = 1 * COIN;
    maxReward = 100000 * COIN;
    minTargetPrice = 1 * USD1 + 1 * USCENT1;    // 1.01USD
}

bool CDmcSystem::CheckBlockReward(const CBlock& block, CAmount nFees, CValidationState& state, CBlockIndex* pindex) const
{
    LogPrintf("CDmcSystem::CheckBlockReward: block.nTime=%d, fees=%d, hash=%s, height=%d, time=%d\n", block.nTime, nFees, pindex->GetBlockHash().ToString(), pindex->nHeight, pindex->nTime);

    CAmount blockOutput = block.vtx[0].GetValueOut();
    if (!blockOutput || blockOutput <= nFees) {
        return state.DoS(100,
                     error("CDmcSystem::CheckBlockReward() : coinbase pays zero or <= fees (coinbase=%d, fees=%d)",
                           blockOutput, nFees),
                           REJECT_INVALID, "bad-cb-amount");
    }
    CAmount blockReward = blockOutput - nFees;

    //TODO(dmc): temporary simplification – one GetBlockReward result comparison check
    //            should be enough in the future (the second case of "if" construction)
    if (pindex->nTime > Params().LiveFeedSwitchTime()) {
        // simplified reward checks
        CAmount prevReward = pindex->pprev ? pindex->pprev->nReward : genesisReward;
        CAmount rewDiff = std::abs(blockReward - prevReward);
        if ((rewDiff == 0 || rewDiff == 1) &&
            (blockReward >= minReward && blockReward <= maxReward)) {
            return true;
        }
        return state.DoS(100,
                         error("CDmcSystem::CheckBlockReward() : coinbase pays wrong (actual=%d vs mustbe=%d)",
                               block.vtx[0].GetValueOut(), GetBlockReward(pindex) + nFees),
                               REJECT_INVALID, "bad-cb-amount");
    }

    if (blockReward != GetBlockReward(pindex)) {
        return state.DoS(100,
                     error("CDmcSystem::CheckBlockReward() : coinbase pays wrong reward (actual=%d vs mustbe=%d, fees=%d)",
                           blockReward, GetBlockReward(pindex), nFees),
                           REJECT_INVALID, "bad-cb-amount");
    }
    return true;
}

CAmount CDmcSystem::GetBlockReward(const CBlockIndex* pindex) const
{
    LogPrintf("CDmcSystem::GetBlockReward: hash=%s, height=%d, time=%d\n", pindex->GetBlockHash().ToString(), pindex->nHeight, pindex->nTime);

    CAmount nSubsidy = 1 * COIN;

    int nHeight = pindex->nHeight;

    if (pindex->nTime > Params().LiveFeedSwitchTime()) {
        CAmount prevReward = pindex->pprev ? pindex->pprev->nReward : genesisReward;
        CAmount reward     = prevReward;
        unsigned int price = GetPrice(pindex->nTime);
        CAmount target     = GetTargetPrice(prevReward);

        if (price < target) {
            reward -= 1 * COIN;
        } else if (price > target) {
            reward += 1 * COIN;
        }
        nSubsidy = std::max(minReward, std::min(reward, maxReward));
    } else {
        if (Params().NetworkID() == CBaseChainParams::MAIN) {
            const int kGenesisRewardZone    = 128000;
            const int kGenesisReward        = 65535;
            const int kDecreasingRewardZone = kGenesisRewardZone + 1 + kGenesisReward;

            if (nHeight >= 0 && nHeight <= kGenesisRewardZone) {
                nSubsidy = kGenesisReward * COIN;
            } else if (nHeight > kGenesisRewardZone && nHeight < kDecreasingRewardZone) {
                nSubsidy = (kDecreasingRewardZone - nHeight) * COIN;
            }
        } else {
            nSubsidy = 1024 * COIN;
        }
    }

    return nSubsidy;
}

CAmount CDmcSystem::GetBlockRewardForNewTip(unsigned int time) const
{
    const CBlockIndex* tip = chainActive.Tip();
    
    if (!tip) {
        return genesisReward;
    }

    CAmount nSubsidy = 1 * COIN;

    int nHeight = tip->nHeight + 1;

    if (tip->nTime > Params().LiveFeedSwitchTime()) {
        CAmount prevReward = tip->nReward;
        CAmount reward     = prevReward;
        unsigned int price = GetPrice(tip->nTime);
        CAmount target     = GetTargetPrice(prevReward);

        if (price < target) {
            reward -= 1 * COIN;
        } else if (price > target) {
            reward += 1 * COIN;
        }
        nSubsidy = std::max(minReward, std::min(reward, maxReward));
    } else {
        if (Params().NetworkID() == CBaseChainParams::MAIN) {
            const int kGenesisRewardZone       = 128000;
            const int kGenesisReward           = 65535;
            const int kDecreasingRewardZone = kGenesisRewardZone + 1 + kGenesisReward;

            if (nHeight >= 0 && nHeight <= kGenesisRewardZone) {
                nSubsidy = kGenesisReward * COIN;
            } else if (nHeight > kGenesisRewardZone && nHeight < kDecreasingRewardZone) {
                nSubsidy = (kDecreasingRewardZone - nHeight) * COIN;
            }
        } else {
            nSubsidy = 1024 * COIN;
        }
    }

    return nSubsidy;
}


CAmount CDmcSystem::GetBlockReward() const
{
    return chainActive.Tip()->nReward;
}

CAmount CDmcSystem::GetPrice() const
{
    return grsApi.GetPrice(chainActive.Tip()->nTime);
}

CAmount CDmcSystem::GetTargetPrice() const
{
    return GetTargetPrice(chainActive.Tip()->nReward);
}

CAmount CDmcSystem::GetTotalCoins() const
{
    return chainActive.Tip()->nChainReward;
}

CAmount CDmcSystem::GetMarketCap() const
{
    return (GetTotalCoins() / COIN) * GetPrice();
}


CAmount CDmcSystem::GetPrice(unsigned int time) const
{
    return grsApi.GetPrice(time);
}

CAmount CDmcSystem::GetTargetPrice(unsigned int time) const
{
    //TODO(dmc): temporary simplification
//    const CBlockIndex* pindex;  // TODO(dmc): get block for time
//    return GetTargetPrice(pindex->nReward);
    return 10 * USCENT1;
}


CAmount CDmcSystem::GetTargetPrice(CAmount reward) const
{
    CAmount targetPrice = 1 * USD1 + (reward / (100 * COIN));

    return std::max(minTargetPrice, targetPrice);
}
