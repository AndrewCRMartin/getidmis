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
uri="http://www.bioinf.org.uk/getidmis/mac/$file"
curl -s -o $file $uri
chmod a+x $file

file="curl_arm"
uri="http://www.bioinf.org.uk/getidmis/mac/curl/$file"
curl -s -o $file $uri
chmod a+x $file

file="curl_x86"
uri="http://www.bioinf.org.uk/getidmis/mac/curl/$file"
curl -s -o $file $uri
chmod a+x $file

file="getidmis.sh"
uri="http://www.bioinf.org.uk/getidmis/mac/$file"
curl -s -o $file $uri
chmod a+x $file

echo ""
echo -e "${green}Determining system curl version${gnorm}"
curlV=`curl --version | head -1 | awk '{print $2}' | awk -F. '{print $1}'`

if [ $curlV -ge 8 ]
then
    echo -e "${green}Using system version of curl${gnorm}"
    ln -sf /usr/bin/curl curl
else
    echo "Your system version of curl is too old (V7 or below), so using a newer one!"
    echo -e "${green}Determining system type${gnorm}"
    arch=`uname -a | awk '{print $NF}'`
    if [ $arch == "x86_64" ]
    then
        ln -sf curl_x86 curl
    else
        ln -sf curl_arm curl
    fi
fi
    
    

echo ""
echo ""
echo -e "${red}You must now copy your certificate file into $bin${rnorm}"
echo -e "${red}   and rename it cert.p12                                   ${rnorm}"
echo ""
echo ""

echo -e -n "Press <Enter> when done"
read junk
