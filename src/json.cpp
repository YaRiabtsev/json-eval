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

#include <algorithm>
#include <ranges>
#include <sstream>

std::string json_lib::json_type_to_string(const json_type type) {
    switch (type) {
    case json_lib::json_type::object_json:
        return "JSON-Object";
    case json_lib::json_type::array_json:
        return "JSON-Array";
    case json_lib::json_type::string_json:
        return "JSON-String";
    case json_lib::json_type::real_json:
        return "JSON-Real";
    case json_lib::json_type::integer_json:
        return "JSON-Integer";
    case json_lib::json_type::boolean_json:
        return "JSON-Boolean";
    case json_lib::json_type::null_json:
        return "JSON-Null";
    default:
        return "Unknown JSON-Type";
    }
}

std::invalid_argument json_lib::throw_message(
    const std::shared_ptr<const json_lib::json>& obj1,
    const std::shared_ptr<const json_lib::json>& obj2,
    const std::source_location location
) {
    std::ostringstream oss;
    oss << "[Json-Error] "
        << "Attempting to evaluate a " << json_type_to_string(obj1->type())
        << " by a " << json_type_to_string(obj2->type()) << ". ";
#ifndef NDEBUG
    oss << "Values: ";
    try {
        oss << obj1->to_string();
    } catch (const std::runtime_error& e) {
        oss << "{ " << e.what() << " }";
    }
    oss << " and ";
    try {
        oss << obj2->to_string();
    } catch (const std::runtime_error& e) {
        oss << "{ " << e.what() << " }";
    }
    oss << ". ";
    oss << "In file: " << location.file_name() << '(' << location.line() << ':'
        << location.column() << ") in function `" << location.function_name()
        << "`.";
#endif
    return std::invalid_argument(oss.str());
}

json_lib::json_boolean::json_boolean(const bool value)
    : value(value) { }

json_lib::json_integer::json_integer(const int value)
    : value(value) { }

json_lib::json_real::json_real(const float value)
    : value(value) {
    const std::string result = std::to_string(value);
    size_t last = result.size() - 1;
    while (result[last] == '0') {
        --last;
    }
    if (result[last] == '.') {
        ++last;
    }
    str_value = result.substr(0, last + 1);
}

json_lib::json_real::json_real(const std::string& str_value)
    : str_value(str_value) {
    size_t pos;
    try {
        value = std::stof(str_value, &pos);
        if (pos != str_value.length()) {
            throw std::invalid_argument("Invalid characters in float string");
        }
    } catch (const std::invalid_argument&) {
        throw;
    } catch (const std::out_of_range&) {
        throw;
    }
}

json_lib::json_string::json_string(std::string value)
    : value(std::move(value)) { }

json_lib::json_array::json_array(const std::vector<std::shared_ptr<json>>& arr)
    : list(arr) {
    touch();
}

json_lib::json_object::json_object(
    const std::vector<std::pair<std::string, std::shared_ptr<json>>>& obj
)
    : data(obj) {
    for (size_t i = 0; i < obj.size(); ++i) {
        indexes[obj[i].first] = i;
    }
    touch();
}

json_lib::json_type json_lib::json::type() const {
    return json_lib::json_type::null_json;
}

json_lib::json_type json_lib::json_boolean::type() const {
    return json_lib::json_type::boolean_json;
}

json_lib::json_type json_lib::json_integer::type() const {
    return json_lib::json_type::integer_json;
}

json_lib::json_type json_lib::json_real::type() const {
    return json_lib::json_type::real_json;
}

json_lib::json_type json_lib::json_string::type() const {
    return json_lib::json_type::string_json;
}

json_lib::json_type json_lib::json_array::type() const {
    return json_lib::json_type::array_json;
}

json_lib::json_type json_lib::json_object::type() const {
    return json_lib::json_type::object_json;
}

