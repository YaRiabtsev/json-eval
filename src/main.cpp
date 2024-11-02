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

#include <iostream>

#include "parser.hpp"

//
// int main(const int argc, char* argv[]) {
//     if (argc != 3) {
//         std::cerr << "Usage: " << argv[0] << " <json-file> <json-path>\n";
//         return 1;
//     }
//
//     std::shared_ptr<json_lib::json> base_json;
//     const std::filesystem::path input_file(argv[1]);
//     parser_lib::parser prs(input_file);
//     prs.completely_parse_json(base_json);
//
//     std::shared_ptr<json_lib::json> raw_json;
//     std::string expr = argv[2];
//     prs = parser_lib::parser(expr);
//     prs.completely_parse_json(raw_json, true);
//
//     std::cout << raw_json->to_string() << std::endl;
//
//     return 0;
// }
int main() {
    std::shared_ptr<json_lib::json> result;
    std::string buffer = "{}";
    parser_lib::parser p(buffer);
    // R"({"a":1, "b":2, "c":3}{.b, .c, .a})";
    // std::cout << 1 << std::endl;
    // {
    //
    // buffer = R"($.key[0]["key1"][1,2]{.key2, .key3}{.a.b.c[0], .a.b.c[1],
    // .a.b.c[2]})"; p = parser_lib::parser(buffer);
    // p.completely_parse_json(result, true);
    // std::cout << result->to_string() << std::endl << std::endl;

    buffer = R"({"key1":5, "key2":@, "key3":55}.key2.key1)";
    p = parser_lib::parser(buffer);
    p.completely_parse_json(result, true);
    std::cout << result->to_string() << std::endl;
    //
    // buffer = R"([fu(), fu(1), fu([1,2,3]), fu({}), fu(@.key, 5),
    // fu().key.key, a.fu(1,2)1])"; p = parser_lib::parser(buffer);
    // p.completely_parse_json(result, true);
    // std::cout << result->to_string() << std::endl;
    //
    // buffer = R"($.a.b.c.d)";
    // p = parser_lib::parser(buffer);
    // p.completely_parse_json(result, true);
    // std::cout << result->to_string() << std::endl;
    // }
    // std::cout << 2 << std::endl;
    // {
    // buffer = R"([17, 314, 51]{[0], [2], [1]})";
    // p = parser_lib::parser(buffer);
    // p.completely_parse_json(result, true);
    // std::cout << result->to_string() << std::endl;
    //
    //     buffer = R"({"key" :5}{.key})";
    //     p = parser_lib::parser(buffer);
    //     p.completely_parse_json(result, true);
    //     std::cout << result->to_string() << std::endl;
    //
    //     buffer = R"({"key" :5}{["key"]})";
    //     p = parser_lib::parser(buffer);
    //     p.completely_parse_json(result, true);
    //     std::cout << result->to_string() << std::endl;
    // }

    return 0;
}
