#!/bin/bash

if [ $# -gt 0 ] ; then
  path=$1
else
  path=$QT_PATH
fi

cd src/server || exit 1
mkdir build &> /dev/null
cd build || exit 1
cmake .. -DCMAKE_PREFIX_PATH="$path" -DCMAKE_BUILD_TYPE=RELEASE
np=$(nproc)
np=$((np+1))
make -j"$np"
cd ..
mkdir ../../bin &> /dev/null
cp build/server ../../bin/server
