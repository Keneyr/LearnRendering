cmake_minimum_required(VERSION 3.10)
project(RayTracing)

set(CMAKE_CXX_STANDARD 17)
set (CMAKE_FIND_ROOT_PATH C:/MinGW)

#message(STATUS "Checking OpenMP")
#find_package(OpenMP)
#IF(OPENMP_FOUND)
    #message("Found OpenMP! ^_^")
    # add flags for OpenMP
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    #set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${OpenMP_SHARED_LINKER_FLAGS}")
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread")
    #target_link_libraries(RayTracing PUBLIC OpenMP::OpenMP_CXX)
#ELSE()
    #message("Missed OpenMP! x_x")
#ENDIF()


set(SOURCE_FILES main.cpp Object.hpp Vector.cpp Vector.hpp Sphere.hpp global.hpp Triangle.hpp Scene.cpp
Scene.hpp Light.hpp AreaLight.hpp BVH.cpp BVH.hpp Bounds3.hpp Ray.hpp Material.hpp Intersection.hpp
Renderer.cpp Renderer.hpp)

add_executable(RayTracing ${SOURCE_FILES})
