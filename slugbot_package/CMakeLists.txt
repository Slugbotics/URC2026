cmake_minimum_required(VERSION 3.5)
project(slugbot_package)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

# Besides the package specific dependencies we also need the `pluginlib` and `webots_ros2_driver`
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(pluginlib REQUIRED)
find_package(webots_ros2_driver REQUIRED)
find_package(messages REQUIRED)

# Export the plugin configuration file
pluginlib_export_plugin_description_file(webots_ros2_driver slugbot_driver.xml)

# Obstacle avoider
include_directories(
  include
)
add_executable(obstacle_avoider
  src/ObstacleAvoider.cpp
)
ament_target_dependencies(obstacle_avoider
  rclcpp
  geometry_msgs
  sensor_msgs
)
install(TARGETS
  obstacle_avoider
  DESTINATION lib/${PROJECT_NAME}
)
install(
  DIRECTORY include/
  DESTINATION include
)

# Build the Simulation plugin as a shared library (pluginlib)
add_library(
  ${PROJECT_NAME}
  SHARED
  src/Simulation.cpp
)
target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
  include
)
ament_target_dependencies(
  ${PROJECT_NAME}
  pluginlib
  rclcpp
  webots_ros2_driver
  std_msgs
)
install(TARGETS
  ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

# SlugbotDriver node executable
add_executable(slugbot_driver_node src/SlugbotDriver.cpp)
ament_target_dependencies(slugbot_driver_node
  rclcpp
  std_msgs
  geometry_msgs
  messages
  sensor_msgs
)
install(TARGETS
  slugbot_driver_node
  DESTINATION lib/${PROJECT_NAME}
)
# Install additional directories.
install(DIRECTORY
  launch
  resource
  worlds
  protos
  DESTINATION share/${PROJECT_NAME}/
)
ament_export_include_directories(
  include
)
ament_export_libraries(
  ${PROJECT_NAME}
)
ament_package()