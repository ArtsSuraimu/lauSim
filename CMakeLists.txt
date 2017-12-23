cmake_minimum_required(VERSION 3.2)
project(lauSim VERSION 0.1 LANGUAGES C CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -pedantic -Wl,-z,relro,-z,now -fPIE -pie -fstack-protector-strong -g -Wno-unused-parameter")
set(CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -pedantic -Wl,-z,relro,-z,now -fPIE -pie -fstack-protector-strong -g -Wno-unused-parameter")

add_subdirectory(client)

include_directories("${PROJECT_SOURCE_DIR}/include")
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp"
                  "${PROJECT_SOURCE_DIR}/srv/proto/*.cc"
                  "${PROJECT_SOURCE_DIR}/srv/*.cpp"
                  "${PROJECT_SOURCE_DIR}/com/mosquitto*.cpp")

#MESSAGE( STATUS "SRC_DIR:   " ${PROJECT_SOURCE_DIR})
MESSAGE( STATUS "SOURCES:   " ${SOURCES})

add_executable(lauSim ${SOURCES})
TARGET_LINK_LIBRARIES(lauSim mosquittopp dl protobuf)

install(TARGETS lauSim EXPORT lauSimConfig
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/lauSim/)
install(EXPORT lauSimConfig DESTINATION share/lauSim/cmake)

add_subdirectory(plugins)