#!/bin/bash

/home/khadas/load_isp.sh

# Set the required environment variables
export QT_DEBUG_PLUGINS=0
export QT_PLUGIN_PATH=/home/khadas/qt-projects/plugins
export GST_PLUGIN_PATH=/home/khadas/gst
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/khadas/gst

#./qgst-goen220