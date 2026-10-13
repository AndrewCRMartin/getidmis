if [ "X" == "X$IDMISCERTFILE" ] || [ "X" == "X$IDMISPWDFILE" ]; then
    echo "You must set IDMISCERTFILE to your .p12 certificate file"
    echo "    and set IDMISPWDFILE to your password file"
    exit;
fi

# Fix digital envelope
IN=$IDMISCERTFILE
OUT=${IN}.new
TMP=/tmp/cert.tmp
PW=`cat $IDMISPWDFILE | sed s/\n//`
cp $IN ${IN}.bak

openssl pkcs12 -in $IN -nodes -legacy -out $TMP -passin pass:$PW && openssl pkcs12 -in $TMP -export -passout pass:$PW -out $OUT
rm $TMP
mv $OUT $IN

# Ignore key size
sudo update-crypto-policies --set LEGACY

