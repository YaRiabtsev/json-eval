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
#include "json.hpp"
#include <gtest/gtest.h>

TEST(JsonTest, JsonTypeTest) {
    EXPECT_EQ(
        json_lib::json_type_to_string(json_lib::json_type::object_json),
        "JSON-Object"
    );
    EXPECT_EQ(
        json_lib::json_type_to_string(json_lib::json_type::array_json),
        "JSON-Array"
    );
    EXPECT_EQ(
        json_lib::json_type_to_string(json_lib::json_type::string_json),
        "JSON-String"
    );
    EXPECT_EQ(
        json_lib::json_type_to_string(json_lib::json_type::real_json),
        "JSON-Real"
    );
    EXPECT_EQ(
        json_lib::json_type_to_string(json_lib::json_type::integer_json),
        "JSON-Integer"
    );
    EXPECT_EQ(
        json_lib::json_type_to_string(json_lib::json_type::boolean_json),
        "JSON-Boolean"
    );
    EXPECT_EQ(
        json_lib::json_type_to_string(json_lib::json_type::null_json),
        "JSON-Null"
    );
    constexpr auto invalid_type = static_cast<json_lib::json_type>(-1);
    EXPECT_EQ(json_lib::json_type_to_string(invalid_type), "Unknown JSON-Type");
}

TEST(JsonTest, NullJsonTest) {
    const auto obj1 = std::make_shared<json_lib::json>();
    EXPECT_EQ(obj1->type(), json_lib::json_type::null_json);
    EXPECT_EQ(obj1->to_string(), "null");
    EXPECT_EQ(obj1->to_string(true), "null");
    EXPECT_EQ(obj1->to_string(false), "null");
    EXPECT_EQ(obj1->to_string(0, true), "null");
    EXPECT_EQ(obj1->to_string(0, false), "null");
    const auto obj2 = std::make_shared<json_lib::json>();
    EXPECT_EQ(obj2->type(), json_lib::json_type::null_json);
    EXPECT_EQ(obj2->to_string(), "null");
}

TEST(JsonTest, BooleanJsonTest) {
    const auto obj_true = std::make_shared<json_lib::json_boolean>(true);
    EXPECT_EQ(obj_true->type(), json_lib::json_type::boolean_json);
    const auto obj_false = std::make_shared<json_lib::json_boolean>(false);
    EXPECT_EQ(obj_false->type(), json_lib::json_type::boolean_json);
    const std::shared_ptr<json_lib::json> obj_cast_true
        = std::make_shared<json_lib::json_boolean>(true);
    EXPECT_EQ(obj_cast_true->type(), json_lib::json_type::boolean_json);
    EXPECT_EQ(obj_cast_true->to_string(), "true");
    const std::shared_ptr<json_lib::json> obj_cast_false
        = std::make_shared<json_lib::json_boolean>(false);
    EXPECT_EQ(obj_cast_false->type(), json_lib::json_type::boolean_json);
    EXPECT_EQ(obj_cast_false->to_string(), "false");
    EXPECT_EQ(obj_true->to_string(2, true), "true");
    EXPECT_EQ(obj_false->to_string(2, true), "false");
    EXPECT_EQ(obj_true->to_string(0, true), "true");
    EXPECT_EQ(obj_false->to_string(0, true), "false");
}

