#include "GrsApi.h"
#include "amount.h"
#include "chainparams.h"
#include "main.h"


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
    } else if (time >= block_193536_t && time < block_livefeed_switch_t) {
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
{}

CAmount CDmcSystem::GetPrice() const
{
    return grsApi.GetPrice(chainActive.Tip()->nTime);
}

CAmount CDmcSystem::GetPrice(unsigned int time) const
{
    return grsApi.GetPrice(time);
}

CAmount CDmcSystem::GetTargetPrice() const
{
    if (chainActive.Tip()) {
        return GetTargetPrice(chainActive.Tip()->nReward);
    }
    return 0 * USD1;    // 0USD
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

    if (nHeight >= liveFeedSwitchHeight) {
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

CAmount CDmcSystem::GetBlockRewardForNewTip(unsigned int time) const
{
    const CAmount genesisReward = 65535 * COIN;
    const CAmount minReward = 1 * COIN;
    const CAmount maxReward = 100000 * COIN;

    const CBlockIndex* tip = chainActive.Tip();
    
    if (!tip) {
        return genesisReward;
    }

    CAmount nSubsidy = 1 * COIN;

    int nHeight = tip->nHeight + 1;

    if (nHeight >= liveFeedSwitchHeight) {
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
    CAmount targetPrice = 1 * USD1 + (reward / (100 * COIN));

    return std::max(minTargetPrice, targetPrice);
}
