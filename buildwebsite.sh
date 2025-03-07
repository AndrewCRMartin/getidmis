#!/usr/bin/bash

#DEST=/home/httpd/html/getidmis
DEST=/serv/www/html_bioinf/getidmis

mkdir -p $DEST
mkdir -p $DEST/linux
mkdir -p $DEST/windows
mkdir -p $DEST/mac

cp bin/linux_getidmis $DEST/linux/getidmis
cp bin/mac_getidmis   $DEST/mac/getidmis
cp bin/windows_gi.exe $DEST/windows/gi.exe

cp windows/curl/curl.exe $DEST/windows
cp windows/install.bat   $DEST/windows
cp windows/getidmis.bat  $DEST/windows
cp mac/install.sh        $DEST/mac
cp mac/getidmis.sh       $DEST/mac
cp www/*.html            $DEST

# (cd $DEST; zip -r ../getidmis.zip)
# rm -rf $DEST


