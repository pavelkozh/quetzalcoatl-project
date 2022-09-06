#!/usr/bin/env bash

export CMAKE_PREFIX_PATH=/usr/local/lib/cmake/:$CMAKE_PREFIX_PATH

catkin build \
    quetzalcoatl_software \
    quetzalcoatl_vision \
    quetzalcoatl_maps \
    quetzalcoatl_msgs \
    quetzalcoatl_description