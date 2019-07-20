#!/bin/bash
clear

############ CONFIGURATION ############
# dev_name=/dev/video1
dev_name=/dev/v4l/by-id/usb-046d_HD_Pro_Webcam_C920_91D1FE4F-video-index0

client_IP=127.0.0.1
client_port=4000

cam_width=640
cam_height=480

bitrate_kbps=1024
keyframe_period_ms=1000
############ CONFIGURATION ############

bitrate_bps=$[bitrate_kbps*1000]

echo "Starting stream..."
gst-launch-1.0 -v -e uvch264src device=$dev_name name=src auto-start=true \
        peak-bitrate=$bitrate_bps average-bitrate=$bitrate_bps \
        initial-bitrate=$bitrate_bps iframe-period=$keyframe_period_ms \
    src.vidsrc ! video/x-h264,width=$cam_width,height=$cam_height,framerate=30/1 ! \
    tee name=h \
    h. ! queue ! h264parse ! rtph264pay ! udpsink host=$client_IP port=$client_port \
    h. ! queue ! avdec_h264 ! xvimagesink sync=false
