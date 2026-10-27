#!/bin/bash
PROJECT_ROOT=$(pwd)/build_in_Qt
xhost +

sudo ip a a 192.168.1.108/24 dev eno1 && \
sudo ip route replace 169.254.0.51/32 dev eno1 src 169.254.0.2

docker run -it --rm \
  --env DISPLAY=$DISPLAY \
  --volume /tmp/.X11-unix:/tmp/.X11-unix:rw \
  --volume ~/.Xauthority:/root/.Xauthority:rw \
  --volume "$PROJECT_ROOT"/:/workspace \
  --network host \
  --cap-add=NET_ADMIN \
  --device=/dev/input \
  --name buildCorsair \
  build_corsair \
  /workspace/CorsairTerminalV

