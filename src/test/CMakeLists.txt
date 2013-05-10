

# Suppress switch warnings
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch")
endif()

set(target_NAME test)
add_executable(${target_NAME} main.cpp)
target_link_libraries(${target_NAME} ${SFML_LIBRARIES})

# Copy data assets
add_custom_command(
	TARGET ${target_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${PROJECT_SOURCE_DIR}/data"
	$<TARGET_FILE_DIR:${target_NAME}>
)
