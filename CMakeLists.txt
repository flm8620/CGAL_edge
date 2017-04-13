project (odt)


cmake_minimum_required(VERSION 2.8.11)
cmake_policy(VERSION 2.8.11)


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)



option(USE_EIGEN "Use Eigen-library" ON)

if(USE_EIGEN)
    add_definitions(-DUSE_EIGEN)
endif(USE_EIGEN)



# Add specific Find.cmake modules 
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ) 


if( NOT WIN32 )
        SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -w")
        ## -Wno-long-long -pedantic -fopenmp -Wfatal-errors  -Wunused-variable -Warray-bounds -lboost_program_options -fpermissive")
endif()

if(USE_EIGEN)
    include_directories($ENV{Eigen3_DIR})
endif(USE_EIGEN)


find_package(CGAL)
include( ${CGAL_USE_FILE} )





file(GLOB SRCS
	"*.h"
	"*.cpp"
)


if( CGAL_FOUND)
    include(${CGAL_USE_FILE})
    # The executable itself.
    add_executable( test ${SRCS} )
   
    # Link with CGAL
    target_link_libraries( test ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})
    
    include_directories(.)

else()
  message(STATUS "ERROR")

endif()
