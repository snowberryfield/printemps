/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_JSON_JSON_H__
#define PRINTEMPS_UTILITY_JSON_JSON_H__

namespace printemps::utility::json {
/*****************************************************************************/
using JsonObjectBody = std::vector<std::pair<std::string, std::any>>;
using JsonArrayBody  = std::vector<std::any>;

/*****************************************************************************/
enum class ReadMode { Key, Value };

/*****************************************************************************/
struct JsonObject {
    JsonObjectBody                               body;
    std::unordered_map<std::string, std::size_t> map;
    int                                          indent_level;

    /*************************************************************************/
    JsonObject(void) {
        this->initialize();
    }

    /*************************************************************************/
    template <class T>
    JsonObject(const std::string& a_KEY, const T& a_VALUE) {
        this->initialize();
        this->emplace_back(a_KEY, a_VALUE);
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->body.clear();
        this->map.clear();
        this->indent_level = 0;
    }

    /*************************************************************************/
    template <class T>
    inline void emplace_back(const std::string& a_KEY, const T& a_VALUE) {
        this->map[a_KEY] = this->body.size();
        this->body.emplace_back(a_KEY, a_VALUE);
    }

    /*************************************************************************/
    inline std::any& operator[](const std::string& a_KEY) {
        const std::size_t INDEX = this->map.at(a_KEY);
        return this->body[INDEX].second;
    }

    /*************************************************************************/
    inline const std::any& at(const std::string& a_KEY) const {
        const std::size_t INDEX = this->map.at(a_KEY);
        return this->body[INDEX].second;
    }

    /*************************************************************************/
    inline bool find(const std::string& a_KEY) const {
        return (this->map.find(a_KEY) != this->map.end());
    }

    /*************************************************************************/
    template <class T>
    inline T get(const std::string& a_KEY) const {
        const std::size_t INDEX = this->map.at(a_KEY);
        if (this->body[INDEX].second.type() == typeid(T)) {
            return std::any_cast<T>(this->body[INDEX].second);
        } else {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The type of " + this->body[INDEX].first + " is wrong."));
        }
    }
};

/*****************************************************************************/
struct JsonArray {
    JsonArrayBody body;
    int           indent_level;

    /*************************************************************************/
    JsonArray(void) {
        this->initialize();
    }

    /*************************************************************************/
    template <class T_Container>
    JsonArray(const T_Container& a_ARRAY) {
        this->setup(a_ARRAY);
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->body.clear();
        this->indent_level = 0;
    }

    /*************************************************************************/
    template <class T_Container>
    inline void setup(const T_Container& a_ARRAY) {
        for (const auto& value : a_ARRAY) {
            this->body.emplace_back(value);
        }
    }

    /*************************************************************************/
    template <class T>
    inline void emplace_back(const T& a_VALUE) {
        this->body.emplace_back(a_VALUE);
    }

    /*************************************************************************/
    template <class T>
    inline void emplace_back(const std::string& a_KEY, const T& a_VALUE) {
        JsonObject obj(a_KEY, a_VALUE);
        this->body.emplace_back(obj);
    }

    /*************************************************************************/
    inline std::any& operator[](const std::size_t a_INDEX) {
        return this->body[a_INDEX];
    }

    /*************************************************************************/
    template <class T>
    inline T get(const std::size_t a_INDEX) const {
        if (this->body[a_INDEX].type() == typeid(T)) {
            return std::any_cast<T>(this->body[a_INDEX]);
        } else {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__, "A cast error has occurred."));
        }
    }
};

/*****************************************************************************/
inline bool is_integer(const std::string& a_ARG) {
    for (std::size_t i = 0; i < a_ARG.size(); i++) {
        const auto& c = a_ARG[i];
        if (i == 0) {
            if ((std::isdigit(c) == 0) && (c != '+') && (c != '-')) {
                return false;
            }
        } else {
            if (std::isdigit(c) == 0) {
                return false;
            }
        }
    }

    return true;
}

/*****************************************************************************/
inline std::string indent_space(const int a_INDENT_LEVEL) {
    std::string result = "";
    for (auto i = 0; i < a_INDENT_LEVEL; i++) {
        result += "    ";
    }

    return result;
}

/*****************************************************************************/
inline std::ostream& operator<<(std::ostream& os, const JsonArray& a_JSON);

