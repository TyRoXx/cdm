cmake_minimum_required(VERSION 2.8)
project(using_many_libs)

if(MSVC)
	set(Boost_USE_STATIC_LIBS ON) 
endif()
find_package(Boost REQUIRED filesystem system)
include_directories(${Boost_INCLUDE_DIR})

include_directories(${CATCH_INCLUDE_DIRS})
if(UNIX)
	add_definitions("-std=c++0x")
endif()

include_directories(${RAPIDJSON_INCLUDE_DIR})

include_directories(${RAPIDXML_INCLUDE_DIR})

include_directories(${SQLITE3_INCLUDE_DIRS})

set(CPPNETLIB_PREFIX_PATH "" CACHE TYPE PATH)
set(GTEST_INCLUDE_DIRS "" CACHE TYPE PATH)
set(GTEST_LIBRARIES "" CACHE TYPE STRING)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CPPNETLIB_PREFIX_PATH})
find_package(cppnetlib 0.11.0 REQUIRED)
include_directories(${CPPNETLIB_INCLUDE_DIRS})

include_directories(${WEBSOCKETPP_INCLUDE_DIR})

set(LIBGIT2_INCLUDE_DIR "" CACHE TYPE PATH)
set(LIBGIT2_LIBRARY_DIR "" CACHE TYPE PATH)
if(UNIX)
	find_package(CURL REQUIRED)
	find_package(ZLIB REQUIRED)
	find_package(OpenSSL REQUIRED)
endif()
include_directories(${LIBGIT2_INCLUDE_DIR} ${CURL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})
link_directories(${LIBGIT2_LIBRARY_DIR})

add_executable(using_many_libs using_many_libs.cpp cdm.hpp)
target_link_libraries(using_many_libs ${CPPNETLIB_LIBRARIES} ${Boost_LIBRARIES} ${SQLITE3_LIBRARIES} git2)
if(UNIX)
	target_link_libraries(using_many_libs pthread ${CURL_LIBRARIES} ${ZLIB_LIBRARIES} ${OPENSSL_LIBRARIES} rt dl)
endif()
if(WIN32)
	target_link_libraries(using_many_libs Winhttp Rpcrt4 Crypt32)
endif()
set_target_properties(using_many_libs PROPERTIES FOLDER cdm)
