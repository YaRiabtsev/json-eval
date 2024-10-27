/*
 * The MIT License (Mitem)
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

#ifndef JSON_HPP
#define JSON_HPP
#include <memory>
#include <source_location>
#include <string>
#include <unordered_map>
#include <vector>

namespace json_lib {
/**
 * @brief Enable or disable symmetric indexing (e.g., array index referencing).
 *
 * When `enable_symmetric_indexing` is true, indexing can be used symmetrically,
 * similar to C-style dynamic arrays where `arr[index] == index[arr]`. This
 * functionality is extended here to JSON objects, allowing for keys and values
 * to be accessed interchangeably in certain cases.
 *
 * ### Usage
 * Symmetric indexing can reduce nested brackets when accessing deeply
 * structured JSON objects. For example, consider:
 * ```json
 * {
 *     "obj": {
 *         "secret_key": 5,
 *         "public_arr": ["key", "key", "secret_key", "key"]
 *     }
 * }
 * ```
 * Instead of writing `obj[obj.public_arr[2]]`, you can use
 * `obj.public_arr[2][obj]` or even `obj.public_arr[2].obj`, which can be more
 * concise, albeit less intuitive.
 *
 * **Default:** `false`
 */
inline bool enable_symmetric_indexing = false;

/**
 * @brief Enable or disable negative indexing for arrays.
 *
 * When `enable_negative_indexing` is set to true, negative indices are allowed,
 * similar to Python-style indexing where `array[-index]` retrieves elements in
 * reverse order, calculated as `array[size - index]`.
 *
 * **Example:**
 * ```cpp
 * json_array arr = json_array({ "first", "second", "third" });
 * arr[-1];  // Returns "third" (last element)
 * ```
 *
 * **Default:** `false`
 */
inline bool enable_negative_indexing = false;

enum class json_type {
    null_json,
    boolean_json,
    integer_json,
    real_json,
    string_json,
    array_json,
    object_json,
    // raw_json
};

std::string json_type_to_string(json_type type);

class json : public std::enable_shared_from_this<json> {
public:
    virtual ~json() = default;
    [[nodiscard]] virtual json_type type() const;
    /**
     * @brief Checks if the JSON element is consistently represented in a single
     * line.
     *
     * This method returns `true` if the JSON element is intended to be
     * displayed inline without additional formatting, such as indentation or
     * line breaks. Examples of inline elements include integers, strings, and
     * empty arrays.
     *
     * @return `true` if the element is constantly represented in one line,
     * otherwise `false`.
     */
    virtual bool is_inline() const;

    /**
     * @brief Prevents infinite loops by marking objects during recursive JSON
     * processing.
     *
     * This virtual method is used to safeguard against infinite loops, such as
     * in circular references, by "touching" the JSON object. During recursive
     * operations, this function enables detection of repeated references to
     * halt processing if a loop is detected.
     */
    virtual void touch() { }

    /**
     * @brief Checks if the JSON object is empty, with no nested elements or
     * child items.
     *
     * For `json_array` or `json_object` types, this method checks whether they
     * contain any nested elements by evaluating `size() == 0`. For other JSON
     * types, such as `json_integer`, which contain only a single, non-nested
     * value, this method returns `true`.
     *
     * @return `true` if the JSON object is empty or non-nested, otherwise
     * `false`.
     */
    bool is_empty() const;
    std::string to_string(bool is_pretty = false) const;
    virtual std::string to_string(size_t indent_level, bool is_pretty) const;
    [[nodiscard]] virtual std::shared_ptr<json>
    by(const std::shared_ptr<json>& item) const;
};

class json_boolean final : public json {
public:
    explicit json_boolean(bool value);
    [[nodiscard]] json_type type() const override;
    bool is_inline() const override;

    void touch() override { }

    std::string to_string(size_t indent_level, bool is_pretty) const override;
    [[nodiscard]] std::shared_ptr<json> by(const std::shared_ptr<json>& item
    ) const override;

private:
    bool value;
};

class json_integer final : public json {
public:
    explicit json_integer(int value);
    [[nodiscard]] json_type type() const override;
    bool is_inline() const override;

    void touch() override { }

    std::string to_string(size_t indent_level, bool is_pretty) const override;
    [[nodiscard]] int as_index() const;
    [[nodiscard]] std::shared_ptr<json> by(const std::shared_ptr<json>& item
    ) const override;

private:
    int value;
};

class json_real final : public json {
public:
    explicit json_real(float value);
    explicit json_real(const std::string& str_value);
    [[nodiscard]] json_type type() const override;
    bool is_inline() const override;

    void touch() override { }

