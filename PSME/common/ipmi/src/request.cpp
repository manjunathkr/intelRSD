/*!
 * @brief Request object to IPMI.
 *
 * @header{License}
 * @copyright Copyright (c) 2015-2018 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Filesystem}
 * @file ipmi/request.hpp
 */

#include "ipmi/request.hpp"



using namespace ipmi;


Request::Request(IpmiInterface::NetFn fn, IpmiInterface::Cmd cmd) : Message(fn, cmd) {}


Request::~Request() {}


IpmiInterface::ByteBuffer Request::do_pack() const {
    IpmiInterface::ByteBuffer buffer{};
    pack(buffer);
    return buffer;
}
