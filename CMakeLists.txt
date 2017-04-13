project (edge)


cmake_minimum_required(VERSION 2.8.11)
cmake_policy(VERSION 2.8.11)



find_package(CGAL)
include( ${CGAL_USE_FILE} )

file(GLOB SRCS
	"*.h"
	"*.cpp"
)


if(CGAL_FOUND)
    include(${CGAL_USE_FILE})
    add_executable( test ${SRCS} )
    target_link_libraries( test ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})
    include_directories(.)
else()
  message(STATUS "ERROR")
endif()