TEST(JsonTest, IntegerJsonTest) {
    const std::shared_ptr<json_lib::json> obj1
        = std::make_shared<json_lib::json_integer>(0);
    EXPECT_EQ(obj1->type(), json_lib::json_type::integer_json);
    EXPECT_EQ(obj1->to_string(), "0");
    const auto num1 = std::dynamic_pointer_cast<json_lib::json_integer>(obj1);
    EXPECT_EQ(num1->as_index(), 0);
    const std::shared_ptr<json_lib::json> obj2
        = std::make_shared<json_lib::json_integer>(
            std::numeric_limits<int>::max()
        );
    EXPECT_EQ(obj2->type(), json_lib::json_type::integer_json);
    EXPECT_EQ(obj2->to_string(), "2147483647");
    const auto num2 = std::dynamic_pointer_cast<json_lib::json_integer>(obj2);
    EXPECT_EQ(num2->as_index(), 2147483647);
    const std::shared_ptr<json_lib::json> obj3
        = std::make_shared<json_lib::json_integer>(
            std::numeric_limits<int>::min()
        );
    EXPECT_EQ(obj3->type(), json_lib::json_type::integer_json);
    EXPECT_EQ(obj3->to_string(), "-2147483648");
    const auto num3 = std::dynamic_pointer_cast<json_lib::json_integer>(obj3);
    EXPECT_EQ(num3->as_index(), -2147483648);
    const std::shared_ptr<json_lib::json> obj4
        = std::make_shared<json_lib::json_integer>(-47);
    EXPECT_EQ(obj4->type(), json_lib::json_type::integer_json);
    EXPECT_EQ(obj4->to_string(), "-47");
    const auto num4 = std::dynamic_pointer_cast<json_lib::json_integer>(obj4);
    EXPECT_EQ(num4->as_index(), -47);
    const std::shared_ptr<json_lib::json> obj5
        = std::make_shared<json_lib::json_integer>(73);
    EXPECT_EQ(obj5->type(), json_lib::json_type::integer_json);
    EXPECT_EQ(obj5->to_string(), "73");
    const auto num5 = std::dynamic_pointer_cast<json_lib::json_integer>(obj5);
    EXPECT_EQ(num5->as_index(), 73);
    EXPECT_EQ(obj4->to_string(0, true), "-47");
    EXPECT_EQ(obj5->to_string(2, true), "73");
    const std::shared_ptr<json_lib::json> obj6
        = std::make_shared<json_lib::json_integer>(-2147483000);
    EXPECT_EQ(obj6->type(), json_lib::json_type::integer_json);
    EXPECT_EQ(obj6->to_string(), "-2147483000");
    const auto num6 = std::dynamic_pointer_cast<json_lib::json_integer>(obj6);
    EXPECT_EQ(num6->as_index(), -2147483000);
    const std::shared_ptr<json_lib::json> obj7
        = std::make_shared<json_lib::json_integer>(2147483000);
    EXPECT_EQ(obj7->type(), json_lib::json_type::integer_json);
    EXPECT_EQ(obj7->to_string(), "2147483000");
    const auto num7 = std::dynamic_pointer_cast<json_lib::json_integer>(obj7);
    EXPECT_EQ(num7->as_index(), 2147483000);
}

TEST(JsonTest, RealJsonTest) {
    const std::shared_ptr<json_lib::json> obj1
        = std::make_shared<json_lib::json_real>(0.0f);
    EXPECT_EQ(obj1->type(), json_lib::json_type::real_json);
    EXPECT_EQ(obj1->to_string(), "0.0");
    const std::shared_ptr<json_lib::json> obj2
        = std::make_shared<json_lib::json_real>(std::numeric_limits<float>::max(
        ));
    EXPECT_EQ(obj2->type(), json_lib::json_type::real_json);
    EXPECT_EQ(obj2->to_string(), "340282346638528859811704183484516925440.0");
    const std::shared_ptr<json_lib::json> obj3
        = std::make_shared<json_lib::json_real>(
            std::numeric_limits<float>::denorm_min()
        );
    EXPECT_EQ(obj3->type(), json_lib::json_type::real_json);
    EXPECT_EQ(obj3->to_string(), "0.0");
    const std::shared_ptr<json_lib::json> obj4
        = std::make_shared<json_lib::json_real>(-273.15f);
    EXPECT_EQ(obj4->type(), json_lib::json_type::real_json);
    EXPECT_EQ(obj4->to_string(), "-273.149994");
    const std::shared_ptr<json_lib::json> obj5
        = std::make_shared<json_lib::json_real>(36.6f);
    EXPECT_EQ(obj5->type(), json_lib::json_type::real_json);
    EXPECT_EQ(obj5->to_string(), "36.599998");
    const std::shared_ptr<json_lib::json> obj6
        = std::make_shared<json_lib::json_real>(
            std::numeric_limits<float>::epsilon()
        );
    EXPECT_EQ(obj6->type(), json_lib::json_type::real_json);
    EXPECT_EQ(obj6->to_string(), "0.0");
    const std::shared_ptr<json_lib::json> obj7
        = std::make_shared<json_lib::json_real>(
            std::numeric_limits<float>::lowest()
        );
    EXPECT_EQ(obj7->type(), json_lib::json_type::real_json);
    EXPECT_EQ(obj7->to_string(), "-340282346638528859811704183484516925440.0");
    EXPECT_EQ(obj4->to_string(0, true), "-273.149994");
    EXPECT_EQ(obj5->to_string(2, true), "36.599998");
    const std::shared_ptr<json_lib::json> obj8
        = std::make_shared<json_lib::json_real>(
            std::numeric_limits<float>::denorm_min()
        );
    EXPECT_EQ(obj8->to_string(), "0.0");
}

