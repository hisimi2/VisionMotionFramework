#pragma once

#include <string>
#include <sstream>

namespace EC
{
    template <typename T>
    struct ParamConverter
    {
        static bool Convert(const std::string& text, T& value)
        {
            std::istringstream iss(text);
            T temp;
            iss >> temp;

            if (iss.fail())
                return false;

            value = temp;
            return true;
        }
    };

    template <>
    struct ParamConverter<std::string>
    {
        static bool Convert(const std::string& text, std::string& value)
        {
            value = text;
            return true;
        }
    };

    template <>
    struct ParamConverter<int>
    {
        static bool Convert(const std::string& text, int& value)
        {
            if (text.empty())
                return false;

            char* endPtr = nullptr;
            const long parsed = std::strtol(text.c_str(), &endPtr, 10);

            if (endPtr == text.c_str() || *endPtr != '\0')
                return false;

            value = static_cast<int>(parsed);
            return true;
        }
    };

    template <>
    struct ParamConverter<double>
    {
        static bool Convert(const std::string& text, double& value)
        {
            if (text.empty())
                return false;

            char* endPtr = nullptr;
            const double parsed = std::strtod(text.c_str(), &endPtr);

            if (endPtr == text.c_str() || *endPtr != '\0')
                return false;

            value = parsed;
            return true;
        }
    };

    template <>
    struct ParamConverter<bool>
    {
        static bool Convert(const std::string& text, bool& value)
        {
            if (text == "1" || text == "true" || text == "TRUE" || text == "True")
            {
                value = true;
                return true;
            }

            if (text == "0" || text == "false" || text == "FALSE" || text == "False")
            {
                value = false;
                return true;
            }

            return false;
        }
    };

    template <typename T>
    struct ParamFormatter
    {
        static std::string Format(const T& value)
        {
            std::ostringstream oss;
            oss << value;
            return oss.str();
        }
    };

    template <>
    struct ParamFormatter<std::string>
    {
        static std::string Format(const std::string& value)
        {
            return value;
        }
    };

    template <>
    struct ParamFormatter<const char*>
    {
        static std::string Format(const char* value)
        {
            return value != nullptr ? std::string(value) : std::string();
        }
    };

    template <>
    struct ParamFormatter<bool>
    {
        static std::string Format(const bool& value)
        {
            return value ? "true" : "false";
        }
    };
}
