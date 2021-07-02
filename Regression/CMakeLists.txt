# Set up the project
cmake_minimum_required (VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
project(Regression)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS RIO)
if(ROOT_FOUND)
  message("ROOT found: ${ROOT_INCLUDE_DIRS}")
else()
  message(FATAL_ERROR "ROOT not found")
endif()
include_directories(${ROOT_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(Regression Regression.cc ${sources} ${headers})
target_link_libraries(Regression ${ROOT_LIBRARIES})

install(TARGETS Regression DESTINATION bin)