TEST(JsonTest, RealJsonTestWithString) {
    const std::shared_ptr<json_lib::json> obj1
        = std::make_shared<json_lib::json_real>("0.0");
    EXPECT_EQ(obj1->type(), json_lib::json_type::real_json);
    EXPECT_EQ(obj1->to_string(), "0.0");
    const std::shared_ptr<json_lib::json> obj2
        = std::make_shared<json_lib::json_real>(
            std::to_string(std::numeric_limits<float>::max())
        );
    EXPECT_EQ(obj2->type(), json_lib::json_type::real_json);
    EXPECT_EQ(
        obj2->to_string(), "340282346638528859811704183484516925440.000000"
    );
    const std::shared_ptr<json_lib::json> obj3
        = std::make_shared<json_lib::json_real>(
            std::to_string(std::numeric_limits<float>::denorm_min())
        );
    EXPECT_EQ(obj3->type(), json_lib::json_type::real_json);
    EXPECT_EQ(obj3->to_string(), "0.000000");
    const std::shared_ptr<json_lib::json> obj4
        = std::make_shared<json_lib::json_real>("-273.15");
    EXPECT_EQ(obj4->type(), json_lib::json_type::real_json);
    EXPECT_EQ(obj4->to_string(), "-273.15");
    const std::shared_ptr<json_lib::json> obj5
        = std::make_shared<json_lib::json_real>("36.6");
    EXPECT_EQ(obj5->type(), json_lib::json_type::real_json);
    EXPECT_EQ(obj5->to_string(), "36.6");
    const std::shared_ptr<json_lib::json> obj6
        = std::make_shared<json_lib::json_real>(
            std::to_string(std::numeric_limits<float>::epsilon())
        );
    EXPECT_EQ(obj6->type(), json_lib::json_type::real_json);
    EXPECT_EQ(obj6->to_string(), "0.000000");
    const std::shared_ptr<json_lib::json> obj7
        = std::make_shared<json_lib::json_real>(
            std::to_string(std::numeric_limits<float>::lowest())
        );
    EXPECT_EQ(obj7->type(), json_lib::json_type::real_json);
    EXPECT_EQ(
        obj7->to_string(), "-340282346638528859811704183484516925440.000000"
    );
    EXPECT_EQ(obj4->to_string(0, true), "-273.15");
    EXPECT_EQ(obj5->to_string(2, true), "36.6");
    const std::shared_ptr<json_lib::json> obj8
        = std::make_shared<json_lib::json_real>(
            std::to_string(std::numeric_limits<float>::denorm_min())
        );
    EXPECT_EQ(obj8->to_string(), "0.000000");
    const std::shared_ptr<json_lib::json> obj9
        = std::make_shared<json_lib::json_real>("1e12");
    EXPECT_EQ(obj9->type(), json_lib::json_type::real_json);
    EXPECT_EQ(obj9->to_string(), "1e12");
    const std::shared_ptr<json_lib::json> obj10
        = std::make_shared<json_lib::json_real>("-3E-7");
    EXPECT_EQ(obj10->type(), json_lib::json_type::real_json);
    EXPECT_EQ(obj10->to_string(), "-3E-7");
    const std::shared_ptr<json_lib::json> obj11
        = std::make_shared<json_lib::json_real>("5.67E+24");
    EXPECT_EQ(obj11->type(), json_lib::json_type::real_json);
    EXPECT_EQ(obj11->to_string(), "5.67E+24");
    const std::shared_ptr<json_lib::json> obj12
        = std::make_shared<json_lib::json_real>("-9.81e1");
    EXPECT_EQ(obj12->type(), json_lib::json_type::real_json);
    EXPECT_EQ(obj12->to_string(), "-9.81e1");
    const std::shared_ptr<json_lib::json> obj13
        = std::make_shared<json_lib::json_real>("3.14159");
    EXPECT_EQ(obj13->type(), json_lib::json_type::real_json);
    EXPECT_EQ(obj13->to_string(), "3.14159");
    const std::shared_ptr<json_lib::json> obj14
        = std::make_shared<json_lib::json_real>("2.71828");
    EXPECT_EQ(obj14->type(), json_lib::json_type::real_json);
    EXPECT_EQ(obj14->to_string(), "2.71828");
    EXPECT_EQ(obj9->to_string(), "1e12");
    EXPECT_EQ(obj10->to_string(), "-3E-7");
    EXPECT_EQ(obj11->to_string(), "5.67E+24");
    EXPECT_EQ(obj12->to_string(), "-9.81e1");
    std::shared_ptr<json_lib::json> res;
    EXPECT_THROW(
        res = std::make_shared<json_lib::json_real>("not_a_number"),
        std::invalid_argument
    );
    EXPECT_EQ(res, nullptr);
    EXPECT_THROW(
        res = std::make_shared<json_lib::json_real>(""), std::invalid_argument
    );
    EXPECT_EQ(res, nullptr);
    EXPECT_THROW(
        res = std::make_shared<json_lib::json_real>("   "),
        std::invalid_argument
    );
    EXPECT_EQ(res, nullptr);
    EXPECT_THROW(
        res = std::make_shared<json_lib::json_real>("123abc"),
        std::invalid_argument
    );
    EXPECT_EQ(res, nullptr);
    EXPECT_THROW(
        res = std::make_shared<json_lib::json_real>("1e"), std::invalid_argument
    );
    EXPECT_EQ(res, nullptr);
    EXPECT_THROW(
        res = std::make_shared<json_lib::json_real>("1e1000"), std::out_of_range
    );
    EXPECT_EQ(res, nullptr);
    EXPECT_THROW(
        res = std::make_shared<json_lib::json_real>("-1e1000"),
        std::out_of_range
    );
    EXPECT_EQ(res, nullptr);
}

