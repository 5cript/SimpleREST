# Version check
cmake_minimum_required (VERSION 3.0)

# Project
project(SimpleREST)

# Add files
file(GLOB sources "*.cpp")

# Libraries
add_library(SimpleREST STATIC ${sources})

find_package(Boost REQUIRED COMPONENTS
	system
)

find_library(LATOMIC atomic)
find_library(LSIMPLEJSON NAMES SimpleJSON PATHS "../SimpleJSON/build" "SimpleJSON/build" STATIC)
find_library(LPTHREAD pthread)

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

if(LATOMIC STREQUAL "LATOMIC-NOTFOUND")
	set(LATOMIC "")
endif()
if(LPTHREAD STREQUAL "LPTHREAD-NOTFOUND")
	set(LPTHREAD "")
endif()

target_link_libraries(SimpleREST Boost::system ${LWS2_32} ${LMSWSOCK} ${LATOMIC} ${LPTHREAD})

# Compiler Options
target_compile_options(SimpleREST PRIVATE -fexceptions -std=c++14 -O3 -Wall -pedantic-errors -pedantic)