#!/usr/bin/env bash

export CMAKE_PREFIX_PATH=/usr/local/lib/cmake/:$CMAKE_PREFIX_PATH

catkin build \
    quetzalcoatl_software \
    quetzalcoatl_vision \
    quetzalcoatl_maps \
    quetzalcoatl_msgs \
    quetzalcoatl_description \
    # maddrive_urdf_tools \
    # --cmake-args -DOpenCV_DIR="/usr/local/lib/cmake/opencv4" 

    # rtabmap \
    # rtabmap_ros \
    # stereo_image_proc \
    # spatio_temporal_voxel_layer \
    # camera_calibration \
    # ackermann_controller_plugin \