TEST(JsonTest, StringJsonTest) {
    const std::shared_ptr<json_lib::json> obj1
        = std::make_shared<json_lib::json_string>("Hello, world!");
    EXPECT_EQ(obj1->type(), json_lib::json_type::string_json);
    EXPECT_EQ(obj1->to_string(), "\"Hello, world!\"");
    const std::shared_ptr<json_lib::json> obj2
        = std::make_shared<json_lib::json_string>(
            "Line 1\nLine 2\tTabbed\rCarriage\bBackspace\fFormFeed"
        );
    EXPECT_EQ(obj2->type(), json_lib::json_type::string_json);
    EXPECT_EQ(
        obj2->to_string(),
        "\"Line 1\\nLine 2\\tTabbed\\rCarriage\\bBackspace\\fFormFeed\""
    );
    const std::shared_ptr<json_lib::json> obj3
        = std::make_shared<json_lib::json_string>(
            "Special !@#$%^&*()_+-=[]{};:'\"\\|,<.>/?`~"
        );
    EXPECT_EQ(obj3->type(), json_lib::json_type::string_json);
    EXPECT_EQ(
        obj3->to_string(), "\"Special !@#$%^&*()_+-=[]{};:'\\\"\\\\|,<.>/?`~\""
    );
    const std::shared_ptr<json_lib::json> obj4
        = std::make_shared<json_lib::json_string>("Digits 0123456789");
    EXPECT_EQ(obj4->type(), json_lib::json_type::string_json);
    EXPECT_EQ(obj4->to_string(), "\"Digits 0123456789\"");
    const std::shared_ptr<json_lib::json> obj5
        = std::make_shared<json_lib::json_string>(
            "All ASCII chars: !\"#$%&'()*+,-./0123456789:;<=>?@[\\]^_`{|}~"
        );
    EXPECT_EQ(obj5->type(), json_lib::json_type::string_json);
    EXPECT_EQ(
        obj5->to_string(),
        "\"All ASCII chars: !\\\"#$%&'()*+,-./0123456789:;<=>?@[\\\\]^_`{|}~\""
    );
    const std::shared_ptr<json_lib::json> obj6
        = std::make_shared<json_lib::json_string>(
            "Non-ASCII: üñîçødé, 中文, العربية"
        );
    EXPECT_EQ(obj6->type(), json_lib::json_type::string_json);
    EXPECT_EQ(obj6->to_string(), "\"Non-ASCII: üñîçødé, 中文, العربية\"");
    EXPECT_EQ(obj1->to_string(0, true), "\"Hello, world!\"");
    EXPECT_EQ(
        obj2->to_string(2, true),
        "\"Line 1\\nLine 2\\tTabbed\\rCarriage\\bBackspace\\fFormFeed\""
    );
    EXPECT_EQ(
        obj3->to_string(4, true),
        "\"Special !@#$%^&*()_+-=[]{};:'\\\"\\\\|,<.>/?`~\""
    );
    const std::shared_ptr<json_lib::json_string> str_obj1
        = std::dynamic_pointer_cast<json_lib::json_string>(obj1);
    EXPECT_EQ(str_obj1->as_key(), "Hello, world!");
    const std::shared_ptr<json_lib::json_string> str_obj6
        = std::dynamic_pointer_cast<json_lib::json_string>(obj6);
    EXPECT_EQ(str_obj6->as_key(), "Non-ASCII: üñîçødé, 中文, العربية");
}

