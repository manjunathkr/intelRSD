/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services;

import com.google.common.base.Objects;
import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.Volume;
import com.intel.podm.business.redfish.services.actions.ZoneActionsInvoker;
import com.intel.podm.business.redfish.services.helpers.EndpointFinder;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Set;

import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class VolumeAttacher {
    @Inject
    private EndpointFinder endpointFinder;

    @Inject
    private ZoneActionsInvoker zoneActionsInvoker;

    @Transactional(MANDATORY)
    public void attachVolume(ComposedNode composedNode, Volume volume) throws BusinessApiException {
        Endpoint targetEndpoint = endpointFinder.getOrCreateTargetEndpoint(volume);
        Endpoint initiator = endpointFinder.getOrCreateInitiatorEndpoint(composedNode.getComputerSystem(), targetEndpoint.getFabric());
        attach(composedNode, volume, targetEndpoint);

        Set<Endpoint> zoneEndpoints = composedNode.getEndpoints().stream()
            .filter(endpoint -> Objects.equal(endpoint.getFabric(), targetEndpoint.getFabric()))
            .collect(toSet());

        zoneEndpoints.add(initiator);
        zoneActionsInvoker.createOrUpdateZone(initiator.getZone(), zoneEndpoints, initiator.getFabric());
    }

    private void attach(ComposedNode composedNode, Volume volume, Endpoint targetEndpoint) {
        volume.getMetadata().setAllocated(true);
        composedNode.addVolume(volume);
        targetEndpoint.getMetadata().setAllocated(true);
        composedNode.addEndpoint(targetEndpoint);
    }
}
