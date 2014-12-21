
CAmount ComputeInterest(CTxOut& txOut, periods)
{
   float rate = 0.05;
   CAmount interest = txOut.nValue * pow((1 + rate), periods);
   return interest;
   
}