TEST(JsonTest, ArrayJsonTest) {
    const bool enable_negative_indexing_copy
        = json_lib::enable_negative_indexing;
    const auto obj1 = std::make_shared<json_lib::json_array>();
    EXPECT_EQ(obj1->type(), json_lib::json_type::array_json);
    EXPECT_EQ(obj1->size(), 0);
    EXPECT_EQ(obj1->to_string(0, false), "[]");
    EXPECT_EQ(obj1->to_string(0, true), "[]");
    std::vector<std::shared_ptr<json_lib::json>> values
        = { std::make_shared<json_lib::json_integer>(1),
            std::make_shared<json_lib::json_real>(1.f),
            std::make_shared<json_lib::json_boolean>(true),
            std::make_shared<json_lib::json_string>("test") };
    const auto obj2 = std::make_shared<json_lib::json_array>(values);
    EXPECT_EQ(obj2->type(), json_lib::json_type::array_json);
    EXPECT_EQ(obj2->size(), 4);
    EXPECT_EQ(obj2->at(0)->to_string(), "1");
    EXPECT_EQ(obj2->at(1)->to_string(), "1.0");
    EXPECT_EQ(obj2->at(2)->to_string(), "true");
    EXPECT_EQ(obj2->at(3)->to_string(), "\"test\"");
    EXPECT_EQ(obj2->to_string(0, false), "[1, 1.0, true, \"test\"]");
    EXPECT_EQ(obj2->to_string(1, true), "[1, 1.0, true, \"test\"]");
    json_lib::enable_negative_indexing = true;
    EXPECT_EQ(obj2->at(-1)->to_string(), "\"test\"");
    EXPECT_EQ(obj2->at(-2)->to_string(), "true");
    EXPECT_EQ(obj2->at(-3)->to_string(), "1.0");
    EXPECT_EQ(obj2->at(-4)->to_string(), "1");
    std::shared_ptr<json_lib::json> res;
    json_lib::enable_negative_indexing = false;
    EXPECT_THROW(res = obj2->at(-1), std::out_of_range);
    EXPECT_EQ(res, nullptr);
    EXPECT_THROW(res = obj2->at(4), std::out_of_range);
    EXPECT_EQ(res, nullptr);
    json_lib::enable_negative_indexing = true;
    EXPECT_THROW(res = obj2->at(-5), std::out_of_range);
    EXPECT_EQ(res, nullptr);
    json_lib::enable_negative_indexing = enable_negative_indexing_copy;
    std::vector<std::shared_ptr<json_lib::json>> nested_values
        = { std::make_shared<json_lib::json_array>(values),
            std::make_shared<json_lib::json_integer>(42) };
    const auto obj3 = std::make_shared<json_lib::json_array>(nested_values);
    EXPECT_EQ(obj3->to_string(0, false), "[[1, 1.0, true, \"test\"], 42]");
    EXPECT_EQ(
        obj3->to_string(1, true),
        "[\n\t\t[1, 1.0, true, \"test\"],\n\t\t42\n\t]"
    );
    EXPECT_EQ(obj3->size(), 2);
    const auto nested_array = obj3->at(0);
    EXPECT_EQ(nested_array->type(), json_lib::json_type::array_json);
    EXPECT_EQ(nested_array->to_string(), "[1, 1.0, true, \"test\"]");
    EXPECT_EQ(nested_array->to_string(1, true), "[1, 1.0, true, \"test\"]");
    json_lib::enable_negative_indexing = enable_negative_indexing_copy;
}

