#!/bin/bash
clear
echo "GStreamer Logitech C920 Pro HD stream server"

echo -n "Provide device name (e .g. /dev/video0):"
read dev_name

echo -n "Provide client IP (e. g. 192.168.1.1):"
read client_IP

echo -n "Provide client port (e. g. 6667):"
read client_port

echo -n "Provide picture width and heigh, space-separated (e. g. 1280 720):"
read cam_width cam_height

echo "Starting stream..."
gst-launch-1.0 -v -e v4l2src device=$dev_name ! queue ! video/x-h264,width=$cam_width,height=$cam_height,framerate=30/1 ! rtph264pay ! udpsink host=$client_IP port=$client_port

