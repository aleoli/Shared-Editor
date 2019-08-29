#!/bin/bash

if [ $# -gt 0 ] ; then
  path=$1
else
  path=$QT_PATH
fi

cd src/server
mkdir build &> /dev/null
cd build
cmake .. -DCMAKE_PREFIX_PATH=$path
make
cd ..
mkdir ../../bin &> /dev/null
cp build/server ../../bin/server
