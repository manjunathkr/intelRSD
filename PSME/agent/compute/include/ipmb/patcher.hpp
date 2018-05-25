/*!
 * @brief IPMI message patcher interface.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @file patcher.hpp
 */

#pragma once

/*! Agent namespace */
namespace agent {
/*! Compute namespace */
namespace compute {
/*! IPMB namespace */
namespace ipmb {

class IpmiMessage;

/*! IPMI Message patcher */
class Patcher {
public:
    /*! Patch IPMI Message
     * @param req IPMI request
     * @param rsp IPMI resposne */
    void patch(const IpmiMessage& req, IpmiMessage& rsp);
};

}
}
}

