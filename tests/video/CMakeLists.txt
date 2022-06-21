project(video-tests)

set(K052109_SOURCES
	k052109.cpp)

add_executable(k052109-test ${K052109_SOURCES})
target_link_libraries(k052109-test PUBLIC k052109 k051962)

set(K053251_SOURCES
	k053251.cpp)

add_executable(k053251-test ${K053251_SOURCES})
target_link_libraries(k053251-test PUBLIC k053251)