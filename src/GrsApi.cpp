#include "GrsApi.h"
#include "amount.h"
#include "chainparams.h"
#include "main.h"
#include "util.h"

#include <iostream>
#include <sstream>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Infos.hpp>

#include "json/json_spirit_value.h"
#include "json/json_spirit_reader_template.h"


CGrsApi::CGrsApi(const std::string& baseUrl)
  : baseApiUrl(baseUrl)
{}


CAmount CGrsApi::GetPrice(unsigned int time)
{
    LogPrintf("CGrsApi::GetPrice: time = %d\n", time);

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

    // TODO(dmc): check cached price


    // get price from the live feed
    while (true) {  //TODO(dmc): !!!
        try {
            unsigned int timestamp = 0; //TODO(dmc): must be 'time'
            CAmount price = GetGrsApiPrice(timestamp);
            LogPrintf("GRS price for timestamp: time = %d, price = %d\n", time, price);
            return price;
        } catch (const std::runtime_error& e) {
            error("Can't get GRS price for timestamp: %s\n", e.what());
        }
    }
}

CAmount CGrsApi::GetLatestPrice() const
{
    return 10 * USCENT1;   // STUB: 0.1USD, TODO(dmc): get actual coin price
}

CAmount CGrsApi::GetGrsApiPrice(unsigned int timestamp)
{
    LogPrintf("Getting GRS price for timestamp: time = %d\n", timestamp);
    std::ostringstream reqArgs;
    if (timestamp != 0) {
        reqArgs << timestamp;
    }
    return DoApiPriceRequest("price", reqArgs.str());
}

CAmount CGrsApi::DoApiPriceRequest(const std::string& reqName,
                                   const std::string& args) const
{
    CAmount price = 0;
    int apiResponseCode = 200;

    std::ostringstream apiUrl;
    apiUrl << baseApiUrl << reqName << "/" << args;

    std::ostringstream rawResponse;
    try {
        apiResponseCode = DoApiRequest(apiUrl.str(), rawResponse);
        std::clog << "GRS API response: " << rawResponse.str() << std::endl;
    } catch (const curlpp::RuntimeError& e) {
        throw std::runtime_error(std::string(e.what())
                                 + "; url = '" + apiUrl.str() + "'"
                                 + "; response = '" + rawResponse.str() + "'");
    } catch (const curlpp::LogicError& e) {
        throw std::runtime_error(std::string(e.what())
                                 + "; url = '" + apiUrl.str() + "'"
                                 + "; response = '" + rawResponse.str() + "'");
    } catch (const std::invalid_argument& e) {
        throw std::runtime_error(std::string(e.what())
                                + "; url = '" + apiUrl.str() + "'"
                                + "; response = '" + rawResponse.str() + "'");
    } catch (const std::domain_error& e) {
        throw std::runtime_error(std::string(e.what())
                                 + "; url = '" + apiUrl.str() + "'"
                                 + "; response = '" + rawResponse.str() + "'");
    }

    if (apiResponseCode != 200) {
        std::ostringstream oss;
        oss << "apiResponseCode != 200; code = "
            << apiResponseCode
            << "; url = '" + apiUrl.str() + "'"
             + "; response = '" + rawResponse.str() + "'";
        throw std::runtime_error(oss.str());
    }

    try {
        json_spirit::Value value;
        json_spirit::read_string(rawResponse.str(), value);
        json_spirit::Object obj(value.get_obj());

        bool price_read = false;
        for(json_spirit::Object::size_type i = 0; i != obj.size(); ++i) {
            const json_spirit::Pair& pair = obj[i];
            if (pair.name_ == "price") {
                price = pair.value_.get_int();
                price_read = true;
            }
        }
        if (!price_read) {
            throw std::domain_error("No price field found");
        }
    } catch (const std::invalid_argument& e) {
        throw std::runtime_error(std::string(e.what())
                                + "; url = '" + apiUrl.str() + "'"
                                + "; response = '" + rawResponse.str() + "'");
    } catch (const std::domain_error& e) {
        throw std::runtime_error(std::string(e.what())
                                 + "; url = '" + apiUrl.str() + "'"
                                 + "; response = '" + rawResponse.str() + "'");
    }

    return price;
}

int CGrsApi::DoApiRequest(const std::string& url, std::ostringstream& oss) const
{
    // std::clog << "GRS API url: " << url << std::endl;

    curlpp::Cleanup myCleanup;

    curlpp::options::Url reqUrl(url);
    curlpp::Easy request;
    request.setOpt(reqUrl);

    curlpp::options::WriteStream ws(&oss);
    request.setOpt(ws);
    request.perform();

    return curlpp::infos::ResponseCode::get(request);
}


CDmcSystem::CDmcSystem(const std::string& apiUrl)
    : grsApi(apiUrl)
{
    genesisReward = 65535 * COIN;
    minReward = 1 * COIN;
    maxReward = 100000 * COIN;
    minTargetPrice = 1 * USD1 + 1 * USCENT1;    // 1.01USD
}

bool CDmcSystem::CheckBlockReward(const CBlock& block, CAmount nFees, CValidationState& state, CBlockIndex* pindex)
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

CAmount CDmcSystem::GetBlockReward(const CBlockIndex* pindex)
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

CAmount CDmcSystem::GetBlockRewardForNewTip(unsigned int time)
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

CAmount CDmcSystem::GetPrice()
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

CAmount CDmcSystem::GetMarketCap()
{
    return (GetTotalCoins() / COIN) * GetPrice();
}


CAmount CDmcSystem::GetPrice(unsigned int time)
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
