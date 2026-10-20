FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    wget curl git build-essential \
    cmake ninja-build \
    pkg-config \
    ffmpeg \
    libsdl2-dev \
    libopencv-dev \
    qtbase5-dev qt5-qmake qtbase5-dev-tools \
    libqt5serialport5-dev \
    qt6-base-dev qt6-base-dev-tools qt6-tools-dev \
    qt6-serialport-dev \
    libxcb-util1 libxcb-icccm4 libxcb-image0 libxcb-keysyms1 \
    libxcb-randr0 libxcb-render-util0 libxcb-shape0 libxcb-xfixes0 \
    libx11-xcb1 libxcb-xinerama0 \
    libxkbcommon-x11-0 \
    libfontconfig1 libfreetype6 \
    libpulse-dev \
    libgl-dev \
    gdb \
    rsync \
    mc
# RUN apt-get clean && rm -rf /var/lib/apt/lists/*

ENV DEBIAN_FRONTEND=
WORKDIR /workspace
CMD ["/bin/bash"]
