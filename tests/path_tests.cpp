/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Yaroslav Riabtsev <yaroslav.riabtsev@rwth-aachen.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "parser.hpp"
#include <gtest/gtest.h>

TEST(PathTest, ParseDynamicJsonTest) {
    std::shared_ptr<json_lib::json> result;
    std::string buffer = R"($.first.second.third.fourth)";
    parser_lib::parser p(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(
        result->to_string(), "$[\"first\"][\"second\"][\"third\"][\"fourth\"]"
    );

    buffer = R"(@["library"]["books"])";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(result->to_string(), "@[\"library\"][\"books\"]");

    buffer = R"(array[0][1][2][3][4])";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(result->to_string(), "$[\"array\"][0][1][2][3][4]");

    buffer = R"($["food"].drink.coffee[1])";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(result->to_string(), "$[\"food\"][\"drink\"][\"coffee\"][1]");

    buffer = R"((((($).alpha).beta).gamma.delta)[0])";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(
        result->to_string(), "$[\"alpha\"][\"beta\"][\"gamma\"][\"delta\"][0]"
    );

    buffer = R"($["apple", "banana", "cherry", 7, 8, 9])";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(
        result->to_string(),
        "${[\"apple\"], [\"banana\"], [\"cherry\"], [7], [8], [9]}"
    );

    buffer = R"(${.foo, .bar.baz, [1].qux, [1]["flob"]})";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(
        result->to_string(),
        "${[\"foo\"], [\"bar\"][\"baz\"], [1][\"qux\"], [1][\"flob\"]}"
    );

    buffer = R"((key.a[key.b[(key.c)]]))";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(
        result->to_string(),
        "$[\"key\"][\"a\"][$[\"key\"][\"b\"][$[\"key\"][\"c\"]]]"
    );

    buffer = R"(${1, 2, 3, 4})";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result, true), std::runtime_error);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(result->to_string(), "$");

    buffer = R"(${.a, , .c, .d})";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result, true), std::runtime_error);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(result->to_string(), "$");

    buffer = R"($[(1])";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result, true), std::runtime_error);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(result->to_string(), "$");

    buffer = R"($.[1])";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result, true), std::runtime_error);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(result->to_string(), "$");

    std::shared_ptr<json_lib::json> invalid_result;
    buffer = "$.a";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(
        p.completely_parse_json(invalid_result, false), std::runtime_error
    );
    EXPECT_EQ(invalid_result, nullptr);
}

