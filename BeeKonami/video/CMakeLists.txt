project(beekonami-video)

add_subdirectory(K052109)
add_subdirectory(K051962)
add_subdirectory(K051960)

add_library(beekonami-video INTERFACE)
target_link_libraries(beekonami-video INTERFACE k052109 k051962 k051960)




