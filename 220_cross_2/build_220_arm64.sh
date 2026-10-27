#!/bin/bash

set -euo pipefail

PROJECT_ROOT="$(cd -- "$(dirname -- "$0")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
TOOLCHAIN_FILE="$PROJECT_ROOT/arm64_toolchain_Qt_6_7.cmake"
QT_HOST_PATH="/home/user/Qt/6.7.3/gcc_64"

cmake -S "$PROJECT_ROOT/src" -B "$BUILD_DIR" \
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
    -DCMAKE_BUILD_TYPE=Release \
    -DQT_HOST_PATH:UNINITIALIZED="$QT_HOST_PATH" \
    -DQt6CoreTools_DIR:PATH="$QT_HOST_PATH/lib/cmake/Qt6CoreTools"

cmake --build "$BUILD_DIR" --parallel "$(nproc)"

rsync -av "$BUILD_DIR/ldc20i-control" "$PROJECT_ROOT/prebuild/config.ini" \
    "$PROJECT_ROOT/prebuild/run_ldc20i.sh" \
    khadas@192.168.1.96:/home/khadas/qt_projects/qgst-goen220/

echo "LDC20I build and deployment complete."
