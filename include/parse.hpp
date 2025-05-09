#pragma once

#include <expected>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <cmath>

#include "types.hpp"

namespace stdx::details
{
    template <typename T>
        requires std::same_as<T, std::string> || std::same_as<T, std::string_view>
    constexpr std::expected<T, scan_error> parse_value(std::string_view input, std::string_view fmt)
    {
        if (!fmt.empty() && fmt != "%s")
            return std::unexpected(scan_error{"Unexpected format"});

        if constexpr (std::same_as<T, std::string>)
            return std::string(input);
        else
            return std::string_view(input);
    }

    template <typename T>
    auto from_chars_value(std::string_view input, T& value)
    {
        if constexpr (std::is_integral_v<T>)
            return std::from_chars(input.data(), input.data() + input.size(), value, 10);
        else if constexpr (std::is_floating_point_v<T>)
            return std::from_chars(input.data(), input.data() + input.size(), value);
        else
            static_assert(std::is_arithmetic_v<T>, "Unsupported type for from_chars");
    }

    template <typename T>
        requires std::is_arithmetic_v<T>
    constexpr std::expected<T, scan_error> parse_value(std::string_view input, std::string_view fmt)
    {
        if (!fmt.empty() && (fmt != "%d" && fmt != "%u" && fmt != "%f"))
            return std::unexpected(scan_error{"Unexpected format"});

        T value{};
        auto [ptr, ec] = from_chars_value(input, value);
        if (ec == std::errc::invalid_argument)
            return std::unexpected(scan_error{"Invalid input: invalid argument"});
        else if (ec == std::errc::result_out_of_range)
            return std::unexpected(scan_error{"Value out of range"});
        else if (ptr == input.data())
            return std::unexpected(scan_error{"Nothing parsed"});

        if constexpr (std::is_integral_v<T>)
        {
            auto number_str = std::to_string(value);
            if (number_str.size() != input.size())
                return std::unexpected(scan_error{"Invalid input: not a whole input transform"});
        }
        
        return value;
    }

    // Функция для парсинга значения с учетом спецификатора формата
    template <AllowedTypes T>
    constexpr std::expected<T, scan_error> parse_value_with_format(std::string_view input, std::string_view fmt)
    {
        if (input.empty())
            return std::unexpected(scan_error{"Nothing parsed"});
        return parse_value<std::decay_t<T>>(input, fmt);
    }

    // Функция для проверки корректности входных данных и выделения из обеих строк интересующих данных для парсинга
    std::expected<std::pair<std::vector<std::string_view>, std::vector<std::string_view>>, scan_error>
    parse_sources(std::string_view input, std::string_view format)
    {
        std::vector<std::string_view> format_parts; // Части формата между {}
        std::vector<std::string_view> input_parts;
        size_t start = 0;
        while (true)
        {
            size_t open = format.find('{', start);
            if (open == std::string_view::npos)
            {
                break;
            }
            size_t close = format.find('}', open);
            if (close == std::string_view::npos)
            {
                break;
            }

            // Если между предыдущей } и текущей { есть текст,
            // проверяем его наличие во входной строке
            if (open > start)
            {
                std::string_view between = format.substr(start, open - start);
                auto pos = input.find(between);
                if (input.size() < between.size() || pos == std::string_view::npos)
                {
                    return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
                }
                if (start != 0)
                {
                    input_parts.emplace_back(input.substr(0, pos));
                }

                input = input.substr(pos + between.size());
            }

            // Сохраняем спецификатор формата (то, что между {})
            format_parts.push_back(format.substr(open + 1, close - open - 1));
            start = close + 1;
        }

        // Проверяем оставшийся текст после последней }
        if (start < format.size())
        {
            std::string_view remaining_format = format.substr(start);
            auto pos = input.find(remaining_format);
            if (input.size() < remaining_format.size() || pos == std::string_view::npos)
            {
                return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
            }
            input_parts.emplace_back(input.substr(0, pos));
            input = input.substr(pos + remaining_format.size());
        }
        else
        {
            input_parts.emplace_back(input);
        }
        return std::pair{std::move(format_parts), std::move(input_parts)};
    }
} // namespace stdx::details