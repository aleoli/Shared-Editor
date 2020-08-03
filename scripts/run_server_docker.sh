#!/bin/bash

if [ "$TAG" != "" ]; then
  tag="$TAG"
else
  tag="latest"
fi

docker pull "sharededitor/server:$tag"
docker run --rm -it \
  --network=host \
  -v "$PWD/storage:/home/user/.shared_editor" \
  "sharededitor/server:$tag" "$@"
