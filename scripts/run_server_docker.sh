#!/bin/bash

if [ "$TAG" != "" ]; then
  tag="$TAG"
else
  tag="latest"
fi

docker pull "sharededitor/server:$tag"
docker run --rm -it \
  --network=host \
  "sharededitor/server:$tag" "$@"