bool json_lib::json::is_empty() const {
    if (type() == json_lib::json_type::array_json) {
        const auto arr = std::dynamic_pointer_cast<const json_lib::json_array>(
            shared_from_this()
        );
        return arr->size() == 0;
    }
    if (type() == json_lib::json_type::object_json) {
        const auto arr = std::dynamic_pointer_cast<const json_lib::json_object>(
            shared_from_this()
        );
        return arr->size() == 0;
    }
    return true;
}

bool json_lib::json::is_inline() const { return true; }

bool json_lib::json_boolean::is_inline() const { return true; }

bool json_lib::json_integer::is_inline() const { return true; }

bool json_lib::json_real::is_inline() const { return true; }

bool json_lib::json_string::is_inline() const { return true; }

bool json_lib::json_array::is_inline() const {
    return std::ranges::all_of(list, [](const auto& element) {
        return element->is_inline() && element->is_empty();
    });
}

bool json_lib::json_object::is_inline() const {
    return size() == 0
        || (size() == 1 && data.begin()->second->is_inline()
            && data.begin()->second->is_empty());
}

void json_lib::json_array::touch() {
    if (touched) {
        looped = true;
        return;
    }

    touched = true;
    std::ranges::for_each(list, [](const auto& json) { json->touch(); });
    touched = false;
}

void json_lib::json_object::touch() {
    if (touched) {
        looped = true;
        return;
    }

    touched = true;
    std::ranges::for_each(data | std::views::values, [](const auto& json) {
        json->touch();
    });
    touched = false;
}

std::string json_lib::json::to_string(const bool is_pretty) const {
    return to_string(0, is_pretty);
}

std::string json_lib::json::to_string(size_t, bool) const { return "null"; }

std::string json_lib::json_boolean::to_string(size_t, bool) const {
    return value ? "true" : "false";
}

std::string json_lib::json_integer::to_string(size_t, bool) const {
    return std::to_string(value);
}

std::string json_lib::json_real::to_string(size_t, bool) const {
    return str_value;
}

std::string json_lib::json_string::to_string(size_t, bool) const {
    std::ostringstream escaped;
    escaped << "\"";
    for (const char ch : value) {
        switch (ch) {
        case '\"':
            escaped << "\\\"";
            break;
        case '\\':
            escaped << "\\\\";
            break;
        case '\b':
            escaped << "\\b";
            break;
        case '\f':
            escaped << "\\f";
            break;
        case '\n':
            escaped << "\\n";
            break;
        case '\r':
            escaped << "\\r";
            break;
        case '\t':
            escaped << "\\t";
            break;
        default:
            // if (ch >= 0x20 && ch <= 0x7E) {
            escaped << ch;
            // } else {
            //     escaped << "\\u" << std::hex << std::setw(4)
            //             << std::setfill('0')
            //             << (static_cast<int>(static_cast<unsigned
            //             char>(ch)));
            // }
            break;
        }
    }

    escaped << "\"";
    return escaped.str();
}

std::string json_lib::json_array::to_string(
    const size_t indent_level, const bool is_pretty
) const {
    if (looped) {
        throw std::runtime_error("object is looped");
    }
    char brackets[2] = { '[', ']' };
    auto result = json_lib::format_container(
        list, format_item, brackets, indent_level, is_pretty && !is_inline()
    );
    return result;
}

std::string json_lib::json_object::to_string(
    const size_t indent_level, const bool is_pretty
) const {
    if (looped) {
        throw std::runtime_error("object is looped");
    }
    char brackets[2] = { '{', '}' };
    auto result = json_lib::format_container(
        data, format_item, brackets, indent_level, is_pretty && !is_inline()
    );
    return result;
}

std::string json_lib::json_object::format_item(
    const std::pair<std::string, std::shared_ptr<json>>& item,
    const size_t nested_level, const bool is_pretty
) {
    return "\"" + item.first
        + "\": " + item.second->to_string(nested_level, is_pretty);
}

