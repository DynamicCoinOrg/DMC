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
    CAmount GetBlockReward(unsigned int time) const;

    CAmount GetTotalCoins() const;
    CAmount GetMarketCap() const;

protected:
    CAmount GetTargetPrice(CAmount reward) const;
    
private:
    CGrsApi grsApi;
    const static int switchHeight = 10000000;
};


#endif	/* GRSAPI_H */
