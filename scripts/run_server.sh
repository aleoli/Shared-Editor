#!/bin/bash

# da eseguire dalla root del progetto
# -> ./scripts/run_server.sh

cd src/server

mkdir -p .db

./build/server
