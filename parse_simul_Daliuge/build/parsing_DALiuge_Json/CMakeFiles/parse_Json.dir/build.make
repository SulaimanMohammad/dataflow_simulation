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
CMAKE_SOURCE_DIR = /home/sulaiman/Desktop/intern_document/DAL_simgrid/parse_simul_Daliuge

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/sulaiman/Desktop/intern_document/DAL_simgrid/parse_simul_Daliuge/build

# Include any dependencies generated for this target.
include parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/compiler_depend.make

# Include the progress variables for this target.
include parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/progress.make

# Include the compile flags for this target's objects.
include parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/flags.make

parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/src/parse_Json.cpp.o: parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/flags.make
parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/src/parse_Json.cpp.o: ../parsing_DALiuge_Json/src/parse_Json.cpp
parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/src/parse_Json.cpp.o: parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sulaiman/Desktop/intern_document/DAL_simgrid/parse_simul_Daliuge/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/src/parse_Json.cpp.o"
	cd /home/sulaiman/Desktop/intern_document/DAL_simgrid/parse_simul_Daliuge/build/parsing_DALiuge_Json && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/src/parse_Json.cpp.o -MF CMakeFiles/parse_Json.dir/src/parse_Json.cpp.o.d -o CMakeFiles/parse_Json.dir/src/parse_Json.cpp.o -c /home/sulaiman/Desktop/intern_document/DAL_simgrid/parse_simul_Daliuge/parsing_DALiuge_Json/src/parse_Json.cpp

parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/src/parse_Json.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/parse_Json.dir/src/parse_Json.cpp.i"
	cd /home/sulaiman/Desktop/intern_document/DAL_simgrid/parse_simul_Daliuge/build/parsing_DALiuge_Json && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sulaiman/Desktop/intern_document/DAL_simgrid/parse_simul_Daliuge/parsing_DALiuge_Json/src/parse_Json.cpp > CMakeFiles/parse_Json.dir/src/parse_Json.cpp.i

parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/src/parse_Json.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/parse_Json.dir/src/parse_Json.cpp.s"
	cd /home/sulaiman/Desktop/intern_document/DAL_simgrid/parse_simul_Daliuge/build/parsing_DALiuge_Json && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sulaiman/Desktop/intern_document/DAL_simgrid/parse_simul_Daliuge/parsing_DALiuge_Json/src/parse_Json.cpp -o CMakeFiles/parse_Json.dir/src/parse_Json.cpp.s

# Object files for target parse_Json
parse_Json_OBJECTS = \
"CMakeFiles/parse_Json.dir/src/parse_Json.cpp.o"

# External object files for target parse_Json
parse_Json_EXTERNAL_OBJECTS =

../parsing_DALiuge_Json/lib/libparse_Json.a: parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/src/parse_Json.cpp.o
../parsing_DALiuge_Json/lib/libparse_Json.a: parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/build.make
../parsing_DALiuge_Json/lib/libparse_Json.a: parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/sulaiman/Desktop/intern_document/DAL_simgrid/parse_simul_Daliuge/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library ../../parsing_DALiuge_Json/lib/libparse_Json.a"
	cd /home/sulaiman/Desktop/intern_document/DAL_simgrid/parse_simul_Daliuge/build/parsing_DALiuge_Json && $(CMAKE_COMMAND) -P CMakeFiles/parse_Json.dir/cmake_clean_target.cmake
	cd /home/sulaiman/Desktop/intern_document/DAL_simgrid/parse_simul_Daliuge/build/parsing_DALiuge_Json && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/parse_Json.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/build: ../parsing_DALiuge_Json/lib/libparse_Json.a
.PHONY : parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/build

parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/clean:
	cd /home/sulaiman/Desktop/intern_document/DAL_simgrid/parse_simul_Daliuge/build/parsing_DALiuge_Json && $(CMAKE_COMMAND) -P CMakeFiles/parse_Json.dir/cmake_clean.cmake
.PHONY : parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/clean

parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/depend:
	cd /home/sulaiman/Desktop/intern_document/DAL_simgrid/parse_simul_Daliuge/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sulaiman/Desktop/intern_document/DAL_simgrid/parse_simul_Daliuge /home/sulaiman/Desktop/intern_document/DAL_simgrid/parse_simul_Daliuge/parsing_DALiuge_Json /home/sulaiman/Desktop/intern_document/DAL_simgrid/parse_simul_Daliuge/build /home/sulaiman/Desktop/intern_document/DAL_simgrid/parse_simul_Daliuge/build/parsing_DALiuge_Json /home/sulaiman/Desktop/intern_document/DAL_simgrid/parse_simul_Daliuge/build/parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : parsing_DALiuge_Json/CMakeFiles/parse_Json.dir/depend

