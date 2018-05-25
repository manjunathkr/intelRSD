/*!
 * @copyright
 * Copyright (c) 2016-2018 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file null_allowed_validity_checker.cpp
 * */

#include "agent-framework/validators/checkers/null_allowed_validity_checker.hpp"
#include "json-wrapper/json-wrapper.hpp"


using namespace jsonrpc;


NullAllowedValidityChecker::NullAllowedValidityChecker(ValidityChecker::Ptr& _checker) {
    checker.swap(_checker);
}


void NullAllowedValidityChecker::validate(const json::Json& value) const {
    /* only if value IS in the document */
    if ((&value != &(NON_EXISTING_VALUE)) && (value.is_null())) {
        return;
    }
    if (checker) {
        checker->validate(value);
    }
}
