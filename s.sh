ADDRESS=Rsh1vhWudfGeLUgRMfUE3ZXfxQfc6vAc3r
AMT=1.00000001
TXID=`ribbitcoin-cli sendtoaddress $ADDRESS 1`
INPUTS="[{\"txid\":\"$TXID\",\"vout\":0}]"
OUTPUTS="{\"$ADDRESS\":$AMT}"

TX=`ribbitcoin-cli createrawtransaction $INPUTS $OUTPUTS`
echo $TX
SIGNTX=`ribbitcoin-cli signrawtransaction $TX`
echo $SIGNTX
