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
 * @brief Enable or disable symmetric indexing for JSON objects.
 *
 * When `enable_symmetric_indexing` is set to `true`, symmetric indexing allows
 * for flexible access, similar to C-style dynamic arrays where `arr[index] ==
 * index[arr]`. For JSON objects, this feature enables interchangeable access to
 * keys and values in specific cases, allowing for concise access patterns.
 *
 * ### Example with JSON
 * Consider a JSON structure like:
 * ```json
 * {
 *     "obj": {
 *         "secret_key": 5,
 *         "public_arr": ["key", "key", "secret_key", "key"]
 *     }
 * }
 * ```
 * With symmetric indexing enabled, instead of writing `obj[obj.public_arr[2]]`,
 * you could write `obj.public_arr[2][obj]`, which accesses `"secret_key"` in a
 * more concise form.
 *
 * ### How it looks in source code
 * In C++ code, enabling symmetric indexing can simplify complex JSON access
 * patterns:
 * ```cpp
 * // Define JSON object with shared pointers
 * std::vector<std::pair<std::string, std::shared_ptr<json_lib::json>>> values =
 * {
 *     { "secret_key", std::make_shared<json_lib::json_integer>(5) },
 *     { "public_arr", std::make_shared<json_lib::json_array>(
 *         std::vector<std::shared_ptr<json_lib::json>>{
 *             std::make_shared<json_lib::json_string>("key"),
 *             std::make_shared<json_lib::json_string>("key"),
 *             std::make_shared<json_lib::json_string>("secret_key"),
 *             std::make_shared<json_lib::json_string>("key")
 *         })
 *     }
 * };
 * std::shared_ptr<json_lib::json_object> obj =
 *     std::make_shared<json_lib::json_object>(values);
 * assert(obj->at("public_arr")->at(2)->by(obj) ==
 *     obj->by(obj->at("public_arr")->at(2)) &&
 *     "Symmetric indexing is enabled");
 * ```
 * This demonstrates how symmetric indexing can simplify access to values in
 * nested structures, as shown in the JSON example above.
 *
 * **Default:** `false`
 */
inline bool enable_symmetric_indexing = false;

/**
 * @brief Enable or disable support for negative array indexing.
 *
 * When `enable_negative_indexing` is set to `true`, negative indices are
 * supported in arrays, similar to Python-style indexing. This allows for
 * reverse-order element access, where `array[-index]` retrieves elements
 * from the end, computed as `array[size - index]`.
 *
 * ### Example with JSON
 * In a JSON array structure like:
 * ```json
 * {
 *     "arr": [1, 1.0, true, "test"]
 * }
 * ```
 * Accessing `arr[-1]` would be equivalent to `arr[3]` (last element, because
 * `arr.size() == 4`), returning `"test"`.
 *
 * ### How it looks in source code
 * In C++ code, using `shared_ptr` for JSON elements:
 * ```cpp
 * std::vector<std::shared_ptr<json_lib::json>> values = {
 *     std::make_shared<json_lib::json_integer>(1),
 *     std::make_shared<json_lib::json_real>(1.0f),
 *     std::make_shared<json_lib::json_boolean>(true),
 *     std::make_shared<json_lib::json_string>("test")
 * };
 * std::shared_ptr<json_lib::json_array> arr =
 *     std::make_shared<json_lib::json_array>(values);
 * assert(arr->size() == 4 &&
 *     "Array contains 4 elements");
 * assert(arr->at(-1) == arr->at(3) &&
 *     arr->at(-1)->to_string() == "\"test\"" &&
 *     "Negative indexing is enabled");
 * ```
 * This demonstrates how negative indexing retrieves the last element,
 * similar to the JSON example above.
 *
 * **Default:** `false`
 */
inline bool enable_negative_indexing = false;

/**
 * @brief Enumeration representing the different types of JSON values.
 *
 * The `json_type` enumeration defines the various types of JSON values that
 * can be represented in the library. Each type corresponds to a fundamental
 * data structure in JSON, allowing for type checking and handling of
 * different JSON elements.
 */
