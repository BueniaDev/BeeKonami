project(video-tests)

set(K052109_SOURCES
	k052109.cpp)

add_executable(k052109-test k052109.cpp)
target_link_libraries(k052109-test PUBLIC k052109 k051962)