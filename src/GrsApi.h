#ifndef GRSAPI_H
#define	GRSAPI_H

#include "amount.h"

class CGrsApi
{
public:
    CGrsApi();
    CAmount GetLatestPrice() const;
    CAmount GetLatestTargetPrice() const;
private:

};

#endif	/* GRSAPI_H */
