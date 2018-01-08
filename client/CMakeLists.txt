# Copyright 2017 Clemens Jonischkeit
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


include_directories("${PROJECT_SOURCE_DIR}/client/include")
add_executable(client_mqtt "${PROJECT_SOURCE_DIR}/client/src/mqtt.c" "${PROJECT_SOURCE_DIR}/client/src/client.c")
target_compile_definitions(client_mqtt PRIVATE -DCON=MQTT)
TARGET_LINK_LIBRARIES(client_mqtt mosquitto)

add_executable(privileged "${PROJECT_SOURCE_DIR}/client/src/backend.c" "${PROJECT_SOURCE_DIR}/client/src/privileged.c")
add_library(preload SHARED "${PROJECT_SOURCE_DIR}/client/src/backend.c" "${PROJECT_SOURCE_DIR}/client/src/preload.c")
TARGET_LINK_LIBRARIES(preload pthread)