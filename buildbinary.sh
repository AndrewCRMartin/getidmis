#!/usr/bin/bash
DEST=getidmis_binary

make
(cd windows; make -f Makefile.windows64)
mkdir -p $DEST
mv getidmis $DEST/getidmis.linux
mv windows/getidmis.exe $DEST
make clean
(cd windows; make -f Makefile.windows64 clean)
cp windows/curl/curl.exe $DEST

(cd $DEST; zip -r ../getidmis.zip)
rm -rf $DEST


