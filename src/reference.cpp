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

#include "reference.hpp"

#include <ranges>

reference_lib::json_reference::json_reference(const ref_head_type type)
    : head_type(type) { }

reference_lib::json_reference::json_reference(const std::shared_ptr<json>& head)
    : head_type(ref_head_type::object)
    , head(head) { }

reference_lib::json_set::json_set(
    const std::vector<std::shared_ptr<json_reference>>& elements
)
    : elements(elements) {
    set_head_type(ref_head_type::accessor);
}

reference_lib::json_function::json_function(std::string name)
    : name(std::move(name)) { }

json_lib::json_type reference_lib::json_reference::type() const {
    return json_lib::json_type::custom_json;
}

std::string reference_lib::json_reference::indented_string(
    const size_t indent_level, const bool pretty
) const {
    if (looped) {
        throw std::runtime_error("object is looped");
    }
    std::string result;
    switch (head_type) {
    case ref_head_type::object:
        result += head->indented_string(indent_level, pretty);
        break;
    case ref_head_type::local:
        result += "@";
        break;
    case ref_head_type::root:
        result += "$";
        break;
    default:;
        // result += "+";
    }
    result += tail_to_string();
    return result;
}

std::string reference_lib::json_set::indented_string(size_t, bool) const {
    std::string result;
    for (const auto& element : elements) {
        if (!result.empty()) {
            result += ", ";
        }
        result += element->to_string();
    }
    if (get_head_type() == ref_head_type::set) {
        result = '[' + result + ']';
    } else {
        result = '{' + result + '}';
    }
    result += tail_to_string();
    return result;
}

std::string reference_lib::json_function::indented_string(size_t, bool) const {
    std::string result;
    for (const auto& arg : args) {
        if (!result.empty()) {
            result += ", ";
        }
        result += arg->to_string();
    }
    result = name + '(' + result + ')';
    result += tail_to_string();
    return result;
}

std::string reference_lib::json_reference::tail_to_string() const {
    std::string result;
    for (const auto& accessor : tail) {
        std::string suffix = accessor->to_string();
        if (accessor->type() == json_lib::json_type::custom_json) {
            if (std::dynamic_pointer_cast<json_reference>(accessor)
                    ->reference_type()
                == json_reference_type::set_json) {
                result += suffix;
                continue;
            }
        }
        result += "[" + suffix + ']';
    }
    return result;
}

void reference_lib::json_reference::touch() {
    if (touched) {
        looped = true;
        return;
    }
    touched = true;
    if (head_type == ref_head_type::object) {
        head->touch();
    }
    touched = false;
}

void reference_lib::json_set::touch() {
    if (get_head_type() == ref_head_type::set) {
        for (const auto& element : elements) {
            element->touch();
        }
    }
}

reference_lib::json_reference_type
reference_lib::json_reference::reference_type() const {
    return json_reference_type::reference_json;
}

reference_lib::json_reference_type
reference_lib::json_set::reference_type() const {
    return json_reference_type::set_json;
}

reference_lib::json_reference_type
reference_lib::json_function::reference_type() const {
    return json_reference_type::function_json;
}

void reference_lib::json_reference::emplace_back(
    const std::shared_ptr<json>& accessor
) {
    const bool abstract = head_type != ref_head_type::object || length() > 0;
    tail.emplace_back(accessor);
    if (!abstract) {
        simplify();
    }
}

void reference_lib::json_set::emplace_back(const std::shared_ptr<json>& item) {
    for (const auto& element : elements) {
        element->emplace_back(item);
    }
}

void reference_lib::json_function::set_args(
    const std::vector<std::shared_ptr<json>>& args
) {
    this->args = args;
}

void reference_lib::json_reference::set_local_head(
    const std::shared_ptr<json>& local
) {
    if (head_type == ref_head_type::local
        || head_type == ref_head_type::accessor) {
        // todo: update_head(local);
        head = local;
        head_type = ref_head_type::object;
        simplify();
        touch();
    }
}

void reference_lib::json_set::set_local_head(const std::shared_ptr<json>& local
) {
    for (const auto& element : elements) {
        element->set_local_head(local);
    }
    set_head_type(ref_head_type::set);
}

void reference_lib::json_function::set_local_head(const std::shared_ptr<
                                                  json>&) { }

size_t reference_lib::json_reference::length() const { return tail.size(); }

std::shared_ptr<json_lib::json> reference_lib::json_reference::value() {
    if (length() > 0 || head_type != ref_head_type::object) {
        return shared_from_this();
    }
    if (head->type() != json_lib::json_type::custom_json) {
        return head;
    }
    return std::dynamic_pointer_cast<json_reference>(head)->value();
}

reference_lib::ref_head_type
reference_lib::json_reference::get_head_type() const {
    return head_type;
}

void reference_lib::json_reference::simplify() {
    while (head_type == ref_head_type::object && length() > 0) {
        auto accessor = tail.front();
        if (head->type() == json_lib::json_type::custom_json) {
            tail.pop_front();
            const auto ref_head
                = std::dynamic_pointer_cast<json_reference>(head);
            ref_head->emplace_back(accessor);
            head = ref_head->value();
        } else {
            if (accessor->type() == json_lib::json_type::custom_json) {
                const auto ref_accessor
                    = std::dynamic_pointer_cast<json_reference>(accessor);
                switch (ref_accessor->reference_type()) {
                case json_reference_type::reference_json: {
                    if (ref_accessor->get_head_type() == ref_head_type::root) {
                        return;
                    }
                    ref_accessor->set_local_head(head);
                    tail.front() = ref_accessor->value();
                    break;
                }
                case json_reference_type::set_json: {
                    std::dynamic_pointer_cast<json_set>(ref_accessor)
                        ->set_local_head(head);
                    head = ref_accessor->value();
                    tail.pop_front();
                    break;
                }
                case json_reference_type::function_json: {
                    // todo
                    return;
                }
                default: {
                    throw std::runtime_error("unsupported accessor type");
                }
                }
            } else {
                head = head->by(accessor);
                tail.pop_front();
            }
        }
    }
}

void reference_lib::json_reference::set_head_type(const ref_head_type type) {
    head_type = type;
}
