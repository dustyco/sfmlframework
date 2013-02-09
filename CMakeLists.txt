

cmake_minimum_required(VERSION 2.8)

project(mingwtest CXX C)


# Build in GNU profiling
# Other compiler id's: Clang, GNU, Intel, MSVC
if(PROFILE)
#	if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
#		message("Building in C GNU profiling")
#		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
#	else()
#		message("Requested C profiling but we don't have it for this compiler: " ${CMAKE_C_COMPILER_ID})
#	endif()
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		message("Building in CXX GNU profiling")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
	else()
		message("Requested CXX profiling but we don't have it for this compiler: " ${CMAKE_CXX_COMPILER_ID})
	endif()
endif()

# Subdirectories can create source file groups
# Usage: add_sources([group name] [source file name]...)
#        After, call propagate_groups() once in each directory
#        The variable [group name]_SRC will contain the source file paths
macro(add_sources)
	set(_arguments ${ARGN})
	list(GET _arguments 0 _group)
	list(REMOVE_AT _arguments 0)
	list(APPEND _groups ${_group})
	file(RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
	foreach(_src ${_arguments})
		if(_relPath)
			list(APPEND ${_group}_SRC "${_relPath}/${_src}")
		else()
			list(APPEND ${_group}_SRC "${_src}")
		endif()
	endforeach()
endmacro()

# Subdirectories can create external library groups
# Usage: require_libraries([group name] [library name]...)
#        After, call propagate_groups() once in each directory
#        The variable [group name]_LIB will contain the library names
macro(require_libraries)
	set(_arguments ${ARGN})
	list(GET _arguments 0 _group)
	list(REMOVE_AT _arguments 0)
	list(APPEND _groups ${_group})
	foreach(_lib ${_arguments})
		list(APPEND ${_group}_LIB "${_lib}")
	endforeach()
endmacro()

# Call this in each subdirectory AFTER all group related calls
macro(propagate_groups)
	file(RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
	if(_relPath)
		set(_groups ${_groups} PARENT_SCOPE)
		foreach(_group ${_groups})
			set(${_group}_SRC ${${_group}_SRC} PARENT_SCOPE)
			set(${_group}_LIB ${${_group}_LIB} PARENT_SCOPE)
		endforeach()
	endif()
endmacro()


# Decide where to look
set(BOOST_ROOT_ENV $ENV{BOOST_ROOT})
if(BOOST_ROOT_ENV)
	# Get boost from a specifically defined folder
	set(BOOST_ROOT $ENV{BOOST_ROOT})
	message("BOOST_ROOT: " ${BOOST_ROOT})
endif()

# Specify which linkable libraries we should find
set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
if (WIN32)
	# Mingw32 makes it a different name unless you set this
	set(Boost_THREADAPI win32)
else ()
	set(Boost_THREADAPI pthread)
endif ()
set(BOOST_COMPONENTS
	system
	thread
	chrono
)

# Find it
find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})
include_directories(${Boost_INCLUDE_DIR})
add_definitions(-DBOOST_ALL_NO_LIB)
add_definitions(-DBOOST_THREAD_USE_LIB)
if (WIN32)
	set(Boost_THREAD_LIBRARY wsock32 ${Boost_THREAD_LIBRARY})
else ()
	set(Boost_THREAD_LIBRARY pthread ${Boost_THREAD_LIBRARY})
endif ()


# Find OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
message(" OPENGL_LIBRARIES: ${OPENGL_LIBRARIES}")
message("OPENGL_gl_LIBRARY: ${OPENGL_gl_LIBRARY}")


# Find SFML
#set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML REQUIRED COMPONENTS system window graphics)
include_directories(${SFML_INCLUDE_DIR})
set(SFML_LIBRARIES ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${SFML_LIBRARIES})
message("   SFML_LIBRARIES: ${SFML_LIBRARIES}")


# Work through the source tree
add_subdirectory(src)

# So I don't have to distribute the compiler libraries that windows doesn't come with
if (MINGW)
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BAR_CXX_FLAGS} -static-libgcc -static-libstdc++")
endif ()

# Game client executable
add_executable(test ${test_SRC})
target_link_libraries(test ${test_LIB})

 
