ADDRESS=Rsh1vhWudfGeLUgRMfUE3ZXfxQfc6vAc3r
##ADDRESS=Rpazq1qUNn9wS5zQ7tatW1cPmoXGAMRk5E
AMT=1.00000009
TXID=`ribbitcoin-cli sendtoaddress $ADDRESS 1`
INPUTS="[{\"txid\":\"$TXID\",\"vout\":0}]"
OUTPUTS="{\"$ADDRESS\":$AMT}"

TX=`ribbitcoin-cli createrawtransaction $INPUTS $OUTPUTS`
echo $TX
#SIGNTX=`ribbitcoin-cli signrawtransaction $TX`
#echo $SIGNTX
