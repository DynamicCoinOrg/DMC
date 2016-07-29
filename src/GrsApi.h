#ifndef GRSAPI_H
#define	GRSAPI_H

#include "amount.h"
#include "chain.h"

#include <ctime>
#include <string>
#include <map>
#include <utility>

class CValidationState;

class CGrsApi
{
public:
    CGrsApi(const std::string& baseUrl);

    // Price at the specified time
    CAmount GetPrice(unsigned int time);
    // Last known price broadcasted by GRS
    CAmount GetLatestPrice() const;

private:

  CAmount GetGrsApiPrice(unsigned int time = 0);
  CAmount DoApiPriceRequest(const std::string& reqName,
                            const std::string& args) const;
  int DoApiRequest(const std::string& url, std::ostringstream& oss) const;


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

    bool CheckBlockReward(const CBlock& block, CAmount fees, CValidationState& state, CBlockIndex* pindex);
    CAmount GetBlockReward(const CBlockIndex* pindex);
    CAmount GetBlockRewardForNewTip(unsigned int time);

    // Blockchain tip information
    CAmount GetBlockReward() const;
    CAmount GetPrice();
    CAmount GetTargetPrice() const;
    CAmount GetTotalCoins() const;
    CAmount GetMarketCap();

protected:
    CAmount GetPrice(unsigned int time);
    CAmount GetTargetPrice(unsigned int time) const;
    
protected:
    CAmount GetTargetPrice(CAmount reward) const;
    
private:
    CGrsApi grsApi;

    CAmount genesisReward;
    CAmount minReward;
    CAmount maxReward;
    CAmount minTargetPrice;
};


#endif	/* GRSAPI_H */
