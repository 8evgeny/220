#!/bin/bash

set -e

PROJECT_ROOT=$(pwd)
BUILD_DIR="$PROJECT_ROOT/build"
TOOLCHAIN_FILE="$PROJECT_ROOT/arm64_toolchain_Qt_6_7.cmake"
QT_HOST_PATH="/home/user/Qt/6.7.3/gcc_64"

echo "Cleaning previous build..."
# Полное удаление папки сборки обязательно!
rm -rf "$BUILD_DIR"
mkdir "$BUILD_DIR"
cd "$BUILD_DIR"

echo "Configuring CMake..."
# Мы передаем QT_HOST_PATH и через файл тулчейна, и через -D для надежности

cmake ../src \
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DQT_HOST_PATH:UNINITIALIZED="$QT_HOST_PATH" \
    -DQt6CoreTools_DIR:PATH=/home"$QT_HOST_PATH"/lib/cmake/Qt6CoreTools

echo "Building..."
cmake --build . --parallel $(nproc)

rsync -av "$BUILD_DIR"/qgst-goen220  khadas@192.168.1.96:/home/khadas/qt_projects/qgst-goen220/qgst-goen220

echo "Done."
