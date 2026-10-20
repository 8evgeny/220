#!/bin/bash
PROJECT_ROOT=$(pwd)
xhost +

docker run -it --rm \
  --env DISPLAY=$DISPLAY \
  --volume /tmp/.X11-unix:/tmp/.X11-unix:rw \
  --volume ~/.Xauthority:/root/.Xauthority:rw \
  --volume "$PROJECT_ROOT"/:/workspace \
  --network host \
  --cap-add=NET_ADMIN \
  --device=/dev/input \
  --name buildCorsair \
  --entrypoint /bin/bash \
  build_corsair

#  --device=/dev/hidraw4 \