TEST(JsonTest, ObjectJsonTest) {
    const auto obj1 = std::make_shared<json_lib::json_object>();
    EXPECT_EQ(obj1->type(), json_lib::json_type::object_json);
    EXPECT_EQ(obj1->size(), 0);
    EXPECT_EQ(obj1->to_string(0, false), "{}");
    EXPECT_EQ(obj1->to_string(0, true), "{}");
    std::vector<std::pair<std::string, std::shared_ptr<json_lib::json>>> values
        = { { "integer", std::make_shared<json_lib::json_integer>(42) },
            { "boolean", std::make_shared<json_lib::json_boolean>(true) },
            { "string",
              std::make_shared<json_lib::json_string>("Hello World") } };
    const auto obj2 = std::make_shared<json_lib::json_object>(values);
    EXPECT_EQ(obj2->type(), json_lib::json_type::object_json);
    EXPECT_EQ(obj2->size(), 3);
    EXPECT_EQ(obj2->at("integer")->to_string(), "42");
    EXPECT_EQ(obj2->at("boolean")->to_string(), "true");
    EXPECT_EQ(obj2->at("string")->to_string(), "\"Hello World\"");
    EXPECT_EQ(
        obj2->to_string(0, false),
        "{\"integer\": 42, \"boolean\": true, \"string\": \"Hello World\"}"
    );
    EXPECT_EQ(
        obj2->to_string(1, true),
        "{\n\t\t\"integer\": 42,\n\t\t\"boolean\": true,\n\t\t\"string\": "
        "\"Hello World\"\n\t}"
    );
    std::vector<std::string> expected_keys = { "string", "boolean", "integer" };
    EXPECT_EQ(obj2->get_keys(), expected_keys);
    std::shared_ptr<json_lib::json> res;
    EXPECT_THROW(res = obj2->at("nonexistent"), std::out_of_range);
    EXPECT_EQ(res, nullptr);
    EXPECT_EQ(obj1->size(), 0);
    EXPECT_THROW(res = obj1->at("nonexistent"), std::out_of_range);
    EXPECT_EQ(res, nullptr);
    std::vector<std::pair<std::string, std::shared_ptr<json_lib::json>>>
        simple_number_object
        = { { "number", std::make_shared<json_lib::json_integer>(10) } };
    const auto simple_number_obj
        = std::make_shared<json_lib::json_object>(simple_number_object);
    EXPECT_EQ(simple_number_obj->to_string(0, true), "{\"number\": 10}");
    std::vector<std::pair<std::string, std::shared_ptr<json_lib::json>>>
        simple_string_object
        = { { "text",
              std::make_shared<json_lib::json_string>("Sample Text") } };
    const auto simple_string_obj
        = std::make_shared<json_lib::json_object>(simple_string_object);
    EXPECT_EQ(
        simple_string_obj->to_string(0, true), "{\"text\": \"Sample Text\"}"
    );
    std::vector<std::pair<std::string, std::shared_ptr<json_lib::json>>>
        simple_null_object
        = { { "nullValue", std::make_shared<json_lib::json>() } };
    const auto simple_null_obj
        = std::make_shared<json_lib::json_object>(simple_null_object);
    EXPECT_EQ(simple_null_obj->to_string(0, true), "{\"nullValue\": null}");
    std::vector<std::pair<std::string, std::shared_ptr<json_lib::json>>>
        simple_array_object
        = { { "array",
              std::make_shared<json_lib::json_array>(
                  std::vector<std::shared_ptr<json_lib::json>> {
                      std::make_shared<json_lib::json_integer>(1),
                      std::make_shared<json_lib::json_integer>(2),
                      std::make_shared<json_lib::json_integer>(3) }
              ) } };
    const auto simple_array_obj
        = std::make_shared<json_lib::json_object>(simple_array_object);
    EXPECT_EQ(
        simple_array_obj->to_string(0, true), "{\n\t\"array\": [1, 2, 3]\n}"
    );
    std::vector<std::pair<std::string, std::shared_ptr<json_lib::json>>>
        simple_nested_object
        = { { "nested",
              std::make_shared<json_lib::json_object>(
                  std::vector<
                      std::pair<std::string, std::shared_ptr<json_lib::json>>> {
                      { "key",
                        std::make_shared<json_lib::json_string>("value") } }
              ) } };
    const auto simple_nested_obj
        = std::make_shared<json_lib::json_object>(simple_nested_object);
    EXPECT_EQ(
        simple_nested_obj->to_string(0, true),
        "{\n\t\"nested\": {\"key\": \"value\"}\n}"
    );
    std::vector<std::pair<std::string, std::shared_ptr<json_lib::json>>>
        nested_values
        = { { "nested_object",
              std::make_shared<json_lib::json_object>(values) },
            { "array",
              std::make_shared<json_lib::json_array>(
                  std::vector<std::shared_ptr<json_lib::json>> {
                      std::make_shared<json_lib::json_integer>(1),
                      std::make_shared<json_lib::json_string>("Item") }
              ) } };
    const auto obj3 = std::make_shared<json_lib::json_object>(nested_values);
    EXPECT_EQ(
        obj3->to_string(0, false),
        "{\"nested_object\": {\"integer\": 42, \"boolean\": true, \"string\": "
        "\"Hello World\"}, \"array\": [1, \"Item\"]}"
    );
    EXPECT_EQ(
        obj3->to_string(1, true),
        "{\n\t\t\"nested_object\": {\n\t\t\t\"integer\": "
        "42,\n\t\t\t\"boolean\": true,\n\t\t\t\"string\": \"Hello "
        "World\"\n\t\t},\n\t\t\"array\": [1, \"Item\"]\n\t}"
    );
    const auto nested_object = obj3->at("nested_object");
    EXPECT_EQ(nested_object->type(), json_lib::json_type::object_json);
    EXPECT_EQ(
        nested_object->to_string(),
        "{\"integer\": 42, \"boolean\": true, \"string\": \"Hello World\"}"
    );
}

