#!/bin/bash

bin="$HOME/getidmis"

#green="\e[32m"
#red="\e[31m"
#gnorm="\e[0m"
#rnorm="\e[0m"

green="* "
red="*** "
gnorm=" *"
rnorm=" ***"

echo -e ""
echo -e "${red}INSTALLING GETIDMIS${rnorm}"
echo -e ""
echo -e "Creating a folder for getidmis: $bin"
mkdir -p $bin

echo -e -n "Enter your certificate password: "
read certPW
echo -e $certPW > "$bin/certpw.txt"
echo -e "${green}Your certificate password is now stored in $bin/certpw.txt${gnorm}"

echo -e "${green}Downloading the getidmis executables to $bin${gnorm}"
cd $bin
file="getidmis"
uri="http://stagleys.co.uk/getidmis/mac/$file"
curl -o $file $uri
chmod a+x $file

#file="curl"
#uri="http://stagleys.co.uk/getidmis/mac/$file"
#curl -o $file $uri
#chmod a+x $file

file="getidmis.sh"
uri="http://stagleys.co.uk/getidmis/mac/$file"
curl -o $file $uri
chmod a+x $file

echo ""
echo ""
echo -e "${red}You must now copy your certificate file into $bin${rnorm}"
echo -e "${red}   and rename it cert.p12                        ${rnorm}"
echo ""
echo ""

echo -e -n "Press <Enter> when done"
read junk
