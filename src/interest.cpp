CInterest::ComputeInterest(int nPeriods, const CTxOut& txOut):
{
    double APY = 0.05;
    double rate = exp(log(1+APY)/nPeriods) - 1;
    CAmount interest = txOut.nValue * pow(1.0+rate, nPeriods) - txOut.nValue;
    return interest;
}

//        CAmount ComputeInterest(int periods, const uint256 txHash)
//        {
//            // do blockchain lookup here
//
//            double APY = 0.05;
//            double rate = exp(log(1+APY)/periods) - 1;
//            CAmount interest = txOut.nValue * pow(1.0+rate, periods);
//            return interest;
//        }

};