/*****************************************************************************/
inline std::ostream& operator<<(std::ostream& os, const JsonObject& a_JSON) {
    int         indent_level = a_JSON.indent_level;
    std::string separator    = "";

    if (a_JSON.body.empty()) {
        return os << "{}";
    }

    os << "{ ";
    indent_level++;
    for (const auto& element : a_JSON.body) {
        os << separator << std::endl;
        os << indent_space(indent_level) << '\"' << element.first << "\": ";

        if (element.second.type() == typeid(void)) {
            /**
             * Write void type element as "null".
             */
            os << "null";
        } else if (element.second.type() == typeid(short)) {
            /**
             * Write short type element.
             */
            os << std::any_cast<short>(element.second);
        } else if (element.second.type() == typeid(unsigned short)) {
            /**
             * Write unsigned short type element.
             */
            os << std::any_cast<unsigned short>(element.second);
        } else if (element.second.type() == typeid(int)) {
            /**
             * Write int type element.
             */
            os << std::any_cast<int>(element.second);
        } else if (element.second.type() == typeid(unsigned int)) {
            /**
             * Write unsigned int type element.
             */
            os << std::any_cast<unsigned int>(element.second);
        } else if (element.second.type() == typeid(long)) {
            /**
             * Write long type element.
             */
            os << std::any_cast<long>(element.second);
        } else if (element.second.type() == typeid(unsigned long)) {
            /**
             * Write unsigned long type element.
             */
            os << std::any_cast<unsigned long>(element.second);
        } else if (element.second.type() == typeid(long long)) {
            /**
             * Write long long type element.
             */
            os << std::any_cast<long long>(element.second);
        } else if (element.second.type() == typeid(unsigned long long)) {
            /**
             * Write unsigned long long type element.
             */
            os << std::any_cast<unsigned long long>(element.second);
        } else if (element.second.type() == typeid(float)) {
            /**
             * Write float type element.
             */
            os << std::scientific << std::any_cast<float>(element.second);
        } else if (element.second.type() == typeid(double)) {
            /**
             * Write double type element.
             */
            os << std::scientific << std::any_cast<double>(element.second);
        } else if (element.second.type() == typeid(long double)) {
            /**
             * Write long double type element.
             */
            os << std::scientific << std::any_cast<long double>(element.second);
        } else if (element.second.type() == typeid(bool)) {
            /**
             * Write bool type element as true or false.
             */
            os << (std::any_cast<bool>(element.second) ? "true" : "false");
        } else if (element.second.type() == typeid(std::string)) {
            /**
             * Write std::string type element with quotation.
             */
            os << "\"" << std::any_cast<std::string>(element.second) << '\"';
        } else if (element.second.type() == typeid(JsonObject)) {
            /**
             * Write Json type element in deeper indent level.
             */
            JsonObject object   = std::any_cast<JsonObject>(element.second);
            object.indent_level = indent_level;
            os << object;
        } else if (element.second.type() == typeid(JsonArray)) {
            /**
             * Write Array type element in deeper indent level.
             */
            JsonArray array    = std::any_cast<JsonArray>(element.second);
            array.indent_level = indent_level;
            os << array;
        } else {
            /**
             * Write other type element as "null".
             */
            os << "null";
        }
        separator = ", ";
    }
    os << std::endl;
    indent_level--;

    return os << indent_space(indent_level) << "}";
}

/*****************************************************************************/
inline int close_count(const std::string& a_ARG, const char a_LEFT,
                       const char a_RIGHT) {
    int  count           = 0;
    bool is_in_quotation = false;
    for (const auto& c : a_ARG) {
        if (is_in_quotation) {
            if (c == '\"') {
                is_in_quotation = false;
            }
        } else {
            if (c == '\"') {
                is_in_quotation = true;
            } else {
                if (c == a_LEFT) {
                    count++;
                } else if (c == a_RIGHT) {
                    count--;
                }
            }
        }
    }

    return count;
}

