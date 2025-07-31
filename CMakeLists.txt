cmake_minimum_required(VERSION 3.5)
project(trx)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

#-- Dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclpy REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(hardware_interface REQUIRED)

ament_export_dependencies(rosidl_default_runtime)

add_library(seed_command
  src/seed_command.cpp
)
set_target_properties(seed_command PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(seed_command PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

add_library(seed_hardware_interface SHARED src/seed_hardware_interface.cpp)
ament_target_dependencies(seed_hardware_interface
  rclcpp
  std_msgs
  hardware_interface
)
target_link_libraries(seed_hardware_interface
  seed_command
)

install(TARGETS seed_hardware_interface seed_command
  DESTINATION lib
)

install(DIRECTORY
  config
  urdf
  launch
  DESTINATION share/${PROJECT_NAME}/
)

install(FILES seed_hardware_interface_plugin.xml
  DESTINATION share/${PROJECT_NAME}
)

pluginlib_export_plugin_description_file(hardware_interface seed_hardware_interface_plugin.xml)
add_subdirectory(controller/trx_joint_trajectory_controller)
install(FILES trx.rviz
  DESTINATION share/${PROJECT_NAME}
)
ament_package()
