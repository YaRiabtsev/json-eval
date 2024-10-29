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

#include <algorithm>

parser_lib::raw_json::raw_json(const std::shared_ptr<json>& head)
    : head(head) { }

parser_lib::raw_json::raw_json(const std::string& name)
    : head(name) { }

json_lib::json_type parser_lib::raw_json::type() const {
    return json_lib::json_type::custom_json;
}

void parser_lib::raw_json::touch() {
    if (touched) {
        looped = true;
        return;
    }
    touched = true;
    std::ranges::for_each(tail, [](const auto& json) { json->touch(); });
    touched = false;
}

size_t parser_lib::raw_json::size() const { return tail.size(); }

void parser_lib::raw_json::emplace_back(const std::shared_ptr<json>& item) {
    tail.emplace_back(item);
}

std::string parser_lib::raw_json::indented_string(size_t, bool) const {
    std::string result = std::visit(
        []<typename T>(T arg) -> std::string {
            if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            }
            if constexpr (std::is_same_v<T, std::shared_ptr<json>>) {
                return arg->to_string();
            }
            return "#";
        },
        head
    );
    for (const auto& item : tail) {
        std::string item_str = item->to_string();
        if (item->type() == json_lib::json_type::array_json) {
            result += item_str;
        } else {
            result += "[" + item_str + "]";
        }
    }
    return result;
}