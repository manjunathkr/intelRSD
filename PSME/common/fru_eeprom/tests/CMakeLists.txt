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
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

add_gtest(parser fru_eeprom
        test_fru_eeprom_parser.cpp
        test_runner.cpp
)

target_link_libraries(${test_target}
        fru_eeprom
        ${SAFESTRING_LIBRARIES}
        ${UUID_LIBRARIES}
)

add_custom_target(unittest_fru_eeprom
                  make
)
add_custom_target(unittest_fru_eeprom_run
                  ctest --output-on-failure
)
