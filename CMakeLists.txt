cmake_minimum_required(VERSION 3.0.2)
project(bcr_leading)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
roscpp
rospy
std_msgs
geometry_msgs
message_generation
)


find_package(cmake_modules REQUIRED)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Eigen3 REQUIRED)

include_directories(${Eigen3_INCLUDE_DIRS})

add_definitions(${EIGEN_DEFINITIONS})
add_message_files(
  DIRECTORY msg
  FILES
  four.msg
  two.msg

)
generate_messages(
   DEPENDENCIES
   std_msgs
)

catkin_package(
  INCLUDE_DIRS include
  CATKIN_DEPENDS roscpp std_msgs message_runtime
  LIBRARIES 
  DEPENDS system_lib Eigen3
)
###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
    include
    ${Boost_INCLUDE_DIR} ${catkin_INCLUDE_DIRS}
    /usr/xenomai/include
    /usr/xenomai/include/cobalt 
)

link_directories(
    ${catkin_LIB_DIRS} lib
    /usr/xenomai/lib
)

link_libraries(
    alchemy
    cobalt	
    pthread  
)

execute_process(COMMAND /usr/xenomai/bin/xeno-config --posix --alchemy --cflags OUTPUT_VARIABLE XENO_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND /usr/xenomai/bin/xeno-config --posix --alchemy --ldflags OUTPUT_VARIABLE XENO_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}${XENO_CFLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${XENO_LDFLAGS}")


## Declare a C++ library
add_executable(bcr_remote src/bcr_remote.cpp)
add_dependencies(bcr_remote ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(bcr_remote ${catkin_LIBRARIES} )

add_executable(main_bcr_leader src/main_bcr_leader.cpp)
add_dependencies(main_bcr_leader ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(main_bcr_leader ${catkin_LIBRARIES} )
