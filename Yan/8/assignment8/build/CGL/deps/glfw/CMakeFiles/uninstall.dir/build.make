# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = E:/cmake-3.14.3-win64-x64/bin/cmake.exe

# The command to remove a file.
RM = E:/cmake-3.14.3-win64-x64/bin/cmake.exe -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = F:/Yan/8/assignment8

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = F:/Yan/8/assignment8/build

# Utility rule file for uninstall.

# Include the progress variables for this target.
include CGL/deps/glfw/CMakeFiles/uninstall.dir/progress.make

CGL/deps/glfw/CMakeFiles/uninstall:
	cd F:/Yan/8/assignment8/build/CGL/deps/glfw && E:/cmake-3.14.3-win64-x64/bin/cmake.exe -P F:/Yan/8/assignment8/build/CGL/deps/glfw/cmake_uninstall.cmake

uninstall: CGL/deps/glfw/CMakeFiles/uninstall
uninstall: CGL/deps/glfw/CMakeFiles/uninstall.dir/build.make

.PHONY : uninstall

# Rule to build all files generated by this target.
CGL/deps/glfw/CMakeFiles/uninstall.dir/build: uninstall

.PHONY : CGL/deps/glfw/CMakeFiles/uninstall.dir/build

CGL/deps/glfw/CMakeFiles/uninstall.dir/clean:
	cd F:/Yan/8/assignment8/build/CGL/deps/glfw && $(CMAKE_COMMAND) -P CMakeFiles/uninstall.dir/cmake_clean.cmake
.PHONY : CGL/deps/glfw/CMakeFiles/uninstall.dir/clean

CGL/deps/glfw/CMakeFiles/uninstall.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" F:/Yan/8/assignment8 F:/Yan/8/assignment8/CGL/deps/glfw F:/Yan/8/assignment8/build F:/Yan/8/assignment8/build/CGL/deps/glfw F:/Yan/8/assignment8/build/CGL/deps/glfw/CMakeFiles/uninstall.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CGL/deps/glfw/CMakeFiles/uninstall.dir/depend

