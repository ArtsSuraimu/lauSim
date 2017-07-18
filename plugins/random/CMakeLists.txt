include_directories("${PROJECT_SOURCE_DIR}/include")
add_library(random SHARED "random.c")

add_dependencies(random lauSim)