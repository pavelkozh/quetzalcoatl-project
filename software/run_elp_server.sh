#!/bin/bash
clear

# dev_name=/dev/video2
dev_name=/dev/v4l/by-id/usb-HD_USB_Camera_HD_USB_Camera_SN0001-video-index1

client1_IP=10.139.1.156
client2_IP=10.139.1.134
client_port=4000

cam_width=640
cam_height=360

bitrate=2048

echo "Starting stream..."
gst-launch-1.0 -v -e v4l2src device=$dev_name ! \
    video/x-h264,width=$cam_width,height=$cam_height,framerate=30/1,bitrate=$bitrate ! tee name=h \
    h. ! queue ! rtph264pay ! udpsink host=$client1_IP port=$client_port \
    h. ! queue ! rtph264pay ! udpsink host=$client2_IP port=$client_port \
    h. ! queue ! rtph264pay ! udpsink host=127.0.0.1 port=$client_port \
    h. ! queue ! avdec_h264 ! fpsdisplaysink sync=false \
    h. ! queue ! h264parse ! video/x-h264,stream-format=avc ! tcpserversink host=0.0.0.0 port=$client_port recover-policy=keyframe sync-method=latest-keyframe