std::string json_lib::json_array::format_item(
    const std::shared_ptr<json>& item, const size_t nested_level,
    const bool is_pretty
) {
    return item->to_string(nested_level, is_pretty);
}

int json_lib::json_integer::as_index() const { return value; }

std::string json_lib::json_string::as_key() const { return value; }

std::vector<std::string> json_lib::json_object::get_keys() {
    if (keys.size() != size()) {
        auto result = std::views::keys(indexes);
        keys = { result.begin(), result.end() };
    }
    return keys;
}

std::shared_ptr<json_lib::json> json_lib::json_array::at(const int index
) const {
    auto absolute_index = static_cast<size_t>(index);
    if (enable_negative_indexing && index < 0) {
        absolute_index += size();
    }
    if (absolute_index < size()) {
        return list.at(absolute_index);
    }
    throw std::out_of_range("index out of range");
}

std::shared_ptr<json_lib::json> json_lib::json_object::at(const std::string& key
) const {
    if (indexes.contains(key)) {
        return data.at(indexes.at(key)).second;
    }
    throw std::out_of_range("key not found");
}

size_t json_lib::json_array::size() const { return list.size(); }

size_t json_lib::json_object::size() const { return data.size(); }

std::shared_ptr<json_lib::json>
json_lib::json::by(const std::shared_ptr<json>& item) const {
    throw json_lib::throw_message(shared_from_this(), item);
}

std::shared_ptr<json_lib::json>
json_lib::json_boolean::by(const std::shared_ptr<json>& item) const {
    throw json_lib::throw_message(shared_from_this(), item);
}

std::shared_ptr<json_lib::json>
json_lib::json_integer::by(const std::shared_ptr<json>& item) const {
    if (enable_symmetric_indexing
        && item->type() == json_lib::json_type::array_json) {
        const auto array
            = std::dynamic_pointer_cast<json_lib::json_array>(item);
        return array->at(as_index());
    }
    throw json_lib::throw_message(shared_from_this(), item);
}

std::shared_ptr<json_lib::json>
json_lib::json_real::by(const std::shared_ptr<json>& item) const {
    throw json_lib::throw_message(shared_from_this(), item);
}

std::shared_ptr<json_lib::json>
json_lib::json_string::by(const std::shared_ptr<json>& item) const {
    if (!enable_symmetric_indexing
        || item->type() != json_lib::json_type::object_json) {
        throw json_lib::throw_message(shared_from_this(), item);
    }
    const auto object = std::dynamic_pointer_cast<json_lib::json_object>(item);
    return object->at(as_key());
}

std::shared_ptr<json_lib::json>
json_lib::json_array::by(const std::shared_ptr<json>& item) const {
    if (item->type() != json_lib::json_type::integer_json) {
        throw json_lib::throw_message(shared_from_this(), item);
    }
    const auto number = std::dynamic_pointer_cast<json_lib::json_integer>(item);
    return at(number->as_index());
}

std::shared_ptr<json_lib::json>
json_lib::json_object::by(const std::shared_ptr<json>& item) const {
    if (item->type() != json_lib::json_type::string_json) {
        throw json_lib::throw_message(shared_from_this(), item);
    }
    const auto string = std::dynamic_pointer_cast<json_lib::json_string>(item);
    return at(string->as_key());
}

// void json_lib::json_array::emplace_back(const std::shared_ptr<json>& element) {
//     list.emplace_back(element);
//     element->touch();
// }
//
// void json_lib::json_object::emplace_back(
//     const std::string& key, const std::shared_ptr<json>& value
// ) {
//     if (indexes.contains(key)) {
//         throw std::invalid_argument("key `" + key + "` already exists");
//     }
//     indexes[key] = data.size();
//     if (!keys.empty() || data.empty()) {
//         keys.push_back(key);
//     }
//     data.emplace_back(key, value);
//     value->touch();
// }