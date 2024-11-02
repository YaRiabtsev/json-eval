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

TEST(ParserTest, ParseConstKeywordJsonTest) {
    std::shared_ptr<json_lib::json> result;
    std::string buffer = "null";
    parser_lib::parser p(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::null_json);

    buffer = "true";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::boolean_json);

    buffer = "false";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::boolean_json);

    buffer = "true, null";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result), std::runtime_error);
    EXPECT_EQ(result->type(), json_lib::json_type::boolean_json);
    std::shared_ptr<json_lib::json> invalid_result;

    buffer = "trulse";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(invalid_result), std::runtime_error);
    EXPECT_EQ(invalid_result, nullptr);
}

TEST(ParserTest, ParseWhiteSpaceJsonTest) {
    std::shared_ptr<json_lib::json> result;
    std::string buffer = "    ";
    parser_lib::parser p(buffer);
    EXPECT_THROW(p.completely_parse_json(result), std::runtime_error);
    EXPECT_EQ(result, nullptr);

    buffer = "\n\n\t\t\t\n    \n\r";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result), std::runtime_error);
    EXPECT_EQ(result, nullptr);

    buffer = R"(                  // illegal comment 0

                     // illegal comment 1
                         // // illegal comment 2
          //////// // illegal comment 3
        //*//*/*//* // illegal comment 4
                 )";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result), std::runtime_error);
    EXPECT_EQ(result, nullptr);

    buffer = R"(

                  // illegal comment 0

                      // illegal comment 1
                          // // illegal comment 2
                  null//////// // illegal comment 3
         //*//*/*//* // illegal comment 4
                  )";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::null_json);

    buffer = R"(// illegal comment 0
                      // illegal comment 1
                          // // illegal comment 2
                  true//////// // illegal comment 3
         //*//*/*//* // illegal comment 4
                  )";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::boolean_json);
}

TEST(ParserTest, ParseNumberJsonTest) {
    std::shared_ptr<json_lib::json> result;
    std::string buffer = "42";
    parser_lib::parser p(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::integer_json);
    EXPECT_EQ(result->to_string(), "42");

    buffer = "0";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::integer_json);
    EXPECT_EQ(result->to_string(), "0");

    buffer = "0.0";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::real_json);
    EXPECT_EQ(result->to_string(), "0.0");

    buffer = "-42";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::integer_json);
    EXPECT_EQ(result->to_string(), "-42");

    buffer = "987654321";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::integer_json);
    EXPECT_EQ(result->to_string(), "987654321");

    buffer = "3.14159";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::real_json);
    EXPECT_EQ(result->to_string(), "3.14159");

    buffer = "-3.14159";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::real_json);
    EXPECT_EQ(result->to_string(), "-3.14159");

    buffer = "0.007";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::real_json);
    EXPECT_EQ(result->to_string(), "0.007");

    buffer = "1.23e-4";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::real_json);
    EXPECT_EQ(result->to_string(), "1.23e-4");

    buffer = "1.23e+4";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::real_json);
    EXPECT_EQ(result->to_string(), "1.23e+4");

    buffer = "2e10";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::real_json);
    EXPECT_EQ(result->to_string(), "2e10");

    buffer = "-5e-2";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::real_json);
    EXPECT_EQ(result->to_string(), "-5e-2");

    buffer = "4.56xxxinvalidsuffix";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result), std::runtime_error);
    EXPECT_EQ(result->type(), json_lib::json_type::real_json);
    EXPECT_EQ(result->to_string(), "4.56");

    std::shared_ptr<json_lib::json> invalid_result;
    buffer = "0123";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(invalid_result), std::runtime_error);
    EXPECT_EQ(invalid_result, nullptr);

    buffer = "3.";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(invalid_result), std::runtime_error);
    EXPECT_EQ(invalid_result, nullptr);

    buffer = "1.23e";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(invalid_result), std::runtime_error);
    EXPECT_EQ(invalid_result, nullptr);

    buffer = "-";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(invalid_result), std::runtime_error);
    EXPECT_EQ(invalid_result, nullptr);

    buffer = "+";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(invalid_result), std::runtime_error);
    EXPECT_EQ(invalid_result, nullptr);
}

