cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(parse_simulate_V1) # TODO: give a real name to your project here

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(SimGrid 3.30.0 REQUIRED) # This template requires SimGrid v3.22
include_directories(${SimGrid_INCLUDE_DIR})


add_subdirectory(parsing_DALiuge_Json) #where the file of library there
include_directories(parsing_DALiuge_Json/include)

foreach( filedir simul_using_json
                 simul_using_deploy
                                   )


    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY Apps/${filedir} ) # Dont use EXECUTABLE_OUTPUT_PATH anymore

    add_executable( ${filedir} src/${filedir}/${filedir}.cpp)
    target_link_libraries(${filedir} ${SimGrid_LIBRARY} parse_Json)

endforeach()

    if(${Tests})

     file(GLOB files "${CMAKE_CURRENT_LIST_DIR}/src/Tests/*.tesh")

     foreach(file  ${files} )

            execute_process(COMMAND python ${CMAKE_CURRENT_LIST_DIR}/../Tesh/tesh
                                            --setenv bindir=Apps/simul_using_json
                                            --setenv graph=${CMAKE_CURRENT_LIST_DIR}/../Graph_json/
                                            --setenv platf=${CMAKE_CURRENT_LIST_DIR}/../platform/
                                            ${file}
            OUTPUT_VARIABLE tests_res
            RESULT_VARIABLE faild )

            get_filename_component(file_name "${file}" NAME)


            if (NOT ${faild})

             message(WARNING "Testing Code " ${file_name} " is PASSED"  )
             #message(STATUS ${tests_res})

            else ()
             message(WARNING "Testing " ${file_name} " is Faild" )
            endif()
     endforeach()

     endif()



