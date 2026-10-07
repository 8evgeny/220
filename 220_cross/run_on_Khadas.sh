#!/bin/bash

# Set the required environment variables
export QT_DEBUG_PLUGINS=0
export QT_PLUGIN_PATH=/home/khadas/qt-projects/plugins
export GST_PLUGIN_PATH=/home/khadas/QtProg/220/220_cross/prebuild/gst
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/khadas/QtProg/220/220_cross/prebuild/gst
export LD_LIBRARY_PATH=/home/khadas/Qt/6.11.3_arm/gcc_arm64/lib:$LD_LIBRARY_PATH
cd build
./qgst-goen220
