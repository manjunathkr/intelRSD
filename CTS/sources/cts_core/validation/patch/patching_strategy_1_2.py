"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2017 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""

from .patching_strategy import PatchingStrategy
from cts_framework.commons.enums import ReturnCodes

class PatchingStrategy1_2(PatchingStrategy):
    @property
    def allowable_return_codes(self):
        """:rtype: list(ReturnCodes)"""

        return [ReturnCodes.OK,                    #200
                ReturnCodes.NO_CONTENT,            #204
                ReturnCodes.BAD_REQUEST,           #400
                ReturnCodes.METHOD_NOT_ALLOWED,    #405
                ReturnCodes.UNPROCESSABLE_ENTITY,  #422
                ReturnCodes.NOT_IMPLEMENTED        #501
                ]

    def was_patch_applied(self, return_code):
        return return_code == ReturnCodes.NO_CONTENT
