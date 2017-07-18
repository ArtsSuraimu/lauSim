include_directories("${PROJECT_SOURCE_DIR}/include")
add_library(echo SHARED "echo.c")

add_dependencies(echo lauSim)