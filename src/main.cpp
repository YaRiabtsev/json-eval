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

int main(const int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <json-file> <json-path>\n";
        return 1;
    }

    std::shared_ptr<json_lib::json> base_json;
    const std::filesystem::path input_file(argv[1]);
    parser prs(input_file);
    prs.completely_parse_json(base_json);

    // parse path with base_json

    std::cout << base_json->to_string() << std::endl;
    // int main() {
    //     std::shared_ptr<json_lib::json> res =
    //     std::make_shared<json_lib::json>(); std::cout << res << ' ' <<
    //     res->to_string() << std::endl; std::shared_ptr<json_lib::json> arr
    //         = std::make_shared<json_lib::json_array>();
    //     std::cout << arr << ' ' << arr->to_string() << std::endl;
    //     *res = arr.get();
    //     std::cout << res << ' ' << res->to_string() << arr << ' '
    //               << arr->to_string() << std::endl;

    return 0;
}
