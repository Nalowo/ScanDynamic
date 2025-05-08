#pragma once

#include "parse.hpp"
#include "types.hpp"

namespace stdx
{
    template <details::AllowedTypes... Ts>
    std::expected<details::scan_result<std::decay_t<Ts>...>, details::scan_error>
    scan(std::string_view input, std::string_view format)
    {
        auto list = details::parse_sources(input, format);
        if (!list.has_value())
            return std::unexpected(list.error());

        details::scan_result<std::decay_t<Ts>...> results;
        const auto &[fmts, values] = *list;
        
        constexpr std::size_t N = sizeof...(Ts);
        if (N != fmts.size())
            return std::unexpected( details::scan_error{ "Number of placeholders and input types are not equal" });

        bool success = true;
        std::string error_msg;

        auto try_parse = [&](auto I, auto type_tag)
        {
            using T = typename decltype(type_tag)::type;
            auto result = details::parse_value_with_format<T>(values[I], fmts[I]);
            if (!result.has_value())
            {
                success = false;
                error_msg = result.error().message;
                return;
            }

            std::get<I>(results._values) = std::move(result.value());
        };

        [&]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            (try_parse(std::integral_constant<std::size_t, Is>{}, std::type_identity<Ts>{}), ...);
        }(std::index_sequence_for<Ts...>{});

        if (!success)
            return std::unexpected(details::scan_error{error_msg});

        return results;
    }
} // namespace stdx
