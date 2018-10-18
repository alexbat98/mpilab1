cmake_minimum_required(VERSION 3.9)
project(mpilab1)

set(CMAKE_CXX_STANDARD 17)

find_package(MPI REQUIRED)
find_package(OpenMP)

add_executable(mpilab1_v1 main.cpp Matrix.cpp)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})
target_link_libraries(mpilab1_v1 PUBLIC ${MPI_CXX_LIBRARIES})

add_executable(mpilab1_v2 ver2.cpp Matrix.cpp)
target_link_libraries(mpilab1_v2 PUBLIC ${MPI_CXX_LIBRARIES})

add_executable(mpilab1_v3 ver3.cpp)
target_link_libraries(mpilab1_v3 PUBLIC ${MPI_CXX_LIBRARIES})

if (OPENMP_FOUND)
    target_link_libraries(mpilab1_v1 PUBLIC ${OpenMP_CXX_LIBRARIES})
    target_link_libraries(mpilab1_v2 PUBLIC ${OpenMP_CXX_LIBRARIES})
endif()

add_executable(singlethread singlethread.cpp)