#!/usr/bin/bash

#DEST=/home/httpd/html/getidmis
DEST=/serv/www/html_bioinf/getidmis

make
(cd windows; make -f Makefile.windows64)
mkdir -p $DEST
mv getidmis $DEST/getidmis.linux
mv windows/gi.exe $DEST
make clean
(cd windows; make -f Makefile.windows64 clean)
cp windows/curl/curl.exe $DEST
cp windows/install.bat   $DEST
cp windows/getidmis.bat  $DEST
cp www/index.html        $DEST

# (cd $DEST; zip -r ../getidmis.zip)
# rm -rf $DEST