TEST(PathTest, SimplifyDynamicJsonTest) {
    std::shared_ptr<json_lib::json> result;
    std::string buffer = R"({"life":42}.life)";
    parser_lib::parser p(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::integer_json);
    EXPECT_EQ(result->to_string(), "42");

    buffer = R"({"key":$}.key.extra)";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(result->to_string(), "$[\"extra\"]");

    buffer = R"([10,20,30,40,50][3])";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::integer_json);
    EXPECT_EQ(result->to_string(), "40");

    buffer = R"([10,20,[30,30,30,{"key" : $.sample},30],40,50][2][3].key)";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(result->to_string(), "$[\"sample\"]");

    buffer = R"([100,50,25,0][@[3]])";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::integer_json);
    EXPECT_EQ(result->to_string(), "100");

    buffer = R"([[1],[2],[3],[4]]{[3],[2],[1],[0]}[$])";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(result->to_string(), "[[4][$], [3][$], [2][$], [1][$]]");

    buffer = R"([[1],[2],[3],[4]]{[3],[2],[1],[0]}[0])";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(result->to_string(), "[4, 3, 2, 1]");

    buffer = R"([1,2,3,4][3,2,1,0])";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(result->to_string(), "[4, 3, 2, 1]");

    buffer = R"([1,2,3,4][$[2]])";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(result->to_string(), "[1, 2, 3, 4][$[2]]");

    buffer = R"([1,2,3,4][$.key])";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(result->to_string(), "[1, 2, 3, 4][$[\"key\"]]");

    buffer = R"({"a":1, "b":2, "c":3}{.b, .c, .a})";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(result->to_string(), "[2, 3, 1]");

    buffer = R"({"key1":5, "key2":@.key1, "key3":55})";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::object_json);
    EXPECT_EQ(result->to_string(), "{\"key1\": 5, \"key2\": 5, \"key3\": 55}");

    buffer = R"({"key1":5, "key2":@, "key3":55}.key2.key1)";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::integer_json);
    EXPECT_EQ(result->to_string(), "5");

    // todo:
    // buffer = R"({"key1":5, "key2":@, "key3":55}.key2.key2.key2.key2.key1)";
    // p = parser_lib::parser(buffer);
    // p.completely_parse_json(result, true);
    // EXPECT_EQ(result->type(), json_lib::json_type::integer_json);
    // EXPECT_EQ(result->to_string(), "5");

    buffer = R"({"key1":5, "key2":@, "key3":55})";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::object_json);
    EXPECT_THROW(result->to_string(), std::runtime_error);

    buffer = R"({"key1":5, "key2":@, "key3":55}[0])";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result, true), std::invalid_argument);
    EXPECT_EQ(result->type(), json_lib::json_type::object_json);
    EXPECT_THROW(result->to_string(), std::runtime_error);

    buffer = R"([1,2,3,@,4])";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::array_json);
    EXPECT_THROW(result->to_string(), std::runtime_error);

    buffer = R"("string"[[1,2,3,@,4]])";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result, true), std::invalid_argument);
    EXPECT_EQ(result->type(), json_lib::json_type::string_json);
    EXPECT_EQ(result->to_string(), "\"string\"");

    buffer = R"({"b" : 5}.a)";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result, true), std::out_of_range);
    EXPECT_EQ(result->to_string(), "{\"b\": 5}");

    buffer = R"({"b" : 5}[0])";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result, true), std::invalid_argument);
    EXPECT_EQ(result->to_string(), "{\"b\": 5}");

    buffer = R"([1,2,3].a)";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result, true), std::invalid_argument);
    EXPECT_EQ(result->to_string(), "[1, 2, 3]");

    buffer = R"([1,2,3][4])";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result, true), std::out_of_range);
    EXPECT_EQ(result->to_string(), "[1, 2, 3]");
}

TEST(PathTest, AbstractFunctionJsonTest) {
    std::shared_ptr<json_lib::json> result;
    std::string buffer = R"(fu())";
    parser_lib::parser p(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(
        std::dynamic_pointer_cast<reference_lib::json_reference>(result)
            ->reference_type(),
        reference_lib::json_reference_type::function_json
    );
    EXPECT_EQ(result->to_string(), "fu()");

    buffer = R"(fu(1))";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(
        std::dynamic_pointer_cast<reference_lib::json_reference>(result)
            ->reference_type(),
        reference_lib::json_reference_type::function_json
    );
    EXPECT_EQ(result->to_string(), "fu(1)");

    buffer
        = R"(fu(null, true, false, 1, 2.0, "string", [1, 2, 3], {"key": 4}))";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(
        std::dynamic_pointer_cast<reference_lib::json_reference>(result)
            ->reference_type(),
        reference_lib::json_reference_type::function_json
    );
    EXPECT_EQ(
        result->to_string(),
        "fu(null, true, false, 1, 2.0, \"string\", [1, 2, 3], {\"key\": 4})"
    );

    buffer = R"(fu($.array[0]))";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(
        std::dynamic_pointer_cast<reference_lib::json_reference>(result)
            ->reference_type(),
        reference_lib::json_reference_type::function_json
    );
    EXPECT_EQ(result->to_string(), "fu($[\"array\"][0])");

    buffer = R"(fu(array[0]))";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(
        std::dynamic_pointer_cast<reference_lib::json_reference>(result)
            ->reference_type(),
        reference_lib::json_reference_type::function_json
    );
    EXPECT_EQ(result->to_string(), "fu($[\"array\"][0])");

    buffer = R"($.array.fu())";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    EXPECT_EQ(result->type(), json_lib::json_type::custom_json);
    EXPECT_EQ(
        std::dynamic_pointer_cast<reference_lib::json_reference>(result)
            ->length(),
        2
    );
    EXPECT_EQ(result->to_string(), "$[\"array\"][fu()]");
}
