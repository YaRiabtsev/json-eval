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
#include "custom_json.hpp"

#include <filesystem>
#include <fstream>
#include <functional>

namespace parser_lib {
class parser {
public:
    explicit parser(std::string& buffer);
    explicit parser(const std::stringstream& ss);
    explicit parser(std::ifstream& is);
    explicit parser(const std::filesystem::path& path);

    void completely_parse_json(
        std::shared_ptr<json_lib::json>& result, bool dynamic = false
    );

private:
    std::ifstream ifs;
    std::string buffer;
    int pos { -1 };
    int line { -1 };

    size_t get_pos() const;
    bool valid() const;
    void next();
    void read_line();
    char peek() const;
    char get();
    bool check_ahead(char expected) const;
    bool separator(char val = ',');

    void nonessential();
    std::string parse_keyword();
    std::string parse_string();
    std::tuple<std::string, bool> parse_number();

    template <typename Container, typename ParseElement>
    Container
    parse_collection(bool dynamic, char halt, ParseElement parse_element);
    void parse_array_item(
        std::vector<std::shared_ptr<json_lib::json>>& children, bool dynamic
    );
    void parse_set_item(
        std::vector<std::shared_ptr<reference_lib::json_reference>>& children,
        bool dynamic
    );
    void parse_object_item(
        std::vector<std::pair<std::string, std::shared_ptr<json_lib::json>>>&
            children,
        bool dynamic
    );

    bool parse_accessor(std::shared_ptr<json_lib::json>& accessor);
    void parse_tail(const std::shared_ptr<reference_lib::json_reference>& result);
    void parse_reference(std::shared_ptr<json_lib::json>& result);
    void parse_json(std::shared_ptr<json_lib::json>& result, bool dynamic);

    std::runtime_error throw_message(
        const std::string& message,
        std::source_location location = std::source_location::current()
    ) const;
};

template <typename Container, typename ParseElement>
Container parser::parse_collection(
    const bool dynamic, const char halt, ParseElement parse_element
) {
    Container children;
    nonessential();
    bool closed = true;
    while (valid() && peek() != halt) {
        std::invoke(parse_element, *this, children, dynamic);
        closed = !separator();
    }
    if (!closed) {
        throw throw_message("expected one more item in enumerator");
    }
    if (!valid() || peek() != halt) {
        throw throw_message("enumerator-object is not closed");
    }
    next();
    return children;
}
}

#endif // PARSER_HPP
