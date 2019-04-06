#!/usr/bin/env bash
if [ ! -d $1 ]; then
    echo $1 "file not found!"
else
    mkdir DybczakJakub
    cp -R $1 DybczakJakub
    rm -f DybczakJakub/$1/README.MD
    tar cvzf DybczakJakub-$1.tar.gz DybczakJakub
    rm -rf DybczakJakub
fi
