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
echo -e "${red}RUNNING GETIDMIS${rnorm}"
echo -e ""

exe="$bin/getidmis"
param="-c $bin/cert.p12 -f $bin/certpw.txt -x $bin/curl"

echo -e "${green}Enter the INN request numbers you wish to download             ${gnorm}"
echo -e "${green}   You may enter multiple numbers (space-separated) on one line${gnorm}"
echo -e "${green}   or one per line, or a combination!                          ${gnorm}"

reqNum='x'
while [ "X$reqNum" != "X" ]
do
   echo -n "Request numbers - or a blank line to finish: "
   read reqNum
   echo "Value = $reqNum"
   if [ "X$reqNum" != 'X' ]
   then
      if [ "X$requests" == 'X' ]
      then
          requests=$reqNum
      else
          requests="$requests $reqNum"
      fi
   fi
done   

cmd="$exe $param $requests"
echo -e "Running command: $cmd"
$cmd

echo ""
echo -e "${red}Downloading complete!${rnorm}"
echo ""

echo -n "Press <Enter> when done"
read junk

