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

package com.intel.podm.services.configuration.difference;

import org.apache.commons.collections4.Equator;
import org.apache.commons.lang3.builder.HashCodeBuilder;

import java.util.Set;

interface EquatorWithHashBuilder<T> extends Equator<T> {
    String asEquatorString(T object);
    default int hash(Set<T> objects) {
        HashCodeBuilder builder = new HashCodeBuilder();
        objects.forEach(obj -> builder.append(this.hash((T) obj)));
        return builder.toHashCode();
    }
}
