#!/bin/bash

if [ "$TAG" != "" ]; then
  tag="$TAG"
else
  tag="latest"
fi

docker pull "sharededitor/stub-client:$tag"
docker run --rm -it \
  --privileged \
  --network=host \
  -e DISPLAY="$DISPLAY" \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  -v "$HOME"/.Xauthority:/home/user/.Xauthority \
  "sharededitor/stub-client:$tag" "$@"
