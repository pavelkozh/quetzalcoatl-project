#!/bin/bash
echo "GStreamer Logitech C920 Pro HD client"

echo -n "Provide port to listen (e. g. 6667): "
read user_port

echo "Starting stream..."
gst-launch-1.0 -v udpsrc port=$user_port ! application/x-rtp, encoding-name=H264 ! rtph264depay ! avdec_h264 ! videoconvert ! autovideosink