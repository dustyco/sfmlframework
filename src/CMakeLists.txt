




add_sources(test test.cpp)
require_libraries(test
#	${Boost_SYSTEM_LIBRARY}
#	${Boost_CHRONO_LIBRARY}
#	${Boost_THREAD_LIBRARY}
	${SFML_LIBRARIES}
)

add_sources(pong pong.cpp)
require_libraries(pong
	${SFML_LIBRARIES}
)

add_sources(shader shader.cpp)
require_libraries(shader
	${SFML_LIBRARIES}
)

add_sources(buoyancy_2d buoyancy_2d.cpp)
require_libraries(buoyancy_2d
	${SFML_LIBRARIES}
)

propagate_groups()
