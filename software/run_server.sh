#!/bin/bash
clear

dev_name=/dev/video1

client_IP=127.0.0.1
client_port=5000

cam_width=800
cam_height=600

echo "Starting stream..."
gst-launch-1.0 -v -e v4l2src device=$dev_name ! \
    video/x-h264,width=$cam_width,height=$cam_height,framerate=30/1,bitrate=2048 ! \
    h264parse ! rtph264pay ! udpsink host=$client_IP port=$client_port
    # avdec_h264 ! fpsdisplaysink sync=false