TEST(ParserTest, ParseStringJsonTest) {
    std::shared_ptr<json_lib::json> result;
    std::string buffer = "\"The quick brown fox jumps over the lazy dog\"";
    parser_lib::parser p(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::string_json);
    EXPECT_EQ(
        result->to_string(), "\"The quick brown fox jumps over the lazy dog\""
    );

    buffer = "\"Hell, world... I need to cover the code with tests, so I'm "
             "thinking about good examples for the test-cases instead of... "
             "instead of thinking of what else I could do while listening to "
             "music.\"";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::string_json);
    EXPECT_EQ(
        result->to_string(),
        "\"Hell, world... I need to cover the code with tests, so I'm thinking "
        "about good examples for the test-cases instead of... instead of "
        "thinking of what else I could do while listening to music.\""
    );

    buffer = "\"I can't think about that right now. If I do, I'll go crazy. "
             "I'll think about that tomorrow.\"";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::string_json);
    EXPECT_EQ(
        result->to_string(),
        "\"I can't think about that right now. If I do, I'll go crazy. I'll "
        "think about that tomorrow.\""
    );

    buffer
        = "\"EXPECT_EQ(result->to_string(), \\\"EXPECT_EQ(result->to_string(), "
          "\\\\\\\"\\\\\\\");\\\"); isn't it a good test string?\"";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::string_json);
    EXPECT_EQ(
        result->to_string(),
        "\"EXPECT_EQ(result->to_string(), \\\"EXPECT_EQ(result->to_string(), "
        "\\\\\\\"\\\\\\\");\\\"); isn't it a good test string?\""
    );

    buffer = R"("C:\\Projects\\JSONTest\\result.json")";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::string_json);
    EXPECT_EQ(
        result->to_string(), "\"C:\\\\Projects\\\\JSONTest\\\\result.json\""
    );
    buffer = R"("The Unicode character for smiley is \u263A")";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::string_json);
    EXPECT_EQ(result->to_string(), "\"The Unicode character for smiley is ☺\"");

    buffer = "\"First line\nSecond line\rBackspace\b happens here\nTabbed "
             "line:\tTabbed\fEnd of string\"";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::string_json);
    EXPECT_EQ(
        result->to_string(),
        "\"First line\\nSecond line\\rBackspace\\b happens here\\nTabbed "
        "line:\\tTabbed\\fEnd of string\""
    );

    buffer
        = "\"\\/First line\\nSecond line\\rBackspace\\b happens here\\nTabbed "
          "line:\\tTabbed\\fEnd of string\\\"\"";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::string_json);
    EXPECT_EQ(
        result->to_string(),
        "\"/First line\\nSecond line\\rBackspace\\b happens here\\nTabbed "
        "line:\\tTabbed\\fEnd of string\\\"\""
    );

    buffer = "\"I hate Emoji! 😊🔥\"";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::string_json);
    EXPECT_EQ(result->to_string(), "\"I hate Emoji! 😊🔥\"");

    buffer = "\"Symbols like @#$%^&*()_+-=...\"";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::string_json);
    EXPECT_EQ(result->to_string(), "\"Symbols like @#$%^&*()_+-=...\"");

    buffer = "\"\"";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result);
    EXPECT_EQ(result->type(), json_lib::json_type::string_json);
    EXPECT_EQ(result->to_string(), "\"\"");
    std::shared_ptr<json_lib::json> invalid_result;

    buffer = "\"It seems like I've missed something...";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(invalid_result), std::runtime_error);
    EXPECT_EQ(invalid_result, nullptr);

    buffer = R"("Invalid\x01ControlCharacter")";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(invalid_result), std::runtime_error);
    EXPECT_EQ(invalid_result, nullptr);

    buffer = R"("An invalid escape: \q")";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(invalid_result), std::runtime_error);
    EXPECT_EQ(invalid_result, nullptr);

    buffer = R"("Unicode escape gone wrong: \u12")";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(invalid_result), std::runtime_error);
    EXPECT_EQ(invalid_result, nullptr);
}

TEST(ParserTest, ParseArrayJsonTest) {
    std::shared_ptr<json_lib::json> result;
    std::string buffer = "[";
    parser_lib::parser p(buffer);
    EXPECT_THROW(p.completely_parse_json(result), std::runtime_error);
    EXPECT_EQ(result, nullptr);

    buffer = "[1, 2, 3, 4, 5, 6, 7, 8, 9, 10";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result), std::runtime_error);
    EXPECT_EQ(result, nullptr);

    buffer = "[1, 2, 3, 4, 5, 6,]";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result), std::runtime_error);
    EXPECT_EQ(result, nullptr);

    buffer = "[1, 2, 3, 4, 5, 6, [7, 8, 9, 10]";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result), std::runtime_error);
    EXPECT_EQ(result, nullptr);

    buffer = "{\"arr\":[1, 2, 3, 4, 5, 6, ]}";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result), std::runtime_error);
    EXPECT_EQ(result, nullptr);

    buffer = "{\"arr\" : [1, 2, 3, 4, 5, 6}";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result), std::runtime_error);
    EXPECT_EQ(result, nullptr);
}

