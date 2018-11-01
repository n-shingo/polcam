cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Project Name
project(polcam)

# ArenSDK Path
set(ArenaDir ~/ArenaSDK/ArenaSDK_Linux_x64)

# Include Path
include_directories(${ArenaDir}/include/Arena)
include_directories(${ArenaDir}/GenICam/library/CPP/include)

# Libarary Path
link_directories(${AreanDir}/lib64)
link_directories(${AreanDir}/GenICam/library/lib/Linux64_x64)

# Bin Directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# Compile for release
set(CMAKE_BILD_TYPE Release)


find_package(OpenCV REQUIRED)
add_executable(polcam ./src/polcam.cpp ./src/polcam_funcs.cpp)
target_link_libraries( polcam arena gentl GCBase_gcc421_v3_0 GenApi_gcc421_v3_0 ${OpenCV_LIBRARIES})

