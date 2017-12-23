include_directories("${PROJECT_SOURCE_DIR}/client/include")
add_executable(client_mqtt "${PROJECT_SOURCE_DIR}/client/src/mqtt.c" "${PROJECT_SOURCE_DIR}/client/src/client.c")
TARGET_LINK_LIBRARIES(client_mqtt mosquitto)