#include "GrsApi.h"
#include "amount.h"
#include "chainparams.h"
#include "main.h"


CGrsApi::CGrsApi(const std::string& baseUrl)
  : baseApiUrl(baseUrl)
{}


CAmount CGrsApi::GetPrice(unsigned int time) const
{
    return GetLatestPrice();
}

CAmount CGrsApi::GetLatestPrice() const
{
    return 10 * USCENT1;   // STUB: 0.1USD, TODO(dmc): get actual coin price
}


CDmcSystem::CDmcSystem(const std::string& apiUrl)
    : grsApi(apiUrl)
{}

CAmount CDmcSystem::GetPrice() const
{
    if (chainActive.Tip()->nHeight >= liveFeedSwitchHeight) {
        return grsApi.GetPrice(chainActive.Tip()->nTime);
    }
    return 10 * USCENT1;   // 0.1USD
}

CAmount CDmcSystem::GetPrice(unsigned int time) const
{
    if (chainActive.Tip()->nHeight >= liveFeedSwitchHeight) {
        return grsApi.GetPrice(time);
    }
    return GetPrice();
}

CAmount CDmcSystem::GetTargetPrice() const
{
    if (chainActive.Tip()->nHeight >= liveFeedSwitchHeight) {
        const CBlockIndex* pindex = chainActive.Tip();
        return GetTargetPrice(pindex->nReward);
    }
    return 1 * USD1;    // 1USD
}

CAmount CDmcSystem::GetTargetPrice(unsigned int time) const
{
    if (chainActive.Tip()->nHeight >= liveFeedSwitchHeight) {
        const CBlockIndex* pindex;  // TODO(dmc): get block for time
        return GetTargetPrice(pindex->nReward);
    }
    return GetTargetPrice();
}

CAmount CDmcSystem::GetBlockReward() const
{
    return chainActive.Tip()->nReward;
}

CAmount CDmcSystem::GetBlockReward(const CBlockIndex* pindex) const
{
    CAmount nSubsidy = 1 * COIN;

    int nHeight = pindex->nHeight;

    if (pindex->nHeight >= liveFeedSwitchHeight) {
        CAmount genesisReward = 65535 * COIN;
        CAmount minReward = 1 * COIN;
        CAmount maxReward = 100000 * COIN;

        CAmount prevReward = pindex->pprev ? pindex->pprev->nReward : genesisReward;
        CAmount reward = prevReward;
        unsigned int price = GetPrice(pindex->nTime);
        CAmount target = GetTargetPrice(prevReward);

        if (price < target) {
            reward -= 1 * COIN;
        } else if (price > target) {
            reward += 1 * COIN;
        }
        nSubsidy = std::max(minReward, std::min(reward, maxReward));
    } else {
        // from deprecated GetBlockValue
        if (Params().NetworkID() == CBaseChainParams::MAIN) {
            const int kFullRewardZone       = 128000;
            const int kFullReward           = 65535;
            const int kDecreasingRewardZone = kFullRewardZone + 1 + kFullReward;

            if (nHeight >= 0 && nHeight <= kFullRewardZone) {
                nSubsidy = kFullReward * COIN;
            } else if (nHeight > kFullRewardZone && nHeight < kDecreasingRewardZone) {
                nSubsidy = (kDecreasingRewardZone - nHeight) * COIN;
            }
        } else {
            nSubsidy = 1024 * COIN;
        }
    }

    return nSubsidy;
}

CAmount CDmcSystem::GetBlockReward(unsigned int time) const
{
//    unsinged int now = 0;
//    if (time > lastBlockTime) {
//        
//    }
    return GetBlockReward(chainActive.Tip());   // TODO(dmc)
}

CAmount CDmcSystem::GetTotalCoins() const
{
    return chainActive.Tip()->nChainReward;
}

CAmount CDmcSystem::GetMarketCap() const
{
    return (GetTotalCoins() / COIN) * GetPrice();
}

CAmount CDmcSystem::GetTargetPrice(CAmount reward) const
{
    CAmount minTargetPrice = 1 * USD1 + 1 * USCENT1;    // 1.01USD
    CAmount targetPrice = 1 * USD1 + (reward / (100 * COIN));

    return std::max(minTargetPrice, targetPrice);
}
