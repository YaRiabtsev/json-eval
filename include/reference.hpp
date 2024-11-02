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

#ifndef CUSTOM_JSON_HPP
#define CUSTOM_JSON_HPP
#include "json.hpp"

#include <deque>

namespace reference_lib {
enum class json_reference_type : int {
    reference_json,
    set_json,
    function_json
};
enum class ref_head_type : int { local, root, accessor, object, set };

class json_reference : public json_lib::json {
public:
    explicit json_reference(ref_head_type type = ref_head_type::root);
    explicit json_reference(const std::shared_ptr<json>& head);

    std::string
    indented_string(size_t indent_level, bool pretty) const override;
    std::string tail_to_string() const;
    void touch() override;
    json_lib::json_type type() const override;
    virtual json_reference_type reference_type() const;

    virtual void emplace_back(const std::shared_ptr<json>& accessor);
    virtual void set_local_head(const std::shared_ptr<json>& local);

    virtual size_t length() const;
    virtual std::shared_ptr<json> value();
    ref_head_type get_head_type() const;

private:
    bool looped { false };
    bool touched { false };
    ref_head_type head_type;
    std::shared_ptr<json> head = nullptr;
    std::deque<std::shared_ptr<json>> tail {};
    void simplify();

protected:
    void set_head_type(ref_head_type type);
};

class json_set final : public json_reference {
public:
    explicit json_set(
        const std::vector<std::shared_ptr<json_reference>>& elements
    );

    std::string
    indented_string(size_t indent_level, bool pretty) const override;
    json_reference_type reference_type() const override;
    void touch() override;
    void emplace_back(const std::shared_ptr<json>& item) override;
    void set_local_head(const std::shared_ptr<json>& local) override;

private:
    bool independent = false;
    std::vector<std::shared_ptr<json_reference>> elements;
};

class json_function final : public json_reference {
public:
    explicit json_function(std::string name);

    std::string
    indented_string(size_t indent_level, bool pretty) const override;
    json_reference_type reference_type() const override;

    void set_local_head(const std::shared_ptr<json>& local) override;
    void set_args(const std::vector<std::shared_ptr<json>>& args);

private:
    std::string name;
    std::vector<std::shared_ptr<json>> args {};
};
}

#endif // CUSTOM_JSON_HPP
