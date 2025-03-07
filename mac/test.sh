#!/bin/bash

bin="/tmp/getidmis"
green="\e[32m"
red="\e[31m"
norm="\e[0m"

echo -e  "${green}INSTALLING GETIDMIS${norm}"
echo -e  ""
echo -e  "Creating a folder for getidmis: $bin"
mkdir -p $bin

echo -e  -n "Enter your certificate password: "
read certPW
echo -e  $certPW > "$bin/certpw.txt"
echo -e  "${green}Your certificate password is now stored in $bin/certpw.txt${norm}"

