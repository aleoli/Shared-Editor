#!/bin/bash

if [ $# -gt 0 ] ; then
  path=$1
else
  path=$QT_PATH
fi

cd src/client/stub
mkdir build &> /dev/null
cd build
cmake .. -DCMAKE_PREFIX_PATH=$path
make -j$((nproc+1))
cd ../../../../
mkdir bin &> /dev/null
cp src/client/stub/build/stubClient bin/stubClient
