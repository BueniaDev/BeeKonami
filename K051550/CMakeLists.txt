set(K051550_HEADERS
	k051550.h)

set(K051550_SOURCES
	k051550.cpp)

add_library(k051550 ${K051550_SOURCES} ${K051550_HEADERS})
target_include_directories(k051550 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(k051550 PRIVATE K051550_STATIC=1 _CRT_SECURE_NO_WARNINGS=1)
add_library(libk051550 ALIAS k051550)