TEST(JsonTest, JsonByJsonTest) {
    const bool enable_symmetric_indexing_copy
        = json_lib::enable_symmetric_indexing;
    const auto json_null = std::make_shared<json_lib::json>();
    const auto json_bool = std::make_shared<json_lib::json_boolean>(true);
    const auto json_int = std::make_shared<json_lib::json_integer>(2);
    const auto json_real = std::make_shared<json_lib::json_real>(2.0f);
    const auto json_str = std::make_shared<json_lib::json_string>("key");
    const auto json_arr = std::make_shared<json_lib::json_array>(
        std::vector<std::shared_ptr<json_lib::json>> {
            std::make_shared<json_lib::json_integer>(10),
            std::make_shared<json_lib::json_integer>(20),
            std::make_shared<json_lib::json_integer>(30) }
    );
    const auto json_obj = std::make_shared<json_lib::json_object>(
        std::vector<std::pair<std::string, std::shared_ptr<json_lib::json>>> {
            { "key", std::make_shared<json_lib::json_integer>(42) },
            { "flag", std::make_shared<json_lib::json_boolean>(true) } }
    );
    std::vector<std::shared_ptr<json_lib::json>> json_objects
        = { json_null, json_bool, json_int, json_real,
            json_str,  json_arr,  json_obj };
    std::shared_ptr<json_lib::json> null_res;
    std::shared_ptr<json_lib::json> res;
    json_lib::enable_symmetric_indexing = false;
    for (const auto& obj1 : json_objects) {
        for (const auto& obj2 : json_objects) {
            if (obj1 == json_arr && obj2 == json_int) {
                EXPECT_NO_THROW(res = obj1->by(obj2));
                EXPECT_EQ(res->to_string(), "30");
                const auto invalid_num
                    = std::make_shared<json_lib::json_integer>(5);
                EXPECT_THROW(
                    null_res = obj1->by(invalid_num), std::out_of_range
                );
                EXPECT_EQ(null_res, nullptr);
            } else if (obj1 == json_obj && obj2 == json_str) {
                EXPECT_NO_THROW(res = obj1->by(obj2));
                EXPECT_EQ(res->to_string(), "42");
                const auto invalid_str
                    = std::make_shared<json_lib::json_string>("invalid_key");
                EXPECT_THROW(
                    null_res = obj1->by(invalid_str), std::out_of_range
                );
                EXPECT_EQ(null_res, nullptr);
            } else {
                EXPECT_THROW(null_res = obj1->by(obj2), std::invalid_argument);
                EXPECT_EQ(null_res, nullptr);
            }
        }
    }
    json_lib::enable_symmetric_indexing = true;
    EXPECT_NO_THROW(res = json_int->by(json_arr));
    EXPECT_EQ(res->to_string(), "30");
    EXPECT_NO_THROW(res = json_str->by(json_obj));
    EXPECT_EQ(res->to_string(), "42");
    EXPECT_THROW(res = json_bool->by(json_int), std::invalid_argument);
    EXPECT_EQ(null_res, nullptr);
    json_lib::enable_symmetric_indexing = enable_symmetric_indexing_copy;
}