    std::string to_string(size_t indent_level, bool is_pretty) const override;
    [[nodiscard]] std::shared_ptr<json> by(const std::shared_ptr<json>& item
    ) const override;

private:
    float value;
    std::string str_value;
};

class json_string final : public json {
public:
    explicit json_string(std::string value);
    [[nodiscard]] json_type type() const override;
    bool is_inline() const override;

    void touch() override { }

    std::string to_string(size_t indent_level, bool is_pretty) const override;
    [[nodiscard]] std::string as_key() const;
    [[nodiscard]] std::shared_ptr<json> by(const std::shared_ptr<json>& item
    ) const override;

private:
    std::string value;
};

class json_array : public json {
public:
    explicit json_array(const std::vector<std::shared_ptr<json>>& arr = {});
    [[nodiscard]] json_type type() const override;
    bool is_inline() const override;
    void touch() override;
    std::string to_string(size_t indent_level, bool is_pretty) const override;
    [[nodiscard]] size_t size() const;
    [[nodiscard]] std::shared_ptr<json> by(const std::shared_ptr<json>& item
    ) const override;
    [[nodiscard]] std::shared_ptr<json> at(int index) const;
    // void emplace_back(const std::shared_ptr<json>& element);

private:
    bool looped { false };
    bool touched { false };
    std::vector<std::shared_ptr<json>> list;

    static std::string format_item(
        const std::shared_ptr<json>& item, size_t nested_level, bool is_pretty
    );
};

class json_object final : public json {
public:
    explicit json_object(
        const std::vector<std::pair<std::string, std::shared_ptr<json>>>& obj
        = {}
    );
    [[nodiscard]] json_type type() const override;
    bool is_inline() const override;
    void touch() override;
    std::string to_string(size_t indent_level, bool is_pretty) const override;
    [[nodiscard]] size_t size() const;
    [[nodiscard]] std::vector<std::string> get_keys();
    [[nodiscard]] std::shared_ptr<json> by(const std::shared_ptr<json>& item
    ) const override;
    [[nodiscard]] std::shared_ptr<json> at(const std::string& key) const;
    // void
    // emplace_back(const std::string& key, const std::shared_ptr<json>& value);

private:
    bool looped { false };
    bool touched { false };
    std::vector<std::pair<std::string, std::shared_ptr<json>>> data;
    std::unordered_map<std::string, size_t> indexes;
    std::vector<std::string> keys {};

    static std::string format_item(
        const std::pair<std::string, std::shared_ptr<json>>& item,
        size_t nested_level, bool is_pretty
    );
};

// class raw_json final : public json_array {
// public:
//     [[nodiscard]] json_type type() const override;
//     bool is_inline() const override;
//     void touch() override;
//     std::string to_string(size_t indent_level, bool is_pretty) const
//     override;
//     [[nodiscard]] std::shared_ptr<json> by(const std::shared_ptr<json>& item
//     ) const override;
//
// private:
//     bool looped { false };
//     bool touched { false };
//     std::vector<std::shared_ptr<json>> list;
// };

template <typename Collection>
concept Iterable = requires(Collection c) {
    { c.begin() } -> std::same_as<typename Collection::iterator>;
    { c.end() } -> std::same_as<typename Collection::iterator>;
};

template <typename Formatter, typename Element>
concept CallableFormatter
    = requires(Formatter f, const Element& e, size_t level, bool is_pretty) {
          { f(e, level, is_pretty) } -> std::convertible_to<std::string>;
      };

template <Iterable Collection, typename Formatter>
    requires CallableFormatter<Formatter, typename Collection::value_type>
std::string format_container(
    const Collection& elements, Formatter lambda, char brackets[2],
    const size_t indent_level, const bool is_pretty
) {
    std::string result;
    result += brackets[0];
    std::string indent;
    size_t nested_level = indent_level;

    if (is_pretty) {
        nested_level++;
        indent = std::string(indent_level, '\t');
        result += "\n";
    }

    for (auto it = elements.begin(); it != elements.end(); ++it) {
        if (it != elements.begin()) {
            result += ",";
            result += is_pretty ? "\n" : " ";
        }
        if (is_pretty) {
            result += indent + '\t';
        }
        result += lambda(*it, nested_level, is_pretty);
    }

    if (is_pretty) {
        result += "\n" + indent;
    }
    result += brackets[1];
    return result;
}

std::invalid_argument throw_message(
    const std::shared_ptr<const json_lib::json>& obj1,
    const std::shared_ptr<const json_lib::json>& obj2,
    std::source_location location = std::source_location::current()
);
}
#endif // JSON_HPP
