# Version check
cmake_minimum_required (VERSION 3.0)

# Project
project(SimpleREST)

# Add files
file(GLOB sources "*.cpp")

# Search Directories
include_directories("..")

# Libraries
add_library(SimpleREST STATIC ${sources})

find_package(Boost REQUIRED COMPONENTS
	system
)

find_library(LSIMPLEJSON NAMES SimpleJSON PATHS "../SimpleJSON/build" "SimpleJSON/build" STATIC)
find_library(LSIMPLEXML NAMES SimpleXML PATHS "../SimpleXML/build" "SimpleXML/build" STATIC)

# MS SOCK
if (WIN32)
	find_library(LWS2_32 ws2_32)
	find_library(LMSWSOCK mswsock)
	
	set(LIBS "${LIBS} ${LWS2_32} ${LMSWSOCK}")
	
	message("-- Windows libraries are getting linked")
	message("	${LWS2_32}")
	message("	${LMSWSOCK}")
	
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		target_compile_definitions(SimpleREST PRIVATE BOOST_USE_WINDOWS_H=1)
	endif()
else()
	set(LWS2_32 "")
	set(LMSWSOCK "")
endif()

if(LSIMPLEJSON STREQUAL "LSIMPLEJSON-NOTFOUND")
	set(LSIMPLEJSON "")
else()
	add_definitions(-DSREST_SUPPORT_JSON)
endif()
if(LSIMPLEXML STREQUAL "LSIMPLEXML-NOTFOUND")
	set(LSIMPLEXML "")
else()
	add_definitions(-DSREST_SUPPORT_XML)
endif()

message("-- External libraries")
message("	${LSIMPLEXML}")
message("	${LSIMPLEJSON}")

target_link_libraries(SimpleREST ${LSIMPLEJSON} ${LSIMPLEXML} Boost::system ${LWS2_32} ${LMSWSOCK})

# Compiler Options
target_compile_options(SimpleREST PRIVATE -fexceptions -std=c++14 -O3 -Wall -pedantic-errors -pedantic)