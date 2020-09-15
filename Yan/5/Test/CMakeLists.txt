cmake_minimum_required(VERSION 3.10)

project(${PROJECT_NAME} vtest)

aux_source_directory(. DIR_SRCS)
add_executable(${PROJECT_NAME} ${DIR_SRCS})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)

#target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -pedantic -Wshadow -Wreturn-type)

#target_link_libraries(${PROJECT_NAME} PUBLIC)

#add_definitions(-D_WINDOWS_PLATFORM)