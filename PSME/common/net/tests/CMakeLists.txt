# <license_header>
#
# Copyright (c) 2015-2018 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# </license_header>

if (NOT GTEST_FOUND)
    return()
endif()

include_directories(
    ${GTEST_INCLUDE_DIRS}
    ${NET_INCLUDE_DIRS}
)

add_gtest(test net
    test_runner.cpp
    ipaddress_test.cpp
    socketaddress_test.cpp
    multicast_socket_test.cpp
)

target_link_libraries(${test_target}
    ${NET_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
)

add_custom_target(unittest_net
                  make
)
add_custom_target(unittest_net_run
                  ctest --output-on-failure
)
