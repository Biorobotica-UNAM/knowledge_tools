cmake_minimum_required(VERSION 3.8)
project(knowledge_tools)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ============================================================
# Dependencies
# ============================================================

find_package(ament_cmake REQUIRED)

find_package(rclcpp REQUIRED)
find_package(std_srvs REQUIRED)
find_package(knowledge_interfaces REQUIRED)
find_package(nlohmann_json REQUIRED)

# ============================================================
# Library
# ============================================================

add_library(${PROJECT_NAME} SHARED
  src/Knowledge.cpp
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  std_srvs
  knowledge_interfaces
  nlohmann_json
)

target_link_libraries(${PROJECT_NAME}
  nlohmann_json::nlohmann_json
)

# ============================================================
# Install
# ============================================================

install(
  TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(
  DIRECTORY include/
  DESTINATION include
)

install(
  DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

# ============================================================
# Export
# ============================================================

ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)

ament_export_dependencies(
  rclcpp
  std_srvs
  knowledge_interfaces
  nlohmann_json
)

# ============================================================
# Testing
# ============================================================

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)

  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)

  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
