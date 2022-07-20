cmake_minimum_required(VERSION 3.0.2)
project(quetzalcoatl_software)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  quetzalcoatl_msgs
  realtime_tools
)

catkin_package(
  CATKIN_DEPENDS message_runtime
)

include_directories(
  ${catkin_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIR}
  include
)

set(ROBOT_LAYER_NAME robot_layer)
set(ROBOT_LAYER_SRC 
  src/robot_layer/main.cpp
  src/robot_layer/odom_model.cpp
)

add_executable(${ROBOT_LAYER_NAME}
  ${ROBOT_LAYER_SRC}
)
target_link_libraries(${ROBOT_LAYER_NAME}
  ${catkin_LIBRARIES}
  ${Boost_LIBRARIES} 
  Threads::Threads
)
