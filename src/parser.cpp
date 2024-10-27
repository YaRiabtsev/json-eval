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

std::runtime_error parser::throw_message(
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

parser::parser(std::string& buffer)
    : buffer(std::move(buffer))
    , pos(0)
    , line(0) { }

// parser::parser(const std::stringstream& ss)
//     : buffer(ss.str())
//     , pos(0)
//     , line(0) { }
//
// parser::parser(std::ifstream& input_stream)
//     : ifs(move(input_stream)) {
//     read_line();
// }

parser::parser(const std::filesystem::path& path) {
    ifs.open(path);
    if (!ifs.is_open()) {
        throw std::invalid_argument(
            "failed to open file with path: " + path.string()
        );
    }
    read_line();
}

void parser::read_line() {
    ++line;
    if (!ifs.is_open()) {
        pos = -1;
        buffer.clear();
        return;
    }
    if (ifs.eof()) {
        ifs.close();
        buffer.clear();
        pos = -1;
    } else {
        std::getline(ifs, buffer);
        if (line > 0) {
            buffer = "\n" + buffer;
        }
        pos = 0;
    }
}

void parser::next() {
    assert(pos >= 0 && "stream is not empty");
    ++pos;
    if (const auto index = static_cast<size_t>(pos); index >= buffer.size()) {
        read_line();
    }
}

bool parser::valid() const {
    if (pos < 0) {
        return false;
    }
    const auto index = static_cast<size_t>(pos);
    return index < buffer.size();
}

char parser::peek() const {
    assert(valid() && "stream and position are valid");
    const auto index = static_cast<size_t>(pos);
    return buffer[index];
}

char parser::get() {
    const char peek_char = peek();
    next();
    return peek_char;
}

void parser::skip_whitespace() {
    while (valid() && std::isspace(peek())) {
        next();
    }
    if (valid() && peek() == '/') {
        next();
        if (valid() && peek() == '/') {
            while (valid() && peek() != '\n') {
                get();
            }
            skip_whitespace();
        } else {
            throw throw_message("invalid json");
        }
    }
}

bool parser::is_enumeration() {
    if (valid() && peek() == ',') {
        next();
        return true;
    }
    return false;
}

std::string parser::parse_keyword() {
    std::string keyword;
    assert(
        valid() && std::isalpha(peek())
        && "expected keyword starting with a letter"
    );
    do {
        keyword += get();
    } while (valid() && (std::isalnum(peek()) || peek() == '_'));
    return keyword;
}

std::string parser::parse_string() {
    skip_whitespace();
    assert(valid() && peek() == '\"' && "expected opening quote");
    next();
    std::string value;
    bool is_backslash = false;
    while (valid()) {
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
    }
    if (!valid() || peek() != '\"') {
        throw throw_message("invalid JSON string: Missing closing quote");
    }
    next();
    return value;
}

std::tuple<std::string, bool> parser::parse_number() {
    skip_whitespace();
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

void parser::parse_array(
    std::shared_ptr<json_lib::json>& result, const bool is_dynamic
) {
    skip_whitespace();
    assert(valid() && peek() == '[' && "array starts with opening bracket `[`");
    next();
    std::vector<std::shared_ptr<json_lib::json>> children;
    skip_whitespace();
    bool closed = (valid() && peek() == ']');
    while (valid() && !closed) {
        std::shared_ptr<json_lib::json> child;
        parse_json(child, is_dynamic);
        children.emplace_back(child);
        skip_whitespace();
        closed = !is_enumeration();
    }
    if (!valid() || peek() != ']') {
        throw throw_message("invalid json array");
    }
    next();
    result = std::make_shared<json_lib::json_array>(children);
}

void parser::parse_object(
    std::shared_ptr<json_lib::json>& result, const bool is_dynamic
) {
    skip_whitespace();
    assert(valid() && peek() == '{' && "object starts with opening brace `{`");
    next();
    std::vector<std::pair<std::string, std::shared_ptr<json_lib::json>>>
        children;
    skip_whitespace();
    bool closed = (valid() && peek() == '}');
    while (valid() && !closed) {
        // if is_dynamic == false:
        skip_whitespace();
        if (!valid() || peek() != '\"') {
            throw throw_message("expected key as a string");
        }
        std::string key = parse_string();
        // todo: if (children.contains(key)) {
        //     throw throw_message("key overusing in the same json object");
        // }
        skip_whitespace();
        if (!valid() || peek() != ':') {
            throw throw_message("expected key-value separator for json object");
        }
        next();
        std::shared_ptr<json_lib::json> child;
        parse_json(child, is_dynamic);
        children.emplace_back(key, child);
        skip_whitespace();
        closed = !is_enumeration();
    }
    if (!valid() || peek() != '}') {
        throw throw_message("invalid json array");
    }
    next();
    result = std::make_shared<json_lib::json_object>(children);
}

// void parser::parse_path(std::vector<std::shared_ptr<json_lib::json>>& keys) {
//
// }

void parser::parse_json(
    std::shared_ptr<json_lib::json>& result, const bool is_dynamic
) {
    skip_whitespace();
    if (!valid()) {
        return;
    }
    // std::vector<std::shared_ptr<json_lib::json>> keys;
    if (std::isalpha(peek())) {
        if (const std::string keyword = parse_keyword(); keyword == "true") {
            result = std::make_shared<json_lib::json_boolean>(true);
        } else if (keyword == "false") {
            result = std::make_shared<json_lib::json_boolean>(false);
        } else if (keyword == "null") {
            result = std::make_shared<json_lib::json>();
        } else if (is_dynamic) {
            // todo: parse_path(path_obj) -> [key / idx];
            // base object is required
            // could be loop
            // todo: json += emplace_back() for json_array and _object
            // skip_whitespace();
            // if (valid() && peek() == '(') {
            // todo: parse_function()
            // } else {
            // keys.emplace_back(
            // std::make_shared<json_lib::json_string>(keyword)
            // );
            // }
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
        parse_array(result, is_dynamic);
    } else if (peek() == '{') {
        parse_object(result, is_dynamic);
    } else {
        throw throw_message("invalid json");
    }
    if (is_dynamic) {
        // todo: if peek == ']' -> parse_path(path_obj)
        // result is a constant object,
        // that is an nested object by path = path_obj,
        // where the base obj is current result

        // while (!keys.empty()) {
        // }
    }
}

void parser::completely_parse_json(
    std::shared_ptr<json_lib::json>& result, const bool is_dynamic
) {
    parse_json(result, is_dynamic);
    skip_whitespace();
    if (valid()) {
        throw throw_message("invalid json");
    }
    if (result == nullptr) {
        result = std::make_shared<json_lib::json>();
    }
}
