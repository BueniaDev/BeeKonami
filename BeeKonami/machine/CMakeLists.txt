project(beekonami-machine)

add_subdirectory(K054000)

add_library(beekonami-machine INTERFACE)
target_link_libraries(beekonami-machine INTERFACE k054000)




