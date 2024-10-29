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

#include <cassert>
#include <utility>

std::runtime_error parser_lib::parser::throw_message(
    const std::string& message, const std::source_location location
) const {
    std::ostringstream oss;
    oss << "[Parser-Error] " << message << ". ";
#ifndef NDEBUG
    if (!ifs.is_open()) {
        oss << "No input stream open. ";
    }
    if (!valid()) {
        oss << "Position is out of range. Line: " << (line + 1)
            << ", position: " << (pos + 1) << " exceeds available input. ";
    } else {
        const char current_char = peek();
        oss << "Character '" << current_char
            << "' (ASCII: " << static_cast<int>(current_char)
            << ") was found at line " << (line + 1) << ", position "
            << (pos + 1) << ". ";
    }
    oss << "In file: " << location.file_name() << '(' << location.line() << ':'
        << location.column() << ") `" << location.function_name() << "`";
#endif
    return std::runtime_error(oss.str());
}

parser_lib::parser::parser(std::string& buffer)
    : buffer(std::move(buffer))
    , pos(0)
    , line(0) { }

parser_lib::parser::parser(const std::stringstream& ss)
    : buffer(ss.str())
    , pos(0)
    , line(0) { }

parser_lib::parser::parser(std::ifstream& is)
    : ifs(move(is)) {
    read_line();
}

parser_lib::parser::parser(const std::filesystem::path& path) {
    ifs.open(path);
    if (!ifs.is_open()) {
        throw std::invalid_argument(
            "failed to open file with path: " + path.string()
        );
    }
    read_line();
}

void parser_lib::parser::read_line() {
    ++line;
    buffer.clear();
    pos = -1;
    if (ifs.eof()) {
        ifs.close();
    }
    if (!ifs.is_open()) {
        return;
    }
    std::getline(ifs, buffer);
    buffer += '\n';
    pos = 0;
}

size_t parser_lib::parser::get_pos() const {
    assert(pos >= 0 && "stream is not empty");
    return static_cast<size_t>(pos);
}

void parser_lib::parser::next() {
    assert(pos >= 0 && "stream is not empty");
    ++pos;
    if (get_pos() >= buffer.size()) {
        read_line();
    }
}

bool parser_lib::parser::valid() const {
    return pos >= 0 && get_pos() < buffer.size();
}

bool parser_lib::parser::check_ahead(const char expected) const {
    if (!valid()) {
        return false;
    }
    const size_t next_index = get_pos() + 1;
    if (next_index >= buffer.size()) {
        return false;
    }
    return buffer[next_index] == expected;
}

char parser_lib::parser::peek() const { return buffer[get_pos()]; }

char parser_lib::parser::get() {
    const char peek_char = peek();
    next();
    return peek_char;
}

void parser_lib::parser::nonessential() {
    while (valid() && std::isspace(peek())) {
        next();
    }
    if (valid() && peek() == '/' && check_ahead('/')) {
        while (valid() && get() != '\n')
            ;
        nonessential();
    }
}

bool parser_lib::parser::separator(const char val) {
    nonessential();
    if (valid() && peek() == val) {
        next();
        nonessential();
        return true;
    }
    return false;
}

std::string parser_lib::parser::parse_keyword() {
    assert(
        valid() && (std::isalpha(peek()) || peek() == '_')
        && "expected keyword starting with a letter"
    );
    std::string keyword;
    do {
        keyword += get();
    } while (valid() && (std::isalnum(peek()) || peek() == '_'));
    return keyword;
}

