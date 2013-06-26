

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
	# Suppress switch warnings
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch")
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
	# Turn on optimization
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

set(target_NAME color_correction_lut)
add_executable(${target_NAME} main.cpp)
target_link_libraries(${target_NAME}
	${SFML_LIBRARIES}
	${GLEW_LIBRARIES}
)

# Copy data assets
add_custom_command(
	TARGET ${target_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${PROJECT_SOURCE_DIR}/data"
	$<TARGET_FILE_DIR:${target_NAME}>
)
