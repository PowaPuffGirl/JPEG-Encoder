# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

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
CMAKE_COMMAND = /home/buttercup/Clion/clion-2018.2.2/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/buttercup/Clion/clion-2018.2.2/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/buttercup/git/MedienInfo

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/buttercup/git/MedienInfo/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/MedienInfo.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/MedienInfo.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/MedienInfo.dir/flags.make

CMakeFiles/MedienInfo.dir/main.cpp.o: CMakeFiles/MedienInfo.dir/flags.make
CMakeFiles/MedienInfo.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/buttercup/git/MedienInfo/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/MedienInfo.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MedienInfo.dir/main.cpp.o -c /home/buttercup/git/MedienInfo/main.cpp

CMakeFiles/MedienInfo.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MedienInfo.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/buttercup/git/MedienInfo/main.cpp > CMakeFiles/MedienInfo.dir/main.cpp.i

CMakeFiles/MedienInfo.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MedienInfo.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/buttercup/git/MedienInfo/main.cpp -o CMakeFiles/MedienInfo.dir/main.cpp.s

# Object files for target MedienInfo
MedienInfo_OBJECTS = \
"CMakeFiles/MedienInfo.dir/main.cpp.o"

# External object files for target MedienInfo
MedienInfo_EXTERNAL_OBJECTS =

MedienInfo: CMakeFiles/MedienInfo.dir/main.cpp.o
MedienInfo: CMakeFiles/MedienInfo.dir/build.make
MedienInfo: CMakeFiles/MedienInfo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/buttercup/git/MedienInfo/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable MedienInfo"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MedienInfo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/MedienInfo.dir/build: MedienInfo

.PHONY : CMakeFiles/MedienInfo.dir/build

CMakeFiles/MedienInfo.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/MedienInfo.dir/cmake_clean.cmake
.PHONY : CMakeFiles/MedienInfo.dir/clean

CMakeFiles/MedienInfo.dir/depend:
	cd /home/buttercup/git/MedienInfo/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/buttercup/git/MedienInfo /home/buttercup/git/MedienInfo /home/buttercup/git/MedienInfo/cmake-build-debug /home/buttercup/git/MedienInfo/cmake-build-debug /home/buttercup/git/MedienInfo/cmake-build-debug/CMakeFiles/MedienInfo.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/MedienInfo.dir/depend

