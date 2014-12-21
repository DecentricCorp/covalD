#include "amount.h"
#include "math.h"
#include "chain.h"

extern BlockMap mapBlockIndex;
extern CChain chainActive;

CAmount ComputeInterest(CTransaction& tx);

