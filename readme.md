# JSON Expression Parser

**JSON Expression Parser** is a C++20 console application that allows users to evaluate expressions on JSON files
directly from the command line. This tool provides easy access to JSON elements, supports intrinsic functions for
efficient data extraction, and offers basic arithmetic operations.

## Setup and Installation

### Requirements

* **C++20** compiler
* **GTest**: for unit tests
* **lcov**: for code coverage reports
* **doxygen** and **graphviz**: for generating documentation

### Building the Application

1. Build with CMake in Release mode:
    ```bash
    $ cmake -DBUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release <path-to-project-root>
    $ cmake --build .
    ```
2. Run the Application:
    ```bash
    $ json_eval [options] <json-file> "<expression>"
    ```
    * `<json-file>`: Path to the JSON file to evaluate.
    * `<expression>`: Expression to query or compute JSON elements.

## JSONPath Syntax

This application supports JSONPath-like syntax for querying JSON files. The table below outlines available JSONPath
operators and their meanings, including additional syntax for multiple paths and index lists.

| XPath  | JSONPath                     | Description                                                                                                               |
|--------|------------------------------|---------------------------------------------------------------------------------------------------------------------------|
| `/`    | `$`                          | Refers to the root object/element.                                                                                        |
| `.`    | `@`                          | Refers to the current object/element.                                                                                     |
| `/`    | `.` or `[]`                  | Child operator.                                                                                                           |
| `[]`   | `[]`                         | Subscript operator for accessing array elements or object members.                                                        |
| `()`   | `()`                         | Expression syntax for evaluating subexpressions.                                                                          |
| `\|`   | `[,]` or `{child-operators}` | Union operator in XPath results in a combination of node sets. JSONPath allows alternate names or array indices as a set. |

This table is adapted from the JSONPath [article](https://goessner.net/articles/JsonPath/).

### Features in Progress

* **Ranges**: `[start:end:step]` — Array slicing syntax inspired by ECMAScript 4 (not yet implemented).
* **Filter Expressions**: `?()` — Filters with boolean expressions (not yet implemented).

## Example Usage

Given a JSON file, `test.json`, with the following content:

```json
{
   "a": { "b": [ 1, 2, { "c": "test" }, [11, 12] ]}
}
```

Retrieve values using expressions that leverage the syntax and features defined above.

### JSON Element Access

Directly access JSON elements with path expressions:

```bash
$ ./json_eval test.json "a.b[1]"
# 2
$ ./json_eval test.json "a.b[2].c"
# "test"
$ ./json_eval test.json "a.b"
# [1, 2, { "c": "test" }, [11, 12]]
```

### Intrinsic Functions: `min`, `max`, and `size`

The parser supports intrinsic functions to aid data extraction:

* `min`: Returns the minimum value in an array or list of arguments.
* `max`: Returns the maximum value in an array or list of arguments.
* `size`: Returns the size of an object, array, or string.

Example:

```bash
$ ./json_eval test.json "max(a.b[0], a.b[1])"
# 2
$ ./json_eval test.json "min(a.b[3])"
# 11
$ ./json_eval test.json "size(a.b)"
# 4
```

### Subscript Expressions and Nested Queries

Use subscripts to perform nested queries or access dynamically evaluated indices:

```bash
$ ./json_eval test.json "a.b[a.b[1]].c"
# "test"
```

### Basic Arithmetic Operations (not yet implemented)

Basic arithmetic operators (`+`, `-`, `*`, `/`) can be applied to numerical JSON elements (partially implemented):

```bash
$ ./json_eval test.json "a.b[0] + a.b[1]"
# 3
```

## Documentation and Contributing

To build and run tests, enable debug mode, or generate coverage reports:

1. **Build with Debug and Coverage:**
   ```bash
   $ cmake -DCMAKE_BUILD_TYPE=Debug -DCOVERAGE=ON <path-to-project-root>
   $ cmake --build .
   $ ctest
   ```
2. **Generate Coverage Report:**
   ```bash
   $ lcov --capture --directory . --output-file coverage.info --rc branch_coverage=1 --ignore-errors gcov,inconsistent,mismatch
   ```
3. Generate HTML Coverage Report:
   ```bash
   $ lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info --rc branch_coverage=1
   $ genhtml coverage.info --demangle-cpp --branch-coverage --output-directory ../cov
   ```

For detailed documentation, see the [Documentation](https://yariabtsev.github.io/json-eval/doc/)  and for the latest
coverage report, see [Coverage](https://yariabtsev.github.io/json-eval/cov/).

## License

This project is open-source and available under the MIT License.
