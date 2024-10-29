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

#include <functional>
#include <variant>

namespace parser_lib {

class parser {
public:
    explicit parser(std::string& buffer);
    explicit parser(const std::stringstream& ss);
    explicit parser(std::ifstream& is);
    explicit parser(const std::filesystem::path& path);

    void completely_parse_json(
        std::shared_ptr<json_lib::json>& result, bool is_dynamic = false
    );

private:
    std::ifstream ifs;
    std::string buffer;
    int pos { -1 };
    int line { -1 };

    size_t get_pos() const;
    void read_line();
    void next();
    bool valid() const;
    bool check_ahead(char expected) const;
    char peek() const;
    char get();

    void nonessential();
    bool separator(char val = ',');
    std::string parse_keyword();
    std::string parse_string();
    std::tuple<std::string, bool> parse_number();

    void parse_array_item(
        std::vector<std::shared_ptr<json_lib::json>>& children, bool is_dynamic
    );
    void parse_set_item(
        std::vector<std::shared_ptr<json_lib::json>>& children, bool is_dynamic
    );
    void parse_object_item(
        std::vector<std::pair<std::string, std::shared_ptr<json_lib::json>>>&
            children,
        bool is_dynamic
    );

    template <typename Container, typename ParseElement>
    Container
    parse_collection(bool is_dynamic, char halt, ParseElement parse_element);

    bool parse_tail(std::shared_ptr<json_lib::json>& result);
    void parse_json(std::shared_ptr<json_lib::json>& result, bool is_dynamic);
    bool parse_accessor(std::shared_ptr<json_lib::json>& acc);

    std::runtime_error throw_message(
        const std::string& message,
        std::source_location location = std::source_location::current()
    ) const;
};

template <typename Container, typename ParseElement>
Container parser::parse_collection(
    const bool is_dynamic, const char halt, ParseElement parse_element
) {
    Container children;
    nonessential();
    bool closed = true;
    while (valid() && peek() != halt) {
        std::invoke(parse_element, *this, children, is_dynamic);
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

class raw_json final : public json_lib::json {
public:
    explicit raw_json(const std::shared_ptr<json>& head);
    explicit raw_json(const std::string& name);
    [[nodiscard]] json_lib::json_type type() const override;
    void touch() override;
    std::string
    indented_string(size_t indent_level, bool is_pretty) const override;
    [[nodiscard]] size_t size() const;
    void to_function(const std::vector<std::shared_ptr<json>>& args);
    void emplace_back(const std::shared_ptr<json>& item);

private:
    bool looped { false };
    bool touched { false };
    bool is_function { false };
    std::variant<std::shared_ptr<json>, std::string> head;
    std::vector<std::shared_ptr<json>> tail {};
};
}

#endif // PARSER_HPP
