#!/usr/bin/env bash

export CMAKE_PREFIX_PATH=/usr/local/lib/cmake/:$CMAKE_PREFIX_PATH

catkin build \
    quetzalcoatl_software \
    quetzalcoatl_vision \
    quetzalcoatl_maps \
    rtabmap \
    rtabmap_ros \
    stereo_image_proc \
    spatio_temporal_voxel_layer \
    camera_calibration \
    --cmake-args -DOpenCV_DIR="/usr/local/lib/cmake/opencv4" 
    