include_directories(.)
include_directories(../cdm/modules)
file(GLOB sources "*.cpp" "*.hpp")
file(GLOB_RECURSE headers "../cdm/modules/*.hpp")

set(CDM_BOOST_ROOT_FOR_TESTING "" CACHE TYPE PATH)
if(CDM_BOOST_ROOT_FOR_TESTING)
	string(REGEX REPLACE "\\\\" "/" CDM_BOOST_ROOT_FOR_TESTING ${CDM_BOOST_ROOT_FOR_TESTING})
endif()
configure_file(boost_root.hpp.in boost_root.hpp ESCAPE_QUOTES @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(system_test ${sources} ${headers} ${CMAKE_CURRENT_BINARY_DIR}/boost_root.hpp)
target_link_libraries(system_test building_configure ${Boost_LIBRARIES})
