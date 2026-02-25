cmake_minimum_required(VERSION 3.21)

project(NeuronMetrics VERSION 0.1.0 LANGUAGES CXX)

# Use modern C++ standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create static library target
add_library(NeuronMetrics STATIC
    src/nmNeuron.cpp
    src/nmNode.cpp
    src/nmSegment.cpp
    src/nmMouseCCF.cpp

    include/nmNeuron.h
    include/nmNode.h
    include/nmSegment.h
    include/nmMouseCCF.h
    include/nmExceptions.h
)

# Include directories for the library
target_include_directories(NeuronMetrics 
    PUBLIC # Expose the include directory to users of the library
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Find Boost libraries (if needed for the project)
set(BOOST_ROOT "D:/boostLibs/boost_1_80_0_msvc143/" CACHE PATH "Boost root")
find_package(Boost 1.80 REQUIRED)

# Find TIFF library
set(TIFF_INCLUDE_DIR "D:/tiff-4.4.0/include" CACHE PATH "TIFF include directory")
set(TIFF_LIBRARY_RELEASE "D:/tiff-4.4.0/lib/tiff.lib" CACHE FILEPATH "TIFF release library")
set(TIFF_LIBRARY_DEBUG "D:/tiff-4.4.0/lib/tiffd.lib" CACHE FILEPATH "TIFF debug library")
find_package(TIFF REQUIRED)

# Include directories for TIFF
target_include_directories(NeuronMetrics
    PRIVATE ${TIFF_INCLUDE_DIR}
)

# Link libraries to NeuronMetrics
target_link_libraries(NeuronMetrics
    PUBLIC Boost::boost
    PRIVATE
        optimized ${TIFF_LIBRARY_RELEASE}
        debug ${TIFF_LIBRARY_DEBUG}
)

# Add benchmarking subdirectory
add_subdirectory(benchmark)

# Set the installation prefix to a local directory within the project
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install
    CACHE PATH "Installation directory" FORCE
)