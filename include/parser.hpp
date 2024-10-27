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

#ifndef PARSER_HPP
#define PARSER_HPP
#include <filesystem>
#include <fstream>

#include "json.hpp"

class parser {
public:
    explicit parser(std::string& buffer);
    explicit parser(const std::stringstream& ss);
    explicit parser(std::ifstream& input_stream);
    explicit parser(const std::filesystem::path& path);

    void completely_parse_json(
        std::shared_ptr<json_lib::json>& result, bool is_dynamic = false
    );

    void parse_evaluate_json(
        std::shared_ptr<json_lib::json>& root,
        std::shared_ptr<json_lib::json>& result
    );

private:
    std::ifstream ifs;
    std::string buffer;
    int pos { -1 };
    int line { -1 };

    void read_line();
    void next();
    bool valid() const;
    char peek() const;
    char get();

    void skip_whitespace();
    bool is_enumeration();
    std::string parse_keyword();
    std::string parse_string();
    std::tuple<std::string, bool> parse_number();

    void parse_array(std::shared_ptr<json_lib::json>& result, bool is_dynamic);
    void parse_object(std::shared_ptr<json_lib::json>& result, bool is_dynamic);
    void parse_json(std::shared_ptr<json_lib::json>& result, bool is_dynamic);
    void parse_path(std::vector<std::shared_ptr<json_lib::json>>& keys);

    std::runtime_error throw_message(
        const std::string& message,
        std::source_location location = std::source_location::current()
    ) const;
};

#endif // PARSER_HPP
