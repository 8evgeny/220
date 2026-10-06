#!/bin/bash

set -e

PROJECT_ROOT=$(pwd)
BUILD_DIR="$PROJECT_ROOT/Debug"
TOOLCHAIN_FILE="$PROJECT_ROOT/arm64_toolchain.cmake"
QT_HOST_PATH="/home/user/Qt/6.11.3/gcc_64"

echo "Cleaning previous build..."
# Полное удаление папки сборки обязательно!
rm -rf "$BUILD_DIR"
mkdir "$BUILD_DIR"
cd "$BUILD_DIR"

echo "Configuring CMake..."
# Мы передаем QT_HOST_PATH и через файл тулчейна, и через -D для надежности


cmake ../src \
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
    -DCMAKE_BUILD_TYPE=Release \
    -DQT_HOST_PATH="$QT_HOST_PATH" \
    -DCMAKE_INSTALL_PREFIX=./install
    
   # -DCMAKE_FIND_DEBUG_MODE=TRUE \
   # -DQT_DEBUG_FIND_PACKAGE=ON \


echo "Building..."
cmake --build . --parallel $(nproc)

echo "Done."
