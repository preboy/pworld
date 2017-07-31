#!/bin/bash

cd ~/pworld

rm -r -f public 
unzip public.zip

cd public/libcore

make -j 4

if [ "$?" != "0" ]
then
    echo -e "\033[0;35mBuild FAILED !!!  \033[0"
    echo -e "\033[0;35mBuild FAILED !!!  \033[0"
    echo -e "\033[0;35mBuild FAILED !!!  \033[0"
    read
    exit -1
else
    echo -e "\033[0;32mBuild OK !!!  \033[0"
fi
