#pragma once
#include <tuple>

namespace stdx::details
{
    namespace
    {
        template <typename T, typename... Ts>
        constexpr bool is_one_of = (std::is_same_v<T, Ts> || ...);
    }

    template <typename T>
    concept AllowedTypes = is_one_of<
        std::decay_t<T>,
        int8_t, int16_t, int32_t, int64_t,
        uint8_t, uint16_t, uint32_t, uint64_t,
        float, double, std::string, std::string_view>;

    // Класс для хранения ошибки неуспешного сканирования
    struct scan_error
    {
        std::string message;
    };

    // Шаблонный класс для хранения результатов успешного сканирования
    template <AllowedTypes... Ts>
    struct scan_result
    {
        std::tuple<Ts...> _values;

        template <std::size_t N>
        auto &values()
        {
            static_assert(N < sizeof...(Ts), "Index out of bounds");
            return std::get<N>(_values);
        }

        template <AllowedTypes T>
        auto &values()
        {
            return std::get<T>(_values);
        }
    };
} // namespace stdx::details