std::string parser_lib::parser::parse_string() {
    assert(valid() && peek() == '\"' && "expected opening quote");
    next();
    std::string value;
    bool is_backslash = false;
    do {
        const char current = peek();
        if (is_backslash) {
            switch (current) {
            case '\"':
                value += '\"';
                break;
            case '\\':
                value += '\\';
                break;
            case '/':
                value += '/';
                break;
            case 'b':
                value += '\b';
                break;
            case 'f':
                value += '\f';
                break;
            case 'n':
                value += '\n';
                break;
            case 'r':
                value += '\r';
                break;
            case 't':
                value += '\t';
                break;
            case 'u': {
                std::string hex;
                for (int i = 0; i < 4; ++i) {
                    next();
                    if (!valid() || !isxdigit(peek())) {
                        throw throw_message(
                            "invalid Unicode escape sequence in JSON string"
                        );
                    }
                    hex += peek();
                }
                const int codepoint = std::stoi(hex, nullptr, 16);
                std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>
                    converter;
                value += converter.to_bytes(static_cast<char32_t>(codepoint));
                break;
            }
            default:
                throw throw_message("invalid escape sequence in JSON string");
            }
            is_backslash = false;
        } else if (current == '\\') {
            is_backslash = true;
        } else if (current == '\"') {
            break;
        } else {
            value += current;
        }
        next();
    } while (valid());
    if (!valid() || peek() != '\"') {
        throw throw_message("invalid JSON string: Missing closing quote");
    }
    next();
    return value;
}

std::tuple<std::string, bool> parser_lib::parser::parse_number() {
    nonessential();
    std::string number;
    bool is_float = false;
    if (valid() && peek() == '-') {
        number += get();
    }
    if (valid() && peek() == '0') {
        number += get();
        if (valid() && std::isdigit(peek())) {
            throw throw_message(
                "invalid number format: leading zeros are not allowed"
            );
        }
    } else if (valid() && std::isdigit(peek())) {
        do {
            number += get();
        } while (valid() && std::isdigit(peek()));
    } else {
        throw throw_message("invalid number format: expected digit");
    }
    if (valid() && peek() == '.') {
        is_float = true;
        number += get();
        if (!valid() || !std::isdigit(peek())) {
            throw throw_message(
                "invalid number format: expected digit after decimal"
            );
        }
        while (valid() && std::isdigit(peek())) {
            number += get();
        }
    }
    if (valid() && (peek() == 'e' || peek() == 'E')) {
        is_float = true;
        number += get();
        if (valid() && (peek() == '+' || peek() == '-')) {
            number += get();
        }
        if (!valid() || !std::isdigit(peek())) {
            throw throw_message(
                "invalid number format: expected digit after exponent"
            );
        }
        while (valid() && std::isdigit(peek())) {
            number += get();
        }
    }
    assert(!number.empty() && "number is not empty");
    return { number, is_float };
}

void parser_lib::parser::parse_array_item(
    std::vector<std::shared_ptr<json_lib::json>>& children,
    const bool is_dynamic
) {
    std::shared_ptr<json_lib::json> child;
    parse_json(child, is_dynamic);
    children.emplace_back(child);
}

void parser_lib::parser::parse_object_item(
    std::vector<std::pair<std::string, std::shared_ptr<json_lib::json>>>&
        children,
    const bool is_dynamic
) {
    if (!valid() || peek() != '\"') {
        throw throw_message("expected key as a string");
    }
    std::string key = parse_string();
    if (!separator(':')) {
        throw throw_message("expected key-value separator for json object");
    }
    std::shared_ptr<json_lib::json> value;
    parse_json(value, is_dynamic);
    children.emplace_back(key, value);
}

void parser_lib::parser::completely_parse_json(
    std::shared_ptr<json_lib::json>& result, const bool is_dynamic
) {
    parse_json(result, is_dynamic);
    nonessential();
    if (result == nullptr) {
        throw throw_message("json is empty");
    }
    if (valid()) {
        throw throw_message("invalid json");
    }
}

void parser_lib::parser::parse_set_item(
    std::vector<std::shared_ptr<json_lib::json>>& children, bool
) {
    std::shared_ptr<json_lib::json> child = std::make_shared<raw_json>("@");
    if (!parse_tail(child)) {
        throw throw_message("expected path");
    }
    children.emplace_back(child);
}