/*****************************************************************************/
inline std::ostream& operator<<(std::ostream& os, const JsonArray& a_ARRAY) {
    int         indent_level = a_ARRAY.indent_level;
    std::string separator    = "";

    if (a_ARRAY.body.empty()) {
        return os << "[]";
    }

    os << "[ ";
    indent_level++;
    for (const auto& element : a_ARRAY.body) {
        os << separator << std::endl;
        if (element.type() == typeid(void)) {
            /**
             * Write void type element as "null".
             */
            os << indent_space(indent_level) << "null";
        } else if (element.type() == typeid(short)) {
            /**
             * Write short type element.
             */
            os << indent_space(indent_level) << std::any_cast<short>(element);
        } else if (element.type() == typeid(unsigned short)) {
            /**
             * Write unsigned short type element.
             */
            os << indent_space(indent_level)
               << std::any_cast<unsigned short>(element);
        } else if (element.type() == typeid(int)) {
            /**
             * Write int type element.
             */
            os << indent_space(indent_level) << std::any_cast<int>(element);
        } else if (element.type() == typeid(unsigned int)) {
            /**
             * Write unsigned int type element.
             */
            os << indent_space(indent_level)
               << std::any_cast<unsigned int>(element);
        } else if (element.type() == typeid(long)) {
            /**
             * Write long type element.
             */
            os << indent_space(indent_level) << std::any_cast<long>(element);
        } else if (element.type() == typeid(unsigned long)) {
            /**
             * Write unsigned long type element.
             */
            os << indent_space(indent_level)
               << std::any_cast<unsigned long>(element);
        } else if (element.type() == typeid(long long)) {
            /**
             * Write long long type element.
             */
            os << indent_space(indent_level) << std::any_cast<long>(element);
        } else if (element.type() == typeid(unsigned long long)) {
            /**
             * Write unsigned long long type element.
             */
            os << indent_space(indent_level) << std::any_cast<long>(element);
        } else if (element.type() == typeid(float)) {
            /**
             * Write float type element.
             */
            os << indent_space(indent_level) << std::scientific
               << std::any_cast<float>(element);
        } else if (element.type() == typeid(double)) {
            /**
             * Write double type element.
             */
            os << indent_space(indent_level) << std::scientific
               << std::any_cast<double>(element);
        } else if (element.type() == typeid(long double)) {
            /**
             * Write long double type element.
             */
            os << indent_space(indent_level) << std::scientific
               << std::any_cast<long double>(element);
        } else if (element.type() == typeid(bool)) {
            /**
             * Write bool type element as true or false.
             */
            os << indent_space(indent_level)
               << (std::any_cast<bool>(element) ? "true" : "false");
        } else if (element.type() == typeid(std::string)) {
            /**
             * Write std::string type element with quotation.
             */
            os << indent_space(indent_level) << '\"'
               << std::any_cast<std::string>(element) << '\"';
        } else if (element.type() == typeid(JsonObject)) {
            /**
             * Write Json type element in deeper indent level.
             */
            JsonObject object   = std::any_cast<JsonObject>(element);
            object.indent_level = indent_level;
            os << indent_space(indent_level) << object;
        } else if (element.type() == typeid(JsonArray)) {
            /**
             * Write Array type element in deeper indent level.
             */
            JsonArray array    = std::any_cast<JsonArray>(element);
            array.indent_level = indent_level;
            os << indent_space(indent_level) << array;
        } else {
            /**
             * Write other type element as "null".
             */
            os << indent_space(indent_level) << "null";
        }
        separator = ", ";
    }
    os << std::endl;
    indent_level--;

    return os << indent_space(indent_level) << "]";
}

/*****************************************************************************/
inline std::vector<std::string> tokenize(const std::string& a_ARG) {
    std::vector<std::pair<std::string, std::string>> rules;
    rules.emplace_back("\r\n", "");
    rules.emplace_back(",", " ");
    rules.emplace_back(":", " ");
    rules.emplace_back("\\{", "{ ");
    rules.emplace_back("\\}", " }");
    rules.emplace_back("\\[", "[ ");
    rules.emplace_back("\\]", " ]");

    std::string preprocessed = a_ARG;
    for (const auto& rule : rules) {
        preprocessed = std::regex_replace(preprocessed, std::regex(rule.first),
                                          rule.second);
    }

    std::vector<std::string> tokens;

    std::size_t position             = 0;
    std::size_t token_start_position = 0;
    bool        is_token_open        = false;
    bool        is_quotation_open    = false;

    while (position < preprocessed.size()) {
        if (preprocessed[position] == '\"') {
            is_quotation_open = !is_quotation_open;
        }

        if (preprocessed[position] == ' ' || preprocessed[position] == '\t') {
            if (is_token_open && !is_quotation_open) {
                tokens.push_back(preprocessed.substr(
                    token_start_position, position - token_start_position));
                is_token_open = false;
            }
        } else {
            if (!is_token_open) {
                token_start_position = position;
                is_token_open        = true;
            }
        }
        position++;
    }

    if (is_token_open) {
        const auto TOKEN = preprocessed.substr(
            token_start_position, preprocessed.size() - token_start_position);
        tokens.push_back(TOKEN);
    }

    return tokens;
}

