project(video-tests)

set(K007232_SOURCES
	k007232.cpp)

add_executable(k007232-test k007232.cpp)
target_link_libraries(k007232-test PUBLIC k007232)