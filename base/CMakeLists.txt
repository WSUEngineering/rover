cmake_minimum_required(VERSION 3.8)
project(base)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rclcpp REQUIRED)

# uncomment the following section in order to fill in
# further dependencies manually.
find_package(SDL2 REQUIRED)
include_directories(base ${SDL2_INCLUDE_DIRS})

add_executable(gamepad_interface src/gamepad_interface.cpp)
ament_target_dependencies(gamepad_interface rclcpp std_msgs)
target_include_directories(gamepad_interface PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)
target_compile_features(gamepad_interface PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17

target_link_libraries(gamepad_interface ${SDL2_LIBRARIES})

install(TARGETS gamepad_interface
  DESTINATION lib/${PROJECT_NAME})

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
