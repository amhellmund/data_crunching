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

#ifndef DATA_CRUNCHING_INTERNAL_UTILS_HPP
#define DATA_CRUNCHING_INTERNAL_UTILS_HPP

#include <ranges>
#include <tuple>

#include <type_traits>

#include "data_crunching/internal/type_list.hpp"

namespace dacr::internal {

// ############################################################################
// Concepts: Arithmetic Data Types
// ############################################################################
template <typename T>
concept IsArithmetic = std::is_arithmetic_v<T>;

template <typename T>
concept IsIntegral = std::is_integral_v<T>;

template <typename T>
concept IsFloatingPoint = std::is_floating_point_v<T>;


// ############################################################################
// Trait: Tuple Prepend
// ############################################################################
template <typename ...>
struct TuplePrependImpl {};

template <typename TypeToPrepend, typename ...TupleTypes>
struct TuplePrependImpl<TypeToPrepend, std::tuple<TupleTypes...>> {
    using type = std::tuple<TypeToPrepend, TupleTypes...>;
};

template <typename TypeToPrepend, typename Tuple>
using TuplePrepend = typename TuplePrependImpl<TypeToPrepend, Tuple>::type;

// ############################################################################
// Trait: Integer Sequence Prepend
// ############################################################################
template <std::size_t, typename>
struct IntegerSequencePrependImpl {};

template <std::size_t NumberToPrepend, std::size_t ...NumbersInSequence>
struct IntegerSequencePrependImpl<NumberToPrepend, std::integer_sequence<std::size_t, NumbersInSequence...>> {
    using type = std::integer_sequence<std::size_t, NumberToPrepend, NumbersInSequence...>;
};

template <std::size_t NumberToPrepend, typename Sequence>
using IntegerSequencePrepend = typename IntegerSequencePrependImpl<NumberToPrepend, Sequence>::type;

// ############################################################################
// Trait: Integer Sequence Size
// ############################################################################
template <typename>
struct IntegerSequenceSizeImpl {
};

template <std::size_t ...Numbers>
struct IntegerSequenceSizeImpl<std::integer_sequence<std::size_t, Numbers...>> {
    static constexpr std::size_t size = sizeof...(Numbers);
};

template <typename Seq>
constexpr std::size_t get_integer_sequence_size = IntegerSequenceSizeImpl<Seq>::size;

// ############################################################################
// Trait: Integer Sequence By Range
// ############################################################################
template <std::size_t LoopIndex, std::size_t EndExclusive>
struct IntegerSequenceByRangeImpl {
    using type = IntegerSequencePrepend<LoopIndex, typename IntegerSequenceByRangeImpl<LoopIndex + 1, EndExclusive>::type>;
};

template <std::size_t EndCondition>
struct IntegerSequenceByRangeImpl<EndCondition, EndCondition> {
    using type = std::integer_sequence<std::size_t>;
};

template <std::size_t Start, std::size_t EndExclusive>
requires (Start <= EndExclusive)
using IntegerSequenceByRange = typename IntegerSequenceByRangeImpl<Start, EndExclusive>::type;

// ############################################################################
// Trait: Is Convertible To Types
// ############################################################################
template <typename ...>
struct IsConvertibleToTypeImpl : std::true_type {};

template <typename FirstTypeToConvert, typename ...RestTypesToConvert, typename FirstTypeConvertedTo, typename ...RestTypesConvertedTo>
struct IsConvertibleToTypeImpl<TypeList<FirstTypeToConvert, RestTypesToConvert...>, TypeList<FirstTypeConvertedTo, RestTypesConvertedTo...>> {
    static constexpr bool value = std::is_convertible_v<FirstTypeToConvert, FirstTypeConvertedTo> && IsConvertibleToTypeImpl<TypeList<RestTypesToConvert...>, TypeList<RestTypesConvertedTo...>>::value;
};

template <typename TypesToConvertList, typename ColumnList>
constexpr bool is_convertible_to = IsConvertibleToTypeImpl<TypesToConvertList, ColumnList>::value;

// ############################################################################
// Trait: Extract Types From Range List
// ############################################################################
template <std::ranges::range ...>
struct ExtractValueTypesFromRangesImpl {
    using type = TypeList<>;
};

template <std::ranges::range FirstRange, std::ranges::range ...RestRanges>
struct ExtractValueTypesFromRangesImpl<FirstRange, RestRanges...> {
    using type = TypeListPrepend<
        std::ranges::range_value_t<FirstRange>,
        typename ExtractValueTypesFromRangesImpl<RestRanges...>::type
    >;
};

template <std::ranges::range ...Ranges>
using ExtractValueTypesFromRanges = typename ExtractValueTypesFromRangesImpl<Ranges...>::type;


// ############################################################################
// Concept: Range With Size
// ############################################################################
template <typename T>
concept IsRangeWithSize = (
    std::ranges::range<T> && requires(T &t) {
        std::ranges::size(t);
    }
);

// ############################################################################
// Util: Get Min Size From Range List
// ############################################################################
template <IsRangeWithSize ...Ranges>
inline std::size_t getMinSizeFromRanges (Ranges&& ...ranges) {
    return std::ranges::min(
        {std::ranges::size(ranges)...}
    );
}

// ############################################################################
// Concept: Is Convertible From String
// ############################################################################
template <typename T>
concept IsConvertibleFromString = 
    IsArithmetic<T> ||
    requires {
        T{std::declval<std::string>()};
    };

} // namespace dacr::internal

#endif // DATA_CRUNCHING_INTERNAL_UTILS_HPP