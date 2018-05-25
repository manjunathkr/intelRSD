/*
 * Copyright (c) 2018 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.intel.podm.business.redfish.services.mappers;

import com.intel.podm.business.dto.DcbxConfigDto.PriorityClassMappingDto;
import com.intel.podm.business.entities.redfish.embeddables.PriorityClassMapping;
import com.intel.podm.mappers.BrilliantMapper;

import javax.enterprise.context.Dependent;

@Dependent
public class PriorityClassMappingDtoMapper extends BrilliantMapper<PriorityClassMapping, PriorityClassMappingDto> {

    protected PriorityClassMappingDtoMapper() {
        super(PriorityClassMapping.class, PriorityClassMappingDto.class);
    }
}
