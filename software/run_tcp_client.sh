#!/bin/bash

clear

server_IP=192.168.1.55
port=4000

gst-launch-1.0 -v \
    tcpclientsrc host=$server_IP port=$port ! \
    h264parse ! avdec_h264 ! \
    fpsdisplaysink sync=false drop=true
