set(K053251_HEADERS
	k053251.h)

set(K053251_SOURCES
	k053251.cpp)

add_library(k053251 ${K053251_SOURCES} ${K053251_HEADERS})
target_include_directories(k053251 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(k053251 PRIVATE K053251_STATIC=1 _CRT_SECURE_NO_WARNINGS=1)
add_library(libk053251 ALIAS k053251)