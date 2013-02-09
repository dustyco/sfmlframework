




add_sources(test test.cpp)

#if (MINGW)
#	require_libraries(general
#		ws2_32  # Required by asio
#	)
#endif ()
require_libraries(test
#	${Boost_SYSTEM_LIBRARY}
#	${Boost_CHRONO_LIBRARY}
#	${Boost_THREAD_LIBRARY}
	${SFML_LIBRARIES}
)

propagate_groups()
