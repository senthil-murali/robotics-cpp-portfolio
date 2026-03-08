cmake_minimum_required(VERSION 3.16)
project(obstacle_detector)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)

# Build as shared library (component node)
add_library(obstacle_detector_node SHARED
  src/obstacle_detector_node.cpp
)
ament_target_dependencies(obstacle_detector_node
  rclcpp
  rclcpp_lifecycle
  rclcpp_components
  sensor_msgs
  std_msgs
)

# Register as component
rclcpp_components_register_node(obstacle_detector_node
  PLUGIN "ObstacleDetector"
  EXECUTABLE obstacle_detector_exec
)

# Install targets
install(TARGETS obstacle_detector_node
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY launch/
  DESTINATION share/${PROJECT_NAME}/launch
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