enum class json_type : int {
    null_json, ///< Represents a JSON null value.
    boolean_json, ///< Represents a JSON boolean value (true or false).
    integer_json, ///< Represents a JSON integer value.
    real_json, ///< Represents a JSON real (floating-point) value.
    string_json, ///< Represents a JSON string value.
    array_json, ///< Represents a JSON array.
    object_json, ///< Represents a JSON object.
    reference_json ///< Represents an abstract JSON type, referring to another
                   ///< JSON value.
};

/**
 * @brief Convert a `json_type` to its corresponding string representation.
 *
 * The `json_type_to_string` function takes a `json_type` value and converts
 * it to a human-readable string. This can be useful for debugging or logging
 * purposes, allowing users to easily identify the type of a JSON value.
 *
 * @param type The `json_type` to convert to a string.
 * @return A string representation of the given `json_type`.
 */
std::string json_type_to_string(json_type type);

/**
 * @brief Base class representing a JSON value.
 *
 * The `json` class serves as the abstract base class for all JSON types in the
 * library. It provides a common interface for accessing and manipulating JSON
 * values, supporting features like type identification, string conversion, and
 * recursive processing while handling potential circular references.
 */
class json : public std::enable_shared_from_this<json> {
public:
    virtual ~json() = default;

    /**
     * @brief Get the JSON type of the object.
     *
     * This method returns the type of the JSON value represented by this
     * object. It allows users to determine the specific kind of JSON element
     * (e.g., null, boolean, integer) at runtime.
     *
     * @return The `json_type` of the object.
     */
    virtual constexpr json_type type() const { return _type; }

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
     * @brief Check if the JSON element is empty.
     *
     * This method determines whether the JSON element contains any nested
     * values. For collection types such as `json_array` and `json_object`, if
     * their size is zero, it means the collection is empty and contains no
     * nested values.
     *
     * @return `true` if the JSON element is empty; `false` otherwise.
     */
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

    /**
     * @brief Convert the JSON element to a string.
     *
     * This method generates a string representation of the JSON element by
     * calling `formatted_string` with the `pretty` flag set to `false`. It
     * provides a compact representation of the JSON value without additional
     * formatting.
     *
     * @return A string representation of the JSON element in a compact format.
     */
    virtual std::string to_string() const;

    /**
     * @brief Convert the JSON element to a formatted string.
     *
     * This method generates a string representation of the JSON element with
     * formatting applied based on the `pretty` flag. If `pretty` is true,
     * the output will be structured with indentation and line breaks for better
     * readability; otherwise, it will return a compact representation.
     *
     * @param pretty Flag indicating whether to format the output for
     * readability.
     * @return A formatted string representation of the JSON element.
     */
    virtual std::string formatted_string(bool pretty) const;

    /**
     * @brief Convert the JSON element to an indented string representation.
     *
     * This method generates a string representation of the JSON element with
     * a specified indentation level and formatting applied based on the
     * `pretty` flag. This is particularly useful for generating well-structured
     * output.
     *
     * @param indent_level The number of spaces to use for indentation.
     * @param pretty Flag indicating whether to format the output for
     * readability.
     * @return An indented string representation of the JSON element.
     */
    virtual std::string indented_string(size_t indent_level, bool pretty) const;

    /**
     * @brief Retrieve a nested JSON element by another JSON value.
     *
     * This method serves as an alternative to `at()`, allowing access to nested
     * values using another JSON object as a key or index. While `at()` works
     * only with constant types (like int or string for sets), `by()` can use
     * JSON values for more flexible indexing. It also supports symmetric and
     * negative indexing if those features are enabled.
     *
     * @param item A shared pointer to a JSON object that serves as the key or
     * index to access a nested value.
     * @return A shared pointer to the nested JSON element corresponding to the
     * provided key or index.
     */
    [[nodiscard]] virtual std::shared_ptr<json>
    by(const std::shared_ptr<json>& item) const;

protected:
    json_type _type = json_type::null_json; ///< The type of the JSON object.
};

class json_boolean final : public json {
public:
    explicit json_boolean(bool value);
    std::string
    indented_string(size_t indent_level, bool pretty) const override;

private:
    bool value;
};

class json_integer final : public json {
public:
    explicit json_integer(int value);
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
    std::string
    indented_string(size_t indent_level, bool pretty) const override;

private:
    float value;
    std::string str_value;
};

class json_string final : public json {
public:
    explicit json_string(std::string value);
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
