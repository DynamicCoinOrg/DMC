#ifndef GRSAPI_H
#define	GRSAPI_H

#include "amount.h"
#include "chain.h"
#include <ctime>
#include <string>

class CGrsApi
{
public:
    CGrsApi(const std::string& baseUrl);

    CAmount GetPrice(unsigned int time) const;
    CAmount GetLatestPrice() const;

private:
  const std::string baseApiUrl;
};

class CDmcSystem
{
public:
    CDmcSystem(const std::string& apiUrl);

    CAmount GetPrice(unsigned int time) const;
    CAmount GetTargetPrice(unsigned int time) const;

    CAmount GetBlockReward() const;
    CAmount GetBlockReward(const CBlockIndex* pindex) const;
    CAmount GetBlockReward(unsigned int time) const;

    CAmount GetTotalCoins() const;
    CAmount GetMarketCap() const;

    CAmount GetLatestPrice() const;
    CAmount GetLatestTargetPrice() const;

private:
    CGrsApi grsApi;
};


#endif	/* GRSAPI_H */
