# cmake needs this line
cmake_minimum_required(VERSION 3.30)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(NOT CMAKE_BUILD_TYPE AND NOT (MSVC_IDE OR XCODE))
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type, one of: Release, Debug"
            FORCE)
endif()

# Set target name, change with your favorite name
project(OpenCVapp CXX)

message( STATUS "Build type: ${CMAKE_BUILD_TYPE}" )
message( STATUS "Configuration types: ${CMAKE_CONFIGURATION_TYPES}" )

# Detect 64 or 32 bit
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64-bit project
    SET(64_BIT_OS TRUE)
    message( STATUS "A 64-bit OS detected" )
else()
    SET(64_BIT_OS FALSE)
    message( STATUS "A 32-bit OS detected" )
endif()

# Find OpenCV, you may need to set OpenCV_DIR variable
# to the absolute path to the directory containing OpenCVConfig.cmake file
# via the command line or GUI
if(WIN32)
    set(OpenCV_DIR "D:/_Program Files/opencv/build/x64/vc15/lib")
endif()

if(APPLE)
    # Set SDK path if it's not set correctly
    set(CMAKE_OSX_SYSROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk")
    set(OpenCV_DIR "/opt/homebrew/lib/cmake/opencv4")
endif()

find_package(OpenCV REQUIRED)

# If the package has been found, several variables will
# be set, you can find the full list with descriptions
# in the OpenCVConfig.cmake file.
# Print some message showing some of them
message(STATUS "OpenCV library status:")
message(STATUS " config: ${OpenCV_DIR}")
message(STATUS " version: ${OpenCV_VERSION}")
message(STATUS " libraries: ${OpenCV_LIBS}")
message(STATUS " include path: ${OpenCV_INCLUDE_DIRS}")

file(GLOB srcs *.cpp *.c)
file(GLOB hdrs *.hpp *.h)

include_directories("${CMAKE_CURRENT_LIST_DIR}" ${OpenCV_INCLUDE_DIRS})

# Create the main ImageEnhancement executable
add_executable(ImageEnhancement
    src/main.cpp
    src/metrics.hpp
    src/metrics.cpp
    src/algorithms.hpp
    src/algorithms.cpp
    src/utils.hpp
    src/utils.cpp
)
target_link_libraries(ImageEnhancement ${OpenCV_LIBS})