#include "GrsApi.h"
#include "amount.h"


CGrsApi::CGrsApi()
{

}

CAmount CGrsApi::GetLatestPrice() const
{
    return 10 * USCENT1;   // STUB: 0.1USD, TODO(dmc): get actual coin price
}

CAmount CGrsApi::GetLatestTargetPrice() const
{
    return 1 * USD1;    // STUB: 1USD, TODO(dmc): get actual target price
}
