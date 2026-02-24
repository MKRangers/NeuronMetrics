cmake_minimum_required(VERSION 3.21)

project(NeuronMetrics VERSION 0.1.0 LANGUAGES CXX)

# Use modern C++ standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create static library target
add_library(NeuronMetrics STATIC
    src/nmSWCReader.cpp
    include/io/nmSWCReader.h
)

# Include directories for the library
target_include_directories(NeuronMetrics 
    PUBLIC # Expose the include directory to users of the library
        ${CMAKE_CURRENT_SOURCE_DIR}/include/io
)

# Find Boost libraries (if needed for the project)
set(BOOST_ROOT "D:/boostLibs/boost_1_80_0_msvc143/" CACHE PATH "Boost root")
find_package(Boost 1.80 REQUIRED)

# Link the Boost libraries to NeuronMetrics
target_link_libraries(NeuronMetrics
    PUBLIC Boost::boost
)

# Add benchmarking subdirectory
add_subdirectory(benchmark)

# Set the installation prefix to a local directory within the project
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install
    CACHE PATH "Installation directory" FORCE
)