// Copyright 2022 Andi Hellmund
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#ifndef DATA_CRUNCHING_INTERNAL_TYPE_LIST_HPP
#define DATA_CRUNCHING_INTERNAL_TYPE_LIST_HPP

#include "data_crunching/internal/fixed_string.hpp"

namespace dacr {

template <typename ...Types>
struct TypeList {
    static constexpr std::size_t getSize() {
        return sizeof...(Types);
    }

    template <template <typename ...> typename TargetType>
    using To = TargetType<Types...>;
};

namespace internal {

// ############################################################################
// Trait: Prepend Type List
// ############################################################################
template <typename, typename>
struct TypeListPrependImpl {};

template <typename TypeToPrepend, typename ...TypesInList>
struct TypeListPrependImpl<TypeToPrepend, TypeList<TypesInList...>> {
    using type = TypeList<TypeToPrepend, TypesInList...>;
};

template <typename TypeToPrepend, typename List>
using TypeListPrepend = typename TypeListPrependImpl<TypeToPrepend, List>::type;

// ############################################################################
// Trait: Convert TypeList To Tuple
// ############################################################################
template <typename>
struct ConvertTypeListToTupleImpl {};

template <typename ...Types>
struct ConvertTypeListToTupleImpl<TypeList<Types...>> {
    using type = std::tuple<Types...>;
};

template <typename Types>
using ConvertTypeListToTuple = typename ConvertTypeListToTupleImpl<Types>::type;

} // namespace internal

} // namespace dacr

#endif // DATA_CRUNCHING_INTERNAL_TYPE_LIST_HPP