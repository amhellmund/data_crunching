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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <type_traits>

#include "data_crunching/internal/column.hpp"
#include "data_crunching/internal/dataframe.hpp"

using dacr::Column;
using dacr::internal::ConstructColumnStoreDataType;

template <typename T>
struct TestContainer {};

TEST(DataFrameInternal, ConstructColumnStoreDataType) {
    EXPECT_TRUE((std::is_same_v<
        ConstructColumnStoreDataType<TestContainer>,
        std::tuple<>
    >));
}

TEST(DataFrameInternal, ConstructColumnStoreDataTypeSingleColumn) {
    EXPECT_TRUE((std::is_same_v<
        ConstructColumnStoreDataType<TestContainer, Column<"int", int>>,
        std::tuple<TestContainer<int>>
    >)); 
}

TEST(DataFrameInternal, ConstructColumnStoreDataTypeTwoColumns) {
    EXPECT_TRUE((std::is_same_v<
        ConstructColumnStoreDataType<TestContainer, Column<"int", int>, Column<"dbl", double>>,
        std::tuple<TestContainer<int>, TestContainer<double>>
    >)); 
}