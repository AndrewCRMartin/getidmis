#!/bin/bash

bin="$HOME/getidmis"

#green="\e[32m"
#red="\e[31m"
#norm="\e[0m"

green="* "
red="*** "
norm=""

echo -e "${green}INSTALLING GETIDMIS${norm}"
echo -e ""
echo -e "Creating a folder for getidmis: $bin"
mkdir -p $bin

echo -e -n "Enter your certificate password: "
read certPW
echo -e $certPW > "$bin/certpw.txt"
echo -e "${green}Your certificate password is now stored in $bin/certpw.txt${norm}"

echo -e Downloading the getidmis executables to $bin -fo Green
cd $bin
$file = "getidmis"
$uri = "http://stagleys.co.uk/getidmis/mac/$file"
curl -o $file $uri
#$file = "curl.exe"
#$uri = "http://stagleys.co.uk/getidmis/$file"
#curl -o $file $uri
$file = "getidmis.sh"
$uri = "http://stagleys.co.uk/getidmis/mac/$file"
curl -o $file $uri

echo -e "${red}You must now copy your certificate file into $bin"
echo -e "${red}   and rename it cert.p12${norm}"

echo -e -n "Press <Enter> when done"
read junk
