# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/sulaiman/Desktop/simgrid/simgid_tutorial/master_forwarder_workers_depend

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/sulaiman/Desktop/simgrid/simgid_tutorial/master_forwarder_workers_depend

# Include any dependencies generated for this target.
include CMakeFiles/simu_funcl.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/simu_funcl.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/simu_funcl.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/simu_funcl.dir/flags.make

CMakeFiles/simu_funcl.dir/simu_funcl.cpp.o: CMakeFiles/simu_funcl.dir/flags.make
CMakeFiles/simu_funcl.dir/simu_funcl.cpp.o: simu_funcl.cpp
CMakeFiles/simu_funcl.dir/simu_funcl.cpp.o: CMakeFiles/simu_funcl.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sulaiman/Desktop/simgrid/simgid_tutorial/master_forwarder_workers_depend/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/simu_funcl.dir/simu_funcl.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/simu_funcl.dir/simu_funcl.cpp.o -MF CMakeFiles/simu_funcl.dir/simu_funcl.cpp.o.d -o CMakeFiles/simu_funcl.dir/simu_funcl.cpp.o -c /home/sulaiman/Desktop/simgrid/simgid_tutorial/master_forwarder_workers_depend/simu_funcl.cpp

CMakeFiles/simu_funcl.dir/simu_funcl.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/simu_funcl.dir/simu_funcl.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sulaiman/Desktop/simgrid/simgid_tutorial/master_forwarder_workers_depend/simu_funcl.cpp > CMakeFiles/simu_funcl.dir/simu_funcl.cpp.i

CMakeFiles/simu_funcl.dir/simu_funcl.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/simu_funcl.dir/simu_funcl.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sulaiman/Desktop/simgrid/simgid_tutorial/master_forwarder_workers_depend/simu_funcl.cpp -o CMakeFiles/simu_funcl.dir/simu_funcl.cpp.s

# Object files for target simu_funcl
simu_funcl_OBJECTS = \
"CMakeFiles/simu_funcl.dir/simu_funcl.cpp.o"

# External object files for target simu_funcl
simu_funcl_EXTERNAL_OBJECTS =

simu_funcl: CMakeFiles/simu_funcl.dir/simu_funcl.cpp.o
simu_funcl: CMakeFiles/simu_funcl.dir/build.make
simu_funcl: /opt/simgrid/lib/libsimgrid.so
simu_funcl: CMakeFiles/simu_funcl.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/sulaiman/Desktop/simgrid/simgid_tutorial/master_forwarder_workers_depend/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable simu_funcl"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/simu_funcl.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/simu_funcl.dir/build: simu_funcl
.PHONY : CMakeFiles/simu_funcl.dir/build

CMakeFiles/simu_funcl.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/simu_funcl.dir/cmake_clean.cmake
.PHONY : CMakeFiles/simu_funcl.dir/clean

CMakeFiles/simu_funcl.dir/depend:
	cd /home/sulaiman/Desktop/simgrid/simgid_tutorial/master_forwarder_workers_depend && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sulaiman/Desktop/simgrid/simgid_tutorial/master_forwarder_workers_depend /home/sulaiman/Desktop/simgrid/simgid_tutorial/master_forwarder_workers_depend /home/sulaiman/Desktop/simgrid/simgid_tutorial/master_forwarder_workers_depend /home/sulaiman/Desktop/simgrid/simgid_tutorial/master_forwarder_workers_depend /home/sulaiman/Desktop/simgrid/simgid_tutorial/master_forwarder_workers_depend/CMakeFiles/simu_funcl.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/simu_funcl.dir/depend

