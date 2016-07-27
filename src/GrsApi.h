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

    // Price at the specified time
    CAmount GetPrice(unsigned int time) const;
    // Last known price broadcasted by GRS
    CAmount GetLatestPrice() const;

private:

  typedef std::pair<unsigned int, unsigned int> time_interval_t;
  std::map<time_interval_t, CAmount> historicalPrices;

  const std::string baseApiUrl;
  
  const static unsigned int block_0_t      = 1417833678;
  const static unsigned int block_128002_t = 1419903209;
  const static unsigned int block_193536_t = 1418206783;
};

class CDmcSystem
{
public:
    CDmcSystem(const std::string& apiUrl);

    CAmount GetBlockReward(const CBlockIndex* pindex) const;
    CAmount GetBlockRewardForNewTip(unsigned int time) const;

    // Blockchain tip information
    CAmount GetBlockReward() const;
    CAmount GetPrice() const;
    CAmount GetTargetPrice() const;
    CAmount GetTotalCoins() const;
    CAmount GetMarketCap() const;

protected:
    CAmount GetPrice(unsigned int time) const;
    CAmount GetTargetPrice(unsigned int time) const;
    
protected:
    CAmount GetTargetPrice(CAmount reward) const;
    
private:
    CGrsApi grsApi;

    const CAmount genesisReward = 65535 * COIN;
    const CAmount minReward = 1 * COIN;
    const CAmount maxReward = 100000 * COIN;
    const CAmount minTargetPrice = 1 * USD1 + 1 * USCENT1;    // 1.01USD
};


#endif	/* GRSAPI_H */
