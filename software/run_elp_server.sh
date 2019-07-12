#!/bin/bash
clear

dev_name=/dev/video2

client_IP=127.0.0.1
client_port=5000

cam_width=800
cam_height=600

cam_width=640
cam_height=360

bitrate=2048

echo "Starting stream..."
gst-launch-1.0 -v -e v4l2src device=$dev_name ! \
    "video/x-h264,width=$cam_width,height=$cam_height,framerate=30/1,bitrate=$bitrate" ! tee name=h \
    h. ! queue ! rtph264pay ! udpsink host=$client_IP port=$client_port \
    h. ! queue ! h264parse ! tcpserversink host=0.0.0.0 port=$client_port recover-policy=keyframe sync-method=latest-keyframe \
    h. ! queue ! avdec_h264 ! fpsdisplaysink sync=false
