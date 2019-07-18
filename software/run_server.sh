#!/bin/bash
clear

# dev_name=/dev/video1
dev_name=/dev/v4l/by-id/usb-046d_HD_Pro_Webcam_C920_91D1FE4F-video-index0

client_IP=10.139.1.134
client_port=5000

cam_width=800
cam_height=600

bitrate=1024

echo "Starting stream..."
gst-launch-1.0 -v -e v4l2src device=$dev_name ! \
    video/x-h264,width=$cam_width,height=$cam_height,framerate=30/1,bitrate=$bitrate ! \
    tee name=h \
    h. ! queue ! avdec_h264 ! fpsdisplaysink sync=false \
    h. ! queue ! h264parse ! tcpserversink host=0.0.0.0 port=$client_port recover-policy=keyframe sync-method=latest-keyframe

#    h. ! queue ! h264parse ! rtph264pay ! udpsink host=$client_IP port=$client_port \