bool parser_lib::parser::parse_accessor(std::shared_ptr<json_lib::json>& acc) {
    nonessential();
    if (!valid()) {
        return false;
    }
    if (peek() == '.') {
        next();
        if (!std::isalpha(peek()) && peek() != '_') {
            throw throw_message("invalid const accessor");
        }
        const std::string keyword = parse_keyword();
        acc = std::make_shared<json_lib::json_string>(keyword);
    } else if (peek() == '[') {
        next();
        auto children
            = parse_collection<std::vector<std::shared_ptr<json_lib::json>>>(
                true, ']', &parser::parse_array_item
            );
        if (children.size() == 1) {
            acc = children[0];
        } else {
            for (auto& child : children) {
                const auto raw_child = std::make_shared<raw_json>("@");
                raw_child->emplace_back(child);
                child = raw_child;
            }
            acc = std::make_shared<json_lib::json_array>(children);
        }
    } else if (peek() == '{') {
        next();
        auto children
            = parse_collection<std::vector<std::shared_ptr<json_lib::json>>>(
                true, '}', &parser::parse_set_item
            );
        acc = std::make_shared<json_lib::json_array>(children);
    } else if (peek() == '(') {
        // auto children
        //     = parse_collection<std::vector<std::shared_ptr<json_lib::json>>>(
        //         true, ')', &parser::parse_array_item
        //     );
        // acc.to
    } else {
        return false;
    }
    return true;
}

bool parser_lib::parser::parse_tail(std::shared_ptr<json_lib::json>& result) {
    std::shared_ptr<raw_json> raw_result;
    if (result->type() == json_lib::json_type::custom_json) {
        raw_result = std::dynamic_pointer_cast<raw_json>(result);
    } else {
        raw_result = std::make_shared<raw_json>(result);
    }
    bool is_const = true;
    for (std::shared_ptr<json_lib::json> next; parse_accessor(next);) {
        raw_result->emplace_back(next);
        is_const = false;
    }
    if (!is_const) {
        result = raw_result;
    }
    return !is_const;
}

void parser_lib::parser::parse_json(
    std::shared_ptr<json_lib::json>& result, const bool is_dynamic
) {
    nonessential();
    if (!valid()) {
        return;
    }
    if (is_dynamic && (peek() == '@' || peek() == '#')) {
        std::string key;
        key += peek();
        result = std::make_shared<raw_json>(key);
        next();
    } else if (std::isalpha(peek()) || peek() == '_') {
        if (const std::string keyword = parse_keyword(); keyword == "true") {
            result = std::make_shared<json_lib::json_boolean>(true);
        } else if (keyword == "false") {
            result = std::make_shared<json_lib::json_boolean>(false);
        } else if (keyword == "null") {
            result = std::make_shared<json_lib::json>();
        } else if (is_dynamic) {
            result = std::make_shared<raw_json>(keyword);
        } else {
            throw throw_message("invalid json");
        }
    } else if (peek() == '-' || std::isdigit(peek())) {
        if (auto [number, is_float] = parse_number(); is_float) {
            // const float value = std::stof(number);
            result = std::make_shared<json_lib::json_real>(number);
        } else {
            const int value = std::stoi(number);
            result = std::make_shared<json_lib::json_integer>(value);
        }
    } else if (peek() == '\"') {
        result = std::make_shared<json_lib::json_string>(parse_string());
    } else if (peek() == '[') {
        next();
        auto children
            = parse_collection<std::vector<std::shared_ptr<json_lib::json>>>(
                is_dynamic, ']', &parser::parse_array_item
            );
        result = std::make_shared<json_lib::json_array>(children);
    } else if (peek() == '{') {
        next();
        auto children = parse_collection<std::vector<
            std::pair<std::string, std::shared_ptr<json_lib::json>>>>(
            is_dynamic, '}', &parser::parse_object_item
        );
        result = std::make_shared<json_lib::json_object>(children);
    } else if (is_dynamic && peek() == '(') {
        next();
        std::shared_ptr<json_lib::json> nested;
        parse_json(nested, is_dynamic);
        nonessential();
        if (!valid() || peek() != ')') {
            throw throw_message("invalid json");
        }
        result = std::make_shared<raw_json>(nested);
        next();
    } else {
        throw throw_message("invalid json");
    }
    if (is_dynamic) {
        parse_tail(result);
    }
}
