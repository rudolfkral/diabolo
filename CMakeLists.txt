cmake_minimum_required (VERSION 2.8)
   
project (Diabolo)
     
set (EXECUTABLE_OUTPUT_PATH "${Diabolo_BINARY_DIR}/bin")
include_directories (${CMAKE_CURRENT_BINARY_DIR})
find_package (Qt4 REQUIRED)
include (${QT_USE_FILE})
add_definitions (${QT_DEFINITIONS})

add_subdirectory (src)