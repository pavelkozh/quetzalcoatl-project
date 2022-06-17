#!/usr/bin/env bash

export CMAKE_PREFIX_PATH=/usr/local/lib/cmake/:$CMAKE_PREFIX_PATH

catkin build \
    quetzalcoatl_vision \
    quetzalcoatl_software \
    quetzalcoatl_maps

    # quetzalcoatl_description \
    # quetzalcoatl_msgs \
