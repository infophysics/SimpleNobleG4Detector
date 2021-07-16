# Set up the project
cmake_minimum_required (VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
project(Regression)

set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -O3 -gencode arch=compute_75,code=sm_75)


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS RIO)
find_package(CUDA REQUIRED)


if(ROOT_FOUND)
  message("ROOT found: ${ROOT_INCLUDE_DIRS}")
else()
  message(FATAL_ERROR "ROOT not found")
endif()

if(CUDA_FOUND)
  message("CUDA found: ${CUDA_INCLUDE_DIRS}")
else()
  message(FATAL_ERROR "CUDA not found")
endif()

include_directories(${ROOT_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include ${CUDA_INCLUDE_DIRS})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc ${PROJECT_SOURCE_DIR}/src/*.cu)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

cuda_add_executable(Regression Regression.cc ${sources} ${headers})
target_link_libraries(Regression ${ROOT_LIBRARIES})

install(TARGETS Regression DESTINATION bin)
