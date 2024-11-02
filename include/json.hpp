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
 * `obj.public_arr[2][obj]`, which can be more
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

enum class json_type : int {
    null_json,
    boolean_json,
    integer_json,
    real_json,
    string_json,
    array_json,
    object_json,
    custom_json
};

std::string json_type_to_string(json_type type);

class json : public std::enable_shared_from_this<json> {
public:
    virtual ~json() = default;
    [[nodiscard]] virtual json_type type() const;
    /**
     * @brief Prevents infinite loops by marking objects during recursive JSON
     * processing.
     *
     * This virtual method is used to safeguard against infinite loops, such as
     * in circular references, by "touching" the JSON object. During recursive
     * operations, this function enables detection of repeated references to
     * halt processing if a loop is detected.
     */
    virtual void touch();
    virtual bool empty() const;
    /**
     * @brief Specifies if the JSON element is consistently represented in a
     * compact, single-line format.
     *
     * This method returns `true` if the JSON element is always displayed in a
     * compact, single-line format, regardless of formatting options like
     * `pretty` or `indent_level`. Compact elements include simple types,
     * such as integers or strings, and empty structures, like empty arrays or
     * objects, which do not require additional formatting.
     *
     * When `compact()` returns `false`, the JSON element may support multi-line
     * formatting with indentation and line breaks, particularly for complex or
     * nested structures.
     *
     * @return `true` if the JSON element is consistently represented in a
     * single-line, compact format; `false` if multi-line or structured
     * formatting may apply.
     */
    virtual bool compact() const;
    virtual std::string to_string() const;
    virtual std::string formatted_string(bool pretty) const;
    virtual std::string indented_string(size_t indent_level, bool pretty) const;
    [[nodiscard]] virtual std::shared_ptr<json>
    by(const std::shared_ptr<json>& item) const;
};

class json_boolean final : public json {
public:
    explicit json_boolean(bool value);
    [[nodiscard]] json_type type() const override;
    std::string
    indented_string(size_t indent_level, bool pretty) const override;

private:
    bool value;
};

class json_integer final : public json {
public:
    explicit json_integer(int value);
    [[nodiscard]] json_type type() const override;
    std::string
    indented_string(size_t indent_level, bool pretty) const override;
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
    std::string
    indented_string(size_t indent_level, bool pretty) const override;

private:
    float value;
    std::string str_value;
};

class json_string final : public json {
public:
    explicit json_string(std::string value);
    [[nodiscard]] json_type type() const override;
    std::string
    indented_string(size_t indent_level, bool pretty) const override;
    [[nodiscard]] std::string as_key() const;
    [[nodiscard]] std::shared_ptr<json> by(const std::shared_ptr<json>& item
    ) const override;

private:
    std::string value;
};

class json_array final : public json {
public:
    explicit json_array(const std::vector<std::shared_ptr<json>>& arr = {});
    [[nodiscard]] json_type type() const override;
    void touch() override;
    bool empty() const override;
    bool compact() const override;
    std::string
    indented_string(size_t indent_level, bool pretty) const override;
    [[nodiscard]] size_t size() const;
    [[nodiscard]] std::shared_ptr<json> at(int index) const;
    [[nodiscard]] std::shared_ptr<json> by(const std::shared_ptr<json>& item
    ) const override;

private:
    bool looped { false };
    bool touched { false };
    std::vector<std::shared_ptr<json>> list;

    static std::string format_item(
        const std::shared_ptr<json>& item, size_t nested_level, bool pretty
    );
};

class json_object final : public json {
public:
    explicit json_object(
        const std::vector<std::pair<std::string, std::shared_ptr<json>>>& obj
        = {}
    );
    [[nodiscard]] json_type type() const override;
    void touch() override;
    bool empty() const override;
    bool compact() const override;
    std::string
    indented_string(size_t indent_level, bool pretty) const override;
    [[nodiscard]] size_t size() const;
    [[nodiscard]] std::vector<std::string> get_keys();
    [[nodiscard]] std::shared_ptr<json> at(const std::string& key) const;
    [[nodiscard]] std::shared_ptr<json> by(const std::shared_ptr<json>& item
    ) const override;

private:
    bool looped { false };
    bool touched { false };
    std::vector<std::pair<std::string, std::shared_ptr<json>>> data;
    std::unordered_map<std::string, size_t> indexes;
    std::vector<std::string> keys {};

    static std::string format_item(
        const std::pair<std::string, std::shared_ptr<json>>& item,
        size_t nested_level, bool pretty
    );
};

template <typename Collection>
concept Iterable = requires(Collection c) {
    { c.begin() } -> std::same_as<typename Collection::iterator>;
    { c.end() } -> std::same_as<typename Collection::iterator>;
};

template <typename Formatter, typename Element>
concept CallableFormatter
    = requires(Formatter f, const Element& e, size_t level, bool pretty) {
          { f(e, level, pretty) } -> std::convertible_to<std::string>;
      };

template <Iterable Collection, typename Formatter>
    requires CallableFormatter<Formatter, typename Collection::value_type>
std::string format_container(
    const Collection& elements, Formatter lambda, const size_t indent_level,
    const bool pretty
) {
    std::string result;
    std::string indent;
    size_t nested_level = indent_level;

    if (pretty) {
        nested_level++;
        indent = std::string(indent_level, '\t');
        result += "\n";
    }

    for (auto it = elements.begin(); it != elements.end(); ++it) {
        if (it != elements.begin()) {
            result += ',';
            result += pretty ? "\n" : " ";
        }
        if (pretty) {
            result += indent + '\t';
        }
        result += lambda(*it, nested_level, pretty);
    }

    if (pretty) {
        result += "\n" + indent;
    }
    return result;
}

std::invalid_argument throw_message(
    const std::shared_ptr<const json>& obj1,
    const std::shared_ptr<const json>& obj2,
    std::source_location location = std::source_location::current()
);
}
#endif // JSON_HPP
