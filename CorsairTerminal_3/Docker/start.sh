#!/bin/bash

xhost +

docker run -it --rm \
  --env DISPLAY=$DISPLAY \
  --volume /tmp/.X11-unix:/tmp/.X11-unix:rw \
  --volume ~/.Xauthority:/root/.Xauthority:rw \
  --volume /media/user/WORK/Github/220/CorsairTerminal_3/:/workspace \
  --network host \
  --cap-add=NET_ADMIN \
  --device=/dev/input \
  --name buildCorsair \
  --entrypoint /bin/bash \
  build_corsair


#--device=/dev/hidraw4 \