/*****************************************************************************/
inline std::size_t find_end_position(const std::vector<std::string>& a_TOKENS,
                                     const std::size_t  a_START_POSITION,
                                     const std::string& a_LEFT,
                                     const std::string& a_RIGHT) {
    int count = 0;
    for (std::size_t i = a_START_POSITION; i < a_TOKENS.size(); i++) {
        const auto& token = a_TOKENS[i];
        if (token == a_LEFT) {
            count++;
        } else if (token == a_RIGHT) {
            count--;
            if (count == 0) {
                return i;
            }
        }
    }

    return 0;
}

/*****************************************************************************/
inline JsonArray parse_json_array(const std::vector<std::string>& a_TOKENS,
                                  const std::size_t a_START_POSITION = 0,
                                  const std::size_t a_END_POSITION   = 0);

/*****************************************************************************/
inline JsonObject parse_json_object(const std::vector<std::string>& a_TOKENS,
                                    const std::size_t a_START_POSITION = 0,
                                    const std::size_t a_END_POSITION   = 0) {
    JsonObject        object;
    std::string       key;
    std::size_t       position = a_START_POSITION + 1;
    const std::size_t END_POSITION =
        a_END_POSITION > 0 ? a_END_POSITION : a_TOKENS.size() - 1;

    ReadMode read_mode = ReadMode::Key;

    while (position < END_POSITION) {
        const auto& TOKEN = a_TOKENS[position];

        if (read_mode == ReadMode::Key) {
            key = TOKEN.substr(1, TOKEN.size() - 2);
            position++;
            read_mode = ReadMode::Value;
            continue;
        }

        if (read_mode == ReadMode::Value) {
            if (TOKEN == "{") {
                const std::size_t SUB_END_POSITION =
                    find_end_position(a_TOKENS, position, "{", "}");
                object.emplace_back(  //
                    key,              //
                    parse_json_object(a_TOKENS, position, SUB_END_POSITION));
                position  = SUB_END_POSITION + 1;
                read_mode = ReadMode::Key;
                continue;
            }

            if (TOKEN == "[") {
                const std::size_t SUB_END_POSITION =
                    find_end_position(a_TOKENS, position, "[", "]");
                object.emplace_back(  //
                    key,              //
                    parse_json_array(a_TOKENS, position, SUB_END_POSITION));
                position  = SUB_END_POSITION + 1;
                read_mode = ReadMode::Key;
                continue;
            }

            if (TOKEN[0] == '\"') {
                /**
                 * Store as std::string value.
                 */
                object.emplace_back(key, TOKEN.substr(1, TOKEN.size() - 2));
            } else if (TOKEN == "null") {
                /**
                 * Store nullptr for null.
                 */
                object.emplace_back(key, nullptr);
            } else if (TOKEN == "true") {
                /**
                 * Store as true boolean value.
                 */
                object.emplace_back(key, true);
            } else if (TOKEN == "false") {
                /**
                 * Store as false boolean value.
                 */
                object.emplace_back(key, false);
            } else if (is_integer(TOKEN)) {
                /**
                 * Store as integer value.
                 */
                object.emplace_back(key, std::stoi(TOKEN));
            } else {
                /**
                 * Store as double value.
                 */
                object.emplace_back(key, std::stod(TOKEN));
            }

            position++;
            read_mode = ReadMode::Key;
        }
    }

    return object;
}

