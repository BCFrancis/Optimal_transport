include_directories(${ARGOS_QTOPENGL_INCLUDE_DIRS})
add_library(trajectory_loop_functions MODULE 
  trajectory_loop_functions.h
  trajectory_loop_functions.cpp)

add_library(box_trajectory_loop_functions MODULE 
  box_trajectory_loop_functions.h
  box_trajectory_loop_functions.cpp)

target_link_libraries(trajectory_loop_functions
  argos3core_simulator
  argos3plugin_simulator_entities
  argos3plugin_simulator_footbot
  argos3plugin_simulator_qtopengl
  ${ARGOS_QTOPENGL_LIBRARIES})
