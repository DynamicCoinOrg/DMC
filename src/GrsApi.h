#ifndef GRSAPI_H
#define	GRSAPI_H

#include "amount.h"
#include "chain.h"
#include <ctime>
#include <string>
#include <map>
#include <utility>

class CGrsApi
{
public:
    CGrsApi(const std::string& baseUrl);

    CAmount GetPrice(unsigned int time) const;
    CAmount GetLatestPrice() const;
    
private:

  typedef std::pair<unsigned int, unsigned int> time_interval_t;
  std::map<time_interval_t, CAmount> historicalPrices;

  const std::string baseApiUrl;
  
  const static unsigned int block_0_t      = 1417833678;
  const static unsigned int block_128002_t = 1419903209;
  const static unsigned int block_193536_t = 1418206783;
  const static unsigned int block_livefeed_switch_t = 0;   //TODO(dmc): define more precisely
};

class CDmcSystem
{
public:
    CDmcSystem(const std::string& apiUrl);

    CAmount GetPrice() const;
    CAmount GetPrice(unsigned int time) const;

    CAmount GetTargetPrice() const;
    CAmount GetTargetPrice(unsigned int time) const;

    CAmount GetBlockReward() const;
    CAmount GetBlockReward(const CBlockIndex* pindex) const;
    CAmount GetBlockRewardForNewTip(unsigned int time) const;

    CAmount GetTotalCoins() const;
    CAmount GetMarketCap() const;

protected:
    CAmount GetTargetPrice(CAmount reward) const;
    
private:
    CGrsApi grsApi;
    const static int liveFeedSwitchHeight = 10000000;
    
    const static CAmount genesisReward = 65535 * COIN;
    const static CAmount minReward = 1 * COIN;
    const static CAmount maxReward = 100000 * COIN;
    
    const static CAmount minTargetPrice = 1 * USD1 + 1 * USCENT1;    // 1.01USD
};


#endif	/* GRSAPI_H */