/*****************************************************************************/
inline JsonArray parse_json_array(const std::vector<std::string>& a_TOKENS,
                                  const std::size_t a_START_POSITION,
                                  const std::size_t a_END_POSITION) {
    JsonArray         array;
    std::size_t       position = a_START_POSITION + 1;
    const std::size_t END_POSITION =
        a_END_POSITION > 0 ? a_END_POSITION : a_TOKENS.size() - 1;

    while (position < END_POSITION) {
        const auto& TOKEN = a_TOKENS[position];

        if (TOKEN == "{") {
            const std::size_t SUB_END_POSITION =
                find_end_position(a_TOKENS, position, "{", "}");
            array.emplace_back(
                parse_json_object(a_TOKENS, position, SUB_END_POSITION));
            position = SUB_END_POSITION + 1;
            continue;
        }

        if (TOKEN == "[") {
            const std::size_t SUB_END_POSITION =
                find_end_position(a_TOKENS, position, "[", "]");
            array.emplace_back(
                parse_json_array(a_TOKENS, position, SUB_END_POSITION));
            position = SUB_END_POSITION + 1;
            continue;
        }

        if (TOKEN[0] == '\"') {
            /**
             * Store as std::string value.
             */
            array.emplace_back(TOKEN.substr(1, TOKEN.size() - 2));
        } else if (TOKEN == "null") {
            /**
             * Store nullptr for null.
             */
            array.emplace_back(nullptr);
        } else if (TOKEN == "true") {
            /**
             * Store as true boolean value.
             */
            array.emplace_back(std::string("true"));
        } else if (TOKEN == "false") {
            /**
             * Store as false boolean value.
             */
            array.emplace_back(std::string("false"));
        } else if (is_integer(TOKEN)) {
            /**
             * Store as integer value.
             */
            array.emplace_back(std::stoi(TOKEN));
        } else {
            /**
             * Store as double value.
             */
            array.emplace_back(std::stod(TOKEN));
        }

        position++;
    }

    return array;
}

/*****************************************************************************/
inline JsonObject parse_json_object(const std::string& a_ARG) {
    const auto TOKENS = tokenize(a_ARG);
    return parse_json_object(TOKENS);
}

/*****************************************************************************/
inline JsonArray parse_json_array(const std::string& a_ARG) {
    const auto TOKENS = tokenize(a_ARG);
    return parse_json_array(TOKENS);
}

/*****************************************************************************/
inline void write_json_object(const JsonObject&  a_OBJECT,
                              const std::string& a_FILE_NAME) {
    std::ofstream ofs(a_FILE_NAME);
    ofs << a_OBJECT;
    ofs.close();
}

/*****************************************************************************/
inline void write_json_array(const JsonArray&   a_ARRAY,
                             const std::string& a_FILE_NAME) {
    std::ofstream ofs(a_FILE_NAME);
    ofs << a_ARRAY;
    ofs.close();
}

/*****************************************************************************/
inline JsonObject read_json_object(const std::string& a_FILE_NAME) {
    std::string   str;
    std::ifstream ifs;
    std::string   buffer;

    ifs.open(a_FILE_NAME.c_str());

    if (ifs.fail()) {
        throw std::runtime_error("Cannot open the specified file: " +
                                 a_FILE_NAME);
    }

    while (std::getline(ifs, buffer)) {
        str += buffer;
    }

    ifs.close();

    if (close_count(str, '{', '}') != 0) {
        throw std::runtime_error("Specified JSON file is not valid: " +
                                 a_FILE_NAME);
    }

    if (close_count(str, '[', ']') != 0) {
        throw std::runtime_error("Specified JSON file is not valid: " +
                                 a_FILE_NAME);
    }

    const auto TOKENS = tokenize(str);

    if (TOKENS.front() != "{" || TOKENS.back() != "}") {
        throw std::runtime_error("Specified JSON file is not valid: " +
                                 a_FILE_NAME);
    }

    return parse_json_object(TOKENS);
}

/*****************************************************************************/
inline JsonArray read_json_array(const std::string& a_FILE_NAME) {
    std::string   str;
    std::ifstream ifs;
    std::string   buffer;

    ifs.open(a_FILE_NAME.c_str());

    if (ifs.fail()) {
        throw std::runtime_error("Cannot open the specified file: " +
                                 a_FILE_NAME);
    }

    while (std::getline(ifs, buffer)) {
        str += buffer;
    }

    ifs.close();

    if (close_count(str, '{', '}') != 0) {
        throw std::runtime_error("Specified JSON file is not valid: " +
                                 a_FILE_NAME);
    }

    if (close_count(str, '[', ']') != 0) {
        throw std::runtime_error("Specified JSON file is not valid: " +
                                 a_FILE_NAME);
    }

    const auto TOKENS = tokenize(str);

    if (TOKENS.front() != "[" || TOKENS.back() != "]") {
        throw std::runtime_error("Specified JSON file is not valid: " +
                                 a_FILE_NAME);
    }

    return parse_json_array(TOKENS);
}
}  // namespace printemps::utility::json
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
