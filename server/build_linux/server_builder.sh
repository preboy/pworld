#!/bin/bash

cd ~/pworld

rm -r -f server 
unzip server.zip

cd server/src

make -j 4

if [ "$?" != "0" ]
then
    echo -e "\033[0;35mBuild FAILED !!!  \033[0"
    echo -e "\033[0;35mBuild FAILED !!!  \033[0"
    echo -e "\033[0;35mBuild FAILED !!!  \033[0"
    read
    exit -1
else
    cd ~/pworld
    echo -e "\033[0;32mBuild OK !!!  \033[0"
fi
