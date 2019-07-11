#!/bin/bash

clear

user_port=5000

# echo "Starting stream..."
gst-launch-1.0 -v udpsrc port=$user_port ! \
    application/x-rtp, encoding-name=H264 ! \
    rtph264depay ! avdec_h264 ! \
    videoconvert ! fpsdisplaysink sync=false