exports_files(
    [
        "data/simple.csv",
        "data/simple_with_header.csv",
    ]
)

cc_test(
    name = "test_argparse",
    srcs = [
        "argparse.test.cpp",
    ],
    deps = [
        "//:data_crunching",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "test_dataframe",
    srcs = [
        "dataframe.test.cpp",
    ],
    deps = [
        "//:data_crunching",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "test_namedtuple",
    srcs = [
        "namedtuple.test.cpp",
    ],
    deps = [
        "//:data_crunching",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "test_string",
    srcs = [
        "string.test.cpp",
    ],
    deps = [
        "//:data_crunching",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "test_io_csv",
    srcs = [
        "io/csv.test.cpp",
    ],
    deps = [
        "//:data_crunching",
        "@googletest//:gtest_main",
    ],
    data = [
        "//tests:data/simple.csv",
        "//tests:data/simple_with_header.csv",
    ]
)

cc_test(
    name = "test_internal",
    srcs = [
        "internal/argparse.test.cpp",
        "internal/column.test.cpp",
        "internal/dataframe_general.test.cpp",
        "internal/dataframe_print.test.cpp",
        "internal/dataframe_sort.test.cpp",
        "internal/dataframe_summarize.test.cpp",
        "internal/fixed_string.test.cpp",
        "internal/name_list.test.cpp",
        "internal/namedtuple.test.cpp",
        "internal/string.test.cpp",
        "internal/type_list.test.cpp",
        "internal/utils.test.cpp",
    ],
    deps = [
        "//:data_crunching",
        "@googletest//:gtest_main",
    ],
)