include_directories("${PROJECT_SOURCE_DIR}/include")
add_library(com_mqtt SHARED "com_mosquitto.cpp")
TARGET_LINK_LIBRARIES(com_mqtt PUBLIC mosquittopp)

add_dependencies(com_mqtt lauSim)