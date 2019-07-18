#!/bin/bash

clear

server_IP=127.0.0.1
port=5000

# echo "Starting stream..."
gst-launch-1.0 -v \
    tcpclientsrc host=$server_IP port=$port ! \
    h264parse ! avdec_h264 ! \
    fpsdisplaysink sync=false drop=true