// TEST(JsonTest, JsonEmplaceBackTest) {
//     const bool enable_symmetric_indexing_copy
//         = json_lib::enable_symmetric_indexing;
//     auto json_arr = std::make_shared<json_lib::json_array>();
//     auto json_obj = std::make_shared<json_lib::json_object>();
//
//     json_arr->emplace_back(std::make_shared<json_lib::json_boolean>(true));
//     json_arr->emplace_back(std::make_shared<json_lib::json_integer>(42));
//     json_arr->emplace_back(std::make_shared<json_lib::json_real>(3.14f));
//     json_arr->emplace_back(std::make_shared<json_lib::json_string>("Test
//     String"
//     ));
//     json_arr->emplace_back(std::make_shared<json_lib::json_array>());
//     json_arr->emplace_back(std::make_shared<json_lib::json_object>());
//
//     EXPECT_EQ(json_arr->size(), 6);
//     EXPECT_EQ(json_arr->type(), json_lib::json_type::array_json);
//     EXPECT_EQ(
//         json_arr->to_string(0, false),
//         "[true, 42, 3.14, \"Test String\", [], {}]"
//     );
//
//     json_obj->emplace_back(
//         "bool_key", std::make_shared<json_lib::json_boolean>(true)
//     );
//     EXPECT_THROW(
//         json_obj->emplace_back(
//             "bool_key", std::make_shared<json_lib::json_boolean>(false)
//         ),
//         std::invalid_argument
//     );
//     json_obj->emplace_back(
//         "int_key", std::make_shared<json_lib::json_integer>(42)
//     );
//     json_obj->emplace_back(
//         "real_key", std::make_shared<json_lib::json_real>(3.14f)
//     );
//     json_obj->emplace_back(
//         "string_key", std::make_shared<json_lib::json_string>("Test String")
//     );
//     json_obj->emplace_back(
//         "array_key", std::make_shared<json_lib::json_array>()
//     );
//     json_obj->emplace_back(
//         "object_key", std::make_shared<json_lib::json_object>()
//     );
//
//     EXPECT_EQ(json_obj->size(), 6);
//     EXPECT_EQ(json_obj->type(), json_lib::json_type::object_json);
//     EXPECT_EQ(
//         json_obj->to_string(0, false),
//         "{\"bool_key\": true, \"int_key\": 42, \"real_key\": 3.14, "
//         "\"string_key\": \"Test String\", \"array_key\": [], \"object_key\":
//         "
//         "{}}"
//     );
//
//     json_arr->emplace_back(json_arr);
//     EXPECT_THROW(json_arr->to_string(0, false), std::runtime_error);
//
//     json_obj->emplace_back("self_ref", json_obj);
//     EXPECT_THROW(json_obj->to_string(0, false), std::runtime_error);
//
//     std::shared_ptr<json_lib::json> res;
//     EXPECT_EQ(json_arr->at(0)->type(), json_lib::json_type::boolean_json);
//     EXPECT_THROW(res = json_arr->at(-1), std::out_of_range);
//     EXPECT_EQ(res, nullptr);
//
//     EXPECT_EQ(
//         json_obj->at("int_key")->type(), json_lib::json_type::integer_json
//     );
//     EXPECT_THROW(res = json_obj->at("missing_key"), std::out_of_range);
//     EXPECT_EQ(res, nullptr);
//
//     EXPECT_THROW(res = json_arr->by(json_arr->at(2)), std::invalid_argument);
//     EXPECT_THROW(
//         res =
//         json_obj->by(std::make_shared<json_lib::json_string>("invalid")),
//         std::out_of_range
//     );
//     EXPECT_EQ(res, nullptr);
//
//     json_lib::enable_symmetric_indexing = true;
//     EXPECT_EQ(
//         std::make_shared<json_lib::json_string>("bool_key")
//             ->by(json_obj)
//             ->to_string(),
//         "true"
//     );
//     EXPECT_THROW(
//         res = std::make_shared<json_lib::json_string>("bool_key")
//                   ->by(json_obj)
//                   ->by(json_arr),
//         std::invalid_argument
//     );
//     EXPECT_EQ(res, nullptr);
//     json_lib::enable_symmetric_indexing = enable_symmetric_indexing_copy;
// }
