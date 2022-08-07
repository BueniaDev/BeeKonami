project(machine-tests)

set(K054000_SOURCES
	k054000.cpp)

add_executable(k054000-test ${K054000_SOURCES})
target_link_libraries(k054000-test PUBLIC k054000)