# ScanDynamic

> Типобезопасная функция `scan` — обратная к `std::format`: разбирает строку по формату в значения заданных типов, с интерпретацией формата в runtime.

`stdx::scan<Ts...>(input, format)` принимает входную строку и строку формата, разбирает плейсхолдеры и возвращает кортеж значений запрошенных типов, обёрнутый в `std::expected` — с понятной ошибкой вместо исключения при несовпадении формата или неверном вводе.

## Возможности

- Извлечение значений нескольких типов из строки по формату за один вызов.
- Проверка соответствия числа плейсхолдеров и числа запрошенных типов.
- Runtime-интерпретация спецификаторов формата для каждого значения.
- Возврат результата или структурированной ошибки без исключений (`std::expected`).

## Технологии и концепции C++

- **Variadic templates:** `template <AllowedTypes... Ts>`, разбор и заполнение результата для произвольного числа типов.
- **C++23 `std::expected`** для обработки ошибок без исключений (`std::unexpected`).
- **Fold-expressions + `std::index_sequence`** для поэлементного разбора и записи в кортеж результата.
- **Концепты** (`AllowedTypes`) для ограничения допустимых типов.
- `std::type_identity`, `std::integral_constant`, `std::decay_t` — работа с типами на этапе компиляции.

## Архитектура

```
stdx::scan<Ts...>(input, format)
  ├─ parse_sources(input, format)        — разбор формата и входной строки
  ├─ проверка: |плейсхолдеры| == sizeof...(Ts)
  └─ fold по index_sequence:
       parse_value_with_format<T>(value, fmt) → std::get<I>(result)
  → std::expected<scan_result<Ts...>, scan_error>
```

## Стек

`C++23` (variadic templates, concepts, `std::expected`) · `CMake` · `CTest` · `GoogleTest`

## Сборка

```bash
mkdir build && cd build
cmake ..
make -j4
```

## Тесты

```bash
cd build
ctest --verbose
```
