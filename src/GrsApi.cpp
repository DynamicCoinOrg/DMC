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

CAmount CDmcSystem::GetPrice(unsigned int time) const
{
    return grsApi.GetPrice(time);
}

CAmount CDmcSystem::GetTargetPrice(unsigned int time) const
{
    return GetLatestTargetPrice();
}

CAmount CDmcSystem::GetBlockReward() const
{
    return chainActive.Tip()->nReward;
}

CAmount CDmcSystem::GetBlockReward(const CBlockIndex* pindex) const
{
    // from deprecated GetBlockValue

    int nHeight = pindex->nHeight;

    CAmount nSubsidy = 1 * COIN;

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

    return nSubsidy;
}

CAmount CDmcSystem::GetBlockReward(unsigned int time) const
{
    return GetBlockReward(chainActive.Tip());   // TODO(dmc)
}

CAmount CDmcSystem::GetTotalCoins() const
{
    return chainActive.Tip()->nChainReward;
}

CAmount CDmcSystem::GetMarketCap() const
{
    return (GetTotalCoins() / COIN) * GetLatestPrice();
}

CAmount CDmcSystem::GetLatestPrice() const
{
    return grsApi.GetPrice(chainActive.Tip()->nTime);
}

CAmount CDmcSystem::GetLatestTargetPrice() const
{
    return 1 * USD1;    // STUB: 1USD, TODO(dmc): get actual target price
}
