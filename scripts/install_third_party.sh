#!/usr/bin/env bash

THIRD_PARTY_DIR=third_party
mkdir -p $THIRD_PARTY_DIR

# ORB-SLAM2

git -C $THIRD_PARTY_DIR clone https://github.com/appliedAI-Initiative/orb_slam_2_ros -b v1.2
git -C $THIRD_PARTY_DIR clone https://github.com/ros-drivers/rosserial.git -b noetic-devel

# Rtabmap

git -C $THIRD_PARTY_DIR clone https://github.com/introlab/rtabmap.git -b 0.20.18-noetic
git -C $THIRD_PARTY_DIR clone https://github.com/introlab/rtabmap_ros.git -b 0.20.18-noetic

git -C $THIRD_PARTY_DIR clone https://github.com/ros-perception/image_pipeline.git -b noetic
# git -C $THIRD_PARTY_DIR clone https://github.com/ros-perception/vision_opencv.git -b noetic

git -C $THIRD_PARTY_DIR clone https://github.com/SteveMacenski/spatio_temporal_voxel_layer.git -b noetic-devel
