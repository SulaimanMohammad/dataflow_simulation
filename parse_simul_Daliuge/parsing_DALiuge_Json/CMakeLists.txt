cmake_minimum_required(VERSION 3.23)

include_directories(include)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/lib)
add_library(parse_Json ./src/parse_Json.cpp ./include/parse_Json.h)

target_include_directories(parse_Json PUBLIC "${CMAKE_INCLUDE_CURRENT_DIR}") # make it public seen by the root

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/Test)
add_executable(parsing_print_example ./Test/parsing_print_example.cpp)
target_link_libraries(parsing_print_example parse_Json )

# tests will not pass until you "make" to have executable becuse they need it  
# after cmake -DTests=1 to lunch tests
# if(${Tests})
#     execute_process(COMMAND python ${CMAKE_CURRENT_LIST_DIR}/../../Tesh/tesh
#                                         --setenv bindir=${CMAKE_CURRENT_LIST_DIR}/Test
#                                         --setenv graph=${CMAKE_CURRENT_LIST_DIR}/../../Graph_json/
#                                         ${CMAKE_CURRENT_LIST_DIR}/Test/parsing_example.tesh
#     OUTPUT_VARIABLE tests_res
#     RESULT_VARIABLE faild
#     )

#     if (NOT ${faild})
#         message(WARNING "Testing the library is PASSED"  )
#     else() 
#         message(WARNING "Testing the library is Faild"  )
#     endif()
   
# endif()