TEST(ParserTest, ParseObjectJsonTest) {
    std::shared_ptr<json_lib::json> result;
    std::string buffer = "{";
    parser_lib::parser p(buffer);
    EXPECT_THROW(p.completely_parse_json(result), std::runtime_error);
    EXPECT_EQ(result, nullptr);

    buffer = "{1, 2, 3, 4, 5, 6, 7, 8, 9, 10";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result), std::runtime_error);
    EXPECT_EQ(result, nullptr);

    buffer = R"({"key1", "key2", "key3"})";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result), std::runtime_error);
    EXPECT_EQ(result, nullptr);

    buffer = R"({"key1" : 1, "key2" : 2, "key3" : 3)";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result), std::runtime_error);
    EXPECT_EQ(result, nullptr);

    buffer = R"({"key1" : 1, "key2" : , "key3" : 3})";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result), std::runtime_error);
    EXPECT_EQ(result, nullptr);

    buffer = R"({"key1" : 1, "key2" : "key3" : 3})";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result), std::runtime_error);
    EXPECT_EQ(result, nullptr);

    buffer = R"({"key1" : 1, "key2" : {"key3" : 3})";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result), std::runtime_error);
    EXPECT_EQ(result, nullptr);

    buffer = R"({"key1" : 1, "key2" : [{"key3" : 3]})";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(p.completely_parse_json(result), std::runtime_error);
    EXPECT_EQ(result, nullptr);
}

TEST(ParserTest, ParseFileJsonTest) {
    std::shared_ptr<json_lib::json> result;
    std::filesystem::path path = "test_data/de.json";
    parser_lib::parser p(path);
    EXPECT_NO_THROW(p.completely_parse_json(result));
    EXPECT_EQ(result->type(), json_lib::json_type::object_json);

    // todo: integer --> int64
    // path = "test_data/lshw.json";
    // p = parser_lib::parser(path);
    // EXPECT_NO_THROW(p.completely_parse_json(result));
    // EXPECT_EQ(result->type(), json_lib::json_type::object_json);

    path = "test_data/troma_imdb.json";
    p = parser_lib::parser(path);
    EXPECT_NO_THROW(p.completely_parse_json(result));
    EXPECT_EQ(result->type(), json_lib::json_type::object_json);
    std::shared_ptr<json_lib::json> pretty_result;

    path = "test_data/pretty_troma.json";
    p = parser_lib::parser(path);
    EXPECT_NO_THROW(p.completely_parse_json(pretty_result));
    EXPECT_EQ(pretty_result->type(), json_lib::json_type::object_json);
    EXPECT_EQ(result->to_string(), pretty_result->to_string());

    path = "test_data/preety_troma.json";
    EXPECT_THROW(p = parser_lib::parser(path), std::invalid_argument);
}

TEST(ParserTest, ParseDynamicJsonTest) {
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

TEST(ParserTest, SimplifyDynamicJsonTest) {
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

    buffer = R"({"b" : 5}.a)";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(
        p.completely_parse_json(result, true), std::out_of_range
    );
    EXPECT_EQ(result->to_string(), "{\"b\": 5}");

    buffer = R"({"b" : 5}[0])";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(
        p.completely_parse_json(result, true), std::invalid_argument
    );
    EXPECT_EQ(result->to_string(), "{\"b\": 5}");

    buffer = R"([1,2,3].a)";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(
        p.completely_parse_json(result, true), std::invalid_argument
    );
    EXPECT_EQ(result->to_string(), "[1, 2, 3]");

    buffer = R"([1,2,3][4])";
    p = parser_lib::parser(buffer);
    EXPECT_THROW(
        p.completely_parse_json(result, true), std::out_of_range
    );
    EXPECT_EQ(result->to_string(), "[1, 2, 3]");
}

TEST(ParserTest, AbstractFunctionJsonTest) {
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
