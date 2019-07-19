#!/bin/bash
clear

# dev_name=/dev/video2
dev_name=/dev/v4l/by-id/usb-USB_Developer_Android_20080411-video-index0

client_port=4000

# Only possible setup for android
cam_width=640
cam_height=480

bitrate=512

echo "Starting stream..."
gst-launch-1.0 -v v4l2src device=$dev_name ! \
    image/jpeg,width=1280,height=720,framerate=30/1 ! \
    jpegparse ! jpegdec ! \
    videoscale ! video/x-raw,width=$cam_width,height=$cam_height ! \
    tee name=h \
    h. ! queue ! \
    x264enc tune=zerolatency bitrate=$bitrate speed-preset=superfast ! \
    tcpserversink host=0.0.0.0 port=$client_port \
    recover-policy=keyframe sync=false sync-method=latest-keyframe \
    h. ! queue ! fpsdisplaysink sync=false silent=true \
