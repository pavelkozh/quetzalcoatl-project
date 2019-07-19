#!/bin/bash
clear

# dev_name=/dev/video1
dev_name=/dev/v4l/by-id/usb-046d_HD_Pro_Webcam_C920_91D1FE4F-video-index0

client_port=5000

cam_width=640
cam_height=480

bitrate=512

echo "Starting stream..."
# gst-launch-1.0 -v -e uvch264src device=$dev_name peak-bitrate=1000000 name=src src.vidsrc ! queue ! \
gst-launch-1.0 -v -e v4l2src device=$dev_name ! \
    video/x-h264,width=$cam_width,height=$cam_height,framerate=30/1 ! \
    tee name=h \
    h. ! queue ! h264parse ! tcpserversink host=0.0.0.0 port=$client_port \
    recover-policy=keyframe sync-method=latest-keyframe \
    h. ! queue ! avdec_h264 ! fpsdisplaysink sync=false silent=true


