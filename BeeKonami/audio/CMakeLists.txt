project(beekonami-audio)

add_subdirectory(K051649)
add_subdirectory(K053260)
add_subdirectory(K054539)
add_subdirectory(K007232)

add_library(beekonami-audio INTERFACE)
target_link_libraries(beekonami-audio INTERFACE k051649 k053260 k054539 k007232)